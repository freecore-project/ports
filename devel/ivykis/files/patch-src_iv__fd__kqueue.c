--- src/iv_fd_kqueue.c.orig	2026-09-02 23:32:58 UTC
+++ src/iv_fd_kqueue.c	2026-09-02 23:32:58 UTC
@@ -91,15 +91,54 @@
 	*_num = num;
 }
 
+/*
+ * Apply a changelist with EV_RECEIPT so kqueue reports a result for
+ * every change.  Without an eventlist, kevent() stops at the first
+ * failure and returns -1.
+ *
+ * Tolerate ENOENT/EBADF for EV_DELETE: the kernel may already have
+ * auto-removed that filter (e.g. pipe EOF/close race).  Other errors
+ * remain fatal.
+ */
 static int __kevent_retry(int kq, const struct kevent *changelist, int nchanges)
 {
+	if (nchanges <= 0)
+		return 0;
+
+	struct kevent changes[nchanges];
+	for (int i = 0; i < nchanges; i++) {
+		changes[i] = changelist[i];
+		changes[i].flags |= EV_RECEIPT;
+	}
+
 	struct timespec to = { 0, 0 };
 	int ret;
-
 	do {
-		ret = kevent(kq, changelist, nchanges, NULL, 0, &to);
+		ret = kevent(kq, changes, nchanges, changes, nchanges, &to);
 	} while (ret < 0 && errno == EINTR);
 
+	if (ret < 0)
+		return ret;
+
+	for (int i = 0; i < ret; i++) {
+		if (!(changes[i].flags & EV_ERROR))
+			continue;
+		if (changes[i].data == 0)
+			continue;
+
+		/*
+		 * Tolerate "filter not present" results for EV_DELETE: the
+		 * kernel may have auto-removed the filter (e.g. on pipe EOF)
+		 * between our last upload and this teardown change.
+		 */
+		if ((changelist[i].flags & EV_DELETE) &&
+		    (changes[i].data == ENOENT || changes[i].data == EBADF))
+			continue;
+
+		errno = changes[i].data;
+		return -1;
+	}
+
 	return ret;
 }
 
@@ -205,23 +244,66 @@
 			continue;
 		}
 
-		if (batch[i].flags & EV_ERROR) {
-			int err = batch[i].data;
-			int fd = batch[i].ident;
+		/*
+		 * Per-fd error conditions are application-level, not internal
+		 * kqueue failures.  Mark the fd ready with MASKERR so the registered
+		 * error handler is dispatched by normal ready processing,
+		 * rather than bailing out, consistent with epoll's EPOLLERR/EPOLLHUP
+		 * handling.
+		 */
 
-			iv_fatal("iv_fd_kqueue_poll: got error %d[%s] "
-				 "polling fd %d", err, strerror(err), fd);
+		/*
+		 * EV_ERROR: kqueue reports per-change failures (e.g.
+		 * EVFILT_WRITE EV_ADD on a pipe whose read end is already
+		 * closed -> EPIPE on FreeBSD) via an eventlist entry with
+		 * EV_ERROR set and data = errno.  macOS happens to accept
+		 * the same registration and surface the condition through
+		 * EV_EOF on the write filter instead, so this branch is a
+		 * no-op there for that scenario, but the contract applies
+		 * to every kqueue platform.
+		 */
+		if (batch[i].flags & EV_ERROR) {
+			fd = (void *)batch[i].udata;
+			/*
+			 * The EV_ADD that produced this EV_ERROR was rejected
+			 * by the kernel, so the filter is NOT actually registered
+			 * in kqueue.  Clear the corresponding bit from
+			 * registered_bands to prevent a later EV_DELETE (issued
+			 * on unregister/close) from failing with ENOENT/EBADF
+			 * and tripping iv_fatal() in kevent_retry().
+			 */
+			if (batch[i].filter == EVFILT_READ) {
+				fd->registered_bands &= ~MASKIN;
+				iv_fd_make_ready(active, fd, MASKIN | MASKERR);
+			} else if (batch[i].filter == EVFILT_WRITE) {
+				fd->registered_bands &= ~MASKOUT;
+				iv_fd_make_ready(active, fd, MASKOUT | MASKERR);
+			} else {
+				fd->registered_bands &= ~(MASKIN | MASKOUT);
+				iv_fd_make_ready(active, fd, MASKIN | MASKOUT | MASKERR);
+			}
+			continue;
 		}
 
 		fd = (void *)batch[i].udata;
-		if (batch[i].filter == EVFILT_READ) {
-			iv_fd_make_ready(active, fd, MASKIN);
-		} else if (batch[i].filter == EVFILT_WRITE) {
-			iv_fd_make_ready(active, fd, MASKOUT);
-		} else {
-			iv_fatal("iv_fd_kqueue_poll: got message from "
-				 "filter %d", batch[i].filter);
+		if (batch[i].filter == EVFILT_READ)	{
+			int bands = MASKIN;
+			/* EV_EOF on read: write end closed; FreeBSD and macOS. */
+			if (batch[i].flags & EV_EOF)
+				bands |= MASKERR;
+			iv_fd_make_ready(active, fd, bands);
 		}
+		else if (batch[i].filter == EVFILT_WRITE) {
+			int bands = MASKOUT;
+			/* EV_EOF on write: read end closed (macOS); FreeBSD reports EPIPE via EV_ERROR. */
+			if (batch[i].flags & EV_EOF)
+				bands |= MASKERR;
+			iv_fd_make_ready(active, fd, bands);
+		}
+		else {
+			iv_fatal("iv_fd_kqueue_poll: got message from filter %d",
+							 batch[i].filter);
+		}
 	}
 
 	if (run_events)
