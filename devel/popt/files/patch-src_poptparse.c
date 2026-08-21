Upstream security fix: buffer overflow(s) in poptConfigFileToString().
Backport of upstream rpm-software-management/popt ea2092edba (2026-08-18),
source hunk only (the new test3 case needs the upstream test harness).

--- src/poptparse.c.orig
+++ src/poptparse.c
@@ -139,7 +139,6 @@
     char * p;
     char * q;
     char * x;
-    size_t t;
     size_t argvlen = 0;
     size_t maxlinelen = sizeof(line);
     size_t linelen;
@@ -186,9 +185,9 @@
 	if (*q == '\0') {
 	    /* single command line option (ie, no name=val, just name) */
 	    q[-1] = '\0';		/* kill off newline from fgets() call */
-	    argvlen += (t = (size_t)(q - p)) + (sizeof(" --")-1);
+	    argvlen += ((size_t)(q - p)) + (sizeof(" --")-1);
 	    if (argvlen >= maxargvlen) {
-		maxargvlen = (t > maxargvlen) ? t*2 : maxargvlen*2;
+		maxargvlen = argvlen * 2;
 		argstr_tmp = realloc(argstr, maxargvlen);
 		if (argstr_tmp == NULL) {
 		    free(argstr);
@@ -218,10 +217,9 @@
 	    *x = '\0';	/* null out last char if space (including fgets() NL) */
 
 	/* rest of line accept */
-	t = (size_t)(x - p);
-	argvlen += t + (sizeof("' --='")-1);
+	argvlen += ((size_t)(x - p) + sizeof("' --='")-1);
 	if (argvlen >= maxargvlen) {
-	    maxargvlen = (t > maxargvlen) ? t*2 : maxargvlen*2;
+	    maxargvlen = argvlen * 2;
 	    argstr_tmp = realloc(argstr, maxargvlen);
 	    if (argstr_tmp == NULL) {
 		free(argstr);
