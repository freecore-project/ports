--- dns/rdtypes/IN/WKS.py.orig
+++ dns/rdtypes/IN/WKS.py
@@ -44,6 +44,10 @@
         self.address = self._as_ipv4_address(address)
         self.protocol = self._as_uint8(protocol)
         self.bitmap = self._as_bytes(bitmap)
+        if len(self.bitmap) > 8192:
+            raise ValueError(
+                "service bitmap must be between 0 and 8192 bytes inclusive"
+            )
 
     def to_text(self, origin=None, relativize=True, **kw):
         bits = []
@@ -77,11 +81,12 @@
                 else:
                     protocol_text = "tcp"
                 serv = socket.getservbyname(value, protocol_text)
+            if serv < 0 or serv > 65535:
+                raise ValueError("service must be between 0 and 65535 inclusive")
             i = serv // 8
             l = len(bitmap)
             if l < i + 1:
-                for _ in range(l, i + 1):
-                    bitmap.append(0)
+                bitmap.extend(bytes(i + 1 - l))
             bitmap[i] = bitmap[i] | (0x80 >> (serv % 8))
         bitmap = dns.rdata._truncate_bitmap(bitmap)
         return cls(rdclass, rdtype, address, protocol, bitmap)
