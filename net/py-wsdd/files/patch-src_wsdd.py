--- src/wsdd.py.orig	2020-06-28 19:10:44 UTC
+++ src/wsdd.py
@@ -1621,5 +1621,5 @@
 # from sys/net/if.h
 IFF_LOOPBACK: int = 0x8
-IFF_MULTICAST: int = 0x800 if platform.system() != 'OpenBSD' else 0x8000
-
+IFF_MULTICAST: int = 0x8000 if platform.system() in ['FreeBSD', 'OpenBSD'] else 0x800
+
 # sys/netinet6/in6_var.h
