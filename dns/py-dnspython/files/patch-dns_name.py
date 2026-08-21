--- dns/name.py.orig
+++ dns/name.py
@@ -91,6 +91,10 @@
     """A DNS compression pointer points forward instead of backward."""
 
 
+class PointerChainTooLong(dns.exception.FormError):
+    """A DNS compression pointer chain was too long."""
+
+
 class BadLabelType(dns.exception.FormError):
     """The label type in DNS name wire format is unknown."""
 
@@ -1072,6 +1076,9 @@
 # we need 'dns.wire.Parser' quoted as dns.name and dns.wire depend on each other.
 
 
+MAX_COMPRESSION_POINTER_CHAIN = 16
+
+
 def from_wire_parser(parser: "dns.wire.Parser") -> Name:
     """Convert possibly compressed wire format into a Name.
 
@@ -1087,6 +1094,7 @@
 
     labels = []
     biggest_pointer = parser.current
+    hops = 0
     with parser.restore_furthest():
         count = parser.get_uint8()
         while count != 0:
@@ -1097,6 +1105,9 @@
                 if current >= biggest_pointer:
                     raise BadPointer
                 biggest_pointer = current
+                hops += 1
+                if hops > MAX_COMPRESSION_POINTER_CHAIN:
+                    raise PointerChainTooLong
                 parser.seek(current)
             else:
                 raise BadLabelType
