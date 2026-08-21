--- asyncssh/connection.py.orig
+++ asyncssh/connection.py
@@ -2719,6 +2719,9 @@
         send_chan = packet.get_uint32()
         send_window = packet.get_uint32()
         send_pktsize = packet.get_uint32()
+
+        if send_pktsize == 0:
+            raise ProtocolError('Invalid maximum packet size')
 
         # Work around an off-by-one error in dropbear introduced in
         # https://github.com/mkj/dropbear/commit/49263b5
@@ -2757,6 +2760,9 @@
         send_window = packet.get_uint32()
         send_pktsize = packet.get_uint32()
 
+        if send_pktsize == 0:
+            raise ProtocolError('Invalid maximum packet size')
+
         # Work around an off-by-one error in dropbear introduced in
         # https://github.com/mkj/dropbear/commit/49263b5
         if b'dropbear' in self._server_version and self._compressor:
