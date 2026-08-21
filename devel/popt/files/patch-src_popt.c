Upstream security fix: off-by-one error in poptStuffArgs().
Backport of upstream rpm-software-management/popt 14c42b415b (2026-08-18),
source hunk only (the new tstuff test needs the upstream CMake harness).

--- src/popt.c.orig
+++ src/popt.c
@@ -1668,7 +1668,7 @@
     int argc;
     int rc;
 
-    if ((con->os - con->optionStack) == POPT_OPTION_DEPTH)
+    if ((con->os - con->optionStack + 1) == POPT_OPTION_DEPTH)
 	return POPT_ERROR_OPTSTOODEEP;
 
     for (argc = 0; argv[argc]; argc++)
