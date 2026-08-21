Python 3.12 removed HTTPSConnection's key_file and cert_file arguments.  Load
the SSO client certificate into an SSLContext before constructing it instead.

--- pyVim/sso.py.orig	2021-11-19 08:31:12 UTC
+++ pyVim/sso.py
@@ -125,6 +125,16 @@ class SSOHTTPSConnection(six.moves.http_client.HTTPSCo
             self.server_cert = _extract_certificate(server_cert)
         else:
             self.server_cert = None
+        key_file = kwargs.pop('key_file', None)
+        cert_file = kwargs.pop('cert_file', None)
+        if key_file and not cert_file:
+            raise ValueError("certfile must be specified")
+        if cert_file:
+            context = kwargs.get('context')
+            if context is None:
+                context = ssl._create_default_https_context()
+                kwargs['context'] = context
+            context.load_cert_chain(cert_file, key_file)
         six.moves.http_client.HTTPSConnection.__init__(self, *args, **kwargs)
 
     def _check_cert(self, peerCert):
