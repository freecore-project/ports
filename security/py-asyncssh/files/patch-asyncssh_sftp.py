--- asyncssh/sftp.py.orig
+++ asyncssh/sftp.py
@@ -3980,6 +3980,9 @@
                     if filename in (b'.', b'..'):
                         continue
 
+                    if b'/' in filename or b'\\' in filename:
+                        raise SFTPBadMessage('Invalid filename')
+
                     srcfile = posixpath.join(srcpath, filename)
                     dstfile = posixpath.join(dstpath, filename)
 
@@ -7202,7 +7205,7 @@
 
         if self._chroot:
             normpath = posixpath.normpath(posixpath.join(b'/', path))
-            return posixpath.join(self._chroot, normpath[1:])
+            return posixpath.join(self._chroot, normpath.lstrip(b'/'))
         else:
             return path
 
