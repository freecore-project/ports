--- /dev/null
+++ pyasn1/compat/octets.py
@@ -0,0 +1,19 @@
+#
+# This file is part of pyasn1 software.
+#
+# Copyright (c) 2005-2020, Ilya Etingof <etingof@gmail.com>
+# License: https://pyasn1.readthedocs.io/en/latest/license.html
+#
+# pyasn1 0.6.1 removed these Python compatibility helpers.  Keep their
+# Python 3 behavior for legacy consumers such as pysnmp 4.4.9.
+
+ints2octs = bytes
+int2oct = lambda x: ints2octs((x,))
+null = ints2octs()
+oct2int = lambda x: x
+octs2ints = lambda x: x
+str2octs = lambda x: x.encode('iso-8859-1')
+octs2str = lambda x: x.decode('iso-8859-1')
+isOctetsType = lambda s: isinstance(s, bytes)
+isStringType = lambda s: isinstance(s, str)
+ensureString = bytes
