--- asyncssh/connection.py.orig
+++ asyncssh/connection.py
@@ -258,6 +258,9 @@
 
 # Max allowed username length
 _MAX_USERNAME_LEN = 1024
+
+# Max receive packet length (including framing and padding)
+_DEFAULT_RECV_PKTLEN = 256*1024     # 256 KiB
 
 # Default rekey parameters
 _DEFAULT_REKEY_BYTES = 1 << 30      # 1 GiB
@@ -1012,6 +1015,7 @@
 
         self._channels: Dict[int, SSHChannel] = {}
         self._next_recv_chan = 0
+        self._max_recv_pktlen = _DEFAULT_RECV_PKTLEN
 
         self._global_request_queue: List[_GlobalRequest] = []
         self._global_request_waiters: \
@@ -1067,6 +1071,11 @@
 
         return msg_bytes.decode('utf-8', self._utf8_decode_errors)
 
+    def _update_recv_pktlen(self, max_pktsize: int) -> None:
+        """Update max receive packet len based on per-channel max_pktsize"""
+
+        self._max_recv_pktlen = max(self._max_recv_pktlen, max_pktsize + 1024)
+
     def _cleanup(self, exc: Optional[Exception]) -> None:
         """Clean up this connection"""
 
@@ -1623,6 +1632,10 @@
             pktlen = self._packet[:4]
 
         self._pktlen = int.from_bytes(pktlen, 'big')
+
+        if self._pktlen > self._max_recv_pktlen:
+            raise ProtocolError('Max packet size exceeded')
+
         self._recv_handler = self._recv_packet
         return True
 
@@ -2720,6 +2733,9 @@
         send_window = packet.get_uint32()
         send_pktsize = packet.get_uint32()
 
+        if send_pktsize == 0:
+            raise ProtocolError('Invalid maximum packet size')
+
         # Work around an off-by-one error in dropbear introduced in
         # https://github.com/mkj/dropbear/commit/49263b5
         if b'dropbear' in self._client_version and self._compressor:
@@ -2756,6 +2772,9 @@
         send_chan = packet.get_uint32()
         send_window = packet.get_uint32()
         send_pktsize = packet.get_uint32()
+
+        if send_pktsize == 0:
+            raise ProtocolError('Invalid maximum packet size')
 
         # Work around an off-by-one error in dropbear introduced in
         # https://github.com/mkj/dropbear/commit/49263b5
@@ -3050,6 +3069,8 @@
 
         """
 
+        self._update_recv_pktlen(max_pktsize)
+
         return SSHTCPChannel(self, self._loop, encoding,
                              errors, window, max_pktsize)
 
@@ -3085,6 +3106,8 @@
 
         """
 
+        self._update_recv_pktlen(max_pktsize)
+
         return SSHUNIXChannel(self, self._loop, encoding,
                               errors, window, max_pktsize)
 
@@ -3110,6 +3133,8 @@
 
         """
 
+        self._update_recv_pktlen(max_pktsize)
+
         return SSHTunTapChannel(self, self._loop, None, 'strict',
                                 window, max_pktsize)
 
@@ -3118,6 +3143,8 @@
             max_pktsize: int = _DEFAULT_MAX_PKTSIZE) -> SSHX11Channel:
         """Create an SSH X11 channel to use in X11 forwarding"""
 
+        self._update_recv_pktlen(max_pktsize)
+
         return SSHX11Channel(self, self._loop, None, 'strict',
                              window, max_pktsize)
 
@@ -3125,6 +3152,8 @@
             self, window: int = _DEFAULT_WINDOW,
             max_pktsize: int = _DEFAULT_MAX_PKTSIZE) -> SSHAgentChannel:
         """Create an SSH agent channel to use in agent forwarding"""
+
+        self._update_recv_pktlen(max_pktsize)
 
         return SSHAgentChannel(self, self._loop, None, 'strict',
                                window, max_pktsize)
@@ -4404,6 +4433,8 @@
         window: int
         max_pktsize: int
 
+        self._update_recv_pktlen(max_pktsize)
+
         chan = SSHClientChannel(self, self._loop, self._utf8_decode_errors,
                                 encoding, errors, window, max_pktsize)
 
@@ -6986,14 +7017,18 @@
            :returns: :class:`SSHServerChannel`
 
         """
+
+        if not max_pktsize:
+            max_pktsize = self._max_pktsize
 
+        self._update_recv_pktlen(max_pktsize)
+
         return SSHServerChannel(self, self._loop, self._allow_pty,
                                 self._line_editor, self._line_echo,
                                 self._line_history, self._max_line_length,
                                 self._encoding if encoding == '' else encoding,
                                 self._errors if errors == '' else errors,
-                                window or self._window,
-                                max_pktsize or self._max_pktsize)
+                                window or self._window, max_pktsize)
 
     async def create_connection(
             self, session_factory: SSHTCPSessionFactory[AnyStr],
