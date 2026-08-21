--- tests/conftest.py.orig	2026-05-09 18:18:02 UTC
+++ tests/conftest.py
@@ -8,2 +7,0 @@
-from icecream import ic
-from icecream import install as install_ic
@@ -26,5 +23,0 @@
-# Better print() for debugging - use ic()!
-install_ic()
-ic.configureOutput(includeContext=True)
-
-
