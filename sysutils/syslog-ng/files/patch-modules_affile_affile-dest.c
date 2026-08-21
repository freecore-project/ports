--- modules/affile/affile-dest.c.orig	2025-10-14 12:39:36 UTC
+++ modules/affile/affile-dest.c
@@ -483,6 +483,23 @@
     case NC_CLOSE:
       affile_dw_reap(self);
       break;
+    case NC_WRITE_ERROR:
+      /* The descriptor can be unusable for good: a revoked tty (FreeBSD
+       * revokes /dev/console when /etc/rc exits), a removed device node.
+       * LogWriter would retry that same descriptor every time_reopen()
+       * forever and, when the error is noticed while idle, stop without
+       * releasing the messages it holds back for flow control, which
+       * suspends the sources feeding this destination.
+       *
+       * Drop the file instead, as network destinations drop a broken
+       * connection.  LogWriter then asks for it to be opened again after
+       * time_reopen() (NC_REOPEN_REQUIRED), and affile_dw_queue() does so
+       * earlier if a message arrives once that time has passed. */
+      msg_notice("Write error on destination file, closing it to reopen after time-reopen()",
+                 evt_tag_str("filename", self->filename),
+                 evt_tag_int("time_reopen", self->owner->writer_options.time_reopen));
+      log_writer_reopen(self->writer, NULL);
+      break;
     case NC_LOGROTATE:
     {
       if (logrotate_is_enabled(&(self->owner->logrotate_options)))
