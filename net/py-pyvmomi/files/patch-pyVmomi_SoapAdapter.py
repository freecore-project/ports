Python 3.12 removed ssl.wrap_socket and the HTTPSConnection key_file/cert_file
arguments.  Backport only the compatibility mechanics from upstream 44d7b9f,
preserving pyVmomi 7.0.3's existing proxy and tunnel behavior.

--- pyVmomi/SoapAdapter.py.orig	2021-11-19 08:31:12 UTC
+++ pyVmomi/SoapAdapter.py
@@ -999,8 +999,30 @@ try:
          if sha1Digest != thumbprint:
             raise ThumbprintMismatchException(thumbprint, sha1Digest)
 
-   # Function used to wrap sockets with SSL
-   _SocketWrapper = ssl.wrap_socket
+   # Function used to wrap sockets with SSL.  Python 3.12 removed the
+   # deprecated module-level ssl.wrap_socket() helper, so retain its Python
+   # 3.11 semantics locally while the 7.0 API remains supported.
+   def _SocketWrapper(rawSocket, keyfile=None, certfile=None,
+                      server_side=False, cert_reqs=ssl.CERT_NONE,
+                      ssl_version=ssl.PROTOCOL_TLS, ca_certs=None,
+                      do_handshake_on_connect=True,
+                      suppress_ragged_eofs=True, ciphers=None):
+      if server_side and not certfile:
+         raise ValueError("certfile must be specified for server-side operations")
+      if keyfile and not certfile:
+         raise ValueError("certfile must be specified")
+      context = ssl.SSLContext(ssl_version)
+      context.verify_mode = cert_reqs
+      if ca_certs:
+         context.load_verify_locations(ca_certs)
+      if certfile:
+         context.load_cert_chain(certfile, keyfile)
+      if ciphers:
+         context.set_ciphers(ciphers)
+      return context.wrap_socket(
+         sock=rawSocket, server_side=server_side,
+         do_handshake_on_connect=do_handshake_on_connect,
+         suppress_ragged_eofs=suppress_ragged_eofs)
 
 except ImportError:
    SSL_THUMBPRINTS_SUPPORTED = False
@@ -1026,9 +1048,19 @@ class _HTTPSConnection(http_client.HTTPSConnection):
       # and push back the params in connect()
       self._sslArgs = {}
       tmpKwargs = kwargs.copy()
+      self.key_file = tmpKwargs.pop('key_file', None)
+      self.cert_file = tmpKwargs.pop('cert_file', None)
+      if self.key_file and not self.cert_file:
+         raise ValueError("certfile must be specified")
       for key in SOAP_ADAPTER_ARGS:
          if key in tmpKwargs:
             self._sslArgs[key] = tmpKwargs.pop(key)
+      if self.cert_file:
+         context = tmpKwargs.get('context')
+         if context is None:
+            context = ssl._create_default_https_context()
+            tmpKwargs['context'] = context
+         context.load_cert_chain(self.cert_file, self.key_file)
       http_client.HTTPSConnection.__init__(self, *args, **tmpKwargs)
 
    ## Override connect to allow us to pass in additional ssl paramters to
@@ -1048,13 +1080,13 @@ class _HTTPSConnection(http_client.HTTPSConnection):
          if self._tunnel_host:
             self.sock = sock
             self._tunnel()
-         self.sock = ssl.wrap_socket(sock, self.key_file, self.cert_file,
-                                     **self._sslArgs)
+         self.sock = _SocketWrapper(sock, self.key_file, self.cert_file,
+                                    **self._sslArgs)
       elif hasattr(self, "timeout"):
          # Python 2.6
          sock = socket.create_connection((self.host, self.port), self.timeout)
-         self.sock = ssl.wrap_socket(sock, self.key_file, self.cert_file,
-                                     **self._sslArgs)
+         self.sock = _SocketWrapper(sock, self.key_file, self.cert_file,
+                                    **self._sslArgs)
       else:
          # Unknown python version. Do nothing
          http_client.HTTPSConnection.connect(self)
