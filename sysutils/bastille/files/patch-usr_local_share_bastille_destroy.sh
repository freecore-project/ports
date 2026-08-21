--- usr/local/share/bastille/destroy.sh.orig	2026-02-19 00:00:00 UTC
+++ usr/local/share/bastille/destroy.sh
@@ -131,7 +131,7 @@ destroy_jail() {
         fi
 
         # Clear any active rdr rules
-        if [ ! -z "$(pfctl -a "rdr/${jail}" -Psn 2>/dev/null)" ]; then
+        if [ -f "${bastille_pf_conf}" ] && [ -n "$(pfctl -a "rdr/${jail}" -Psn 2>/dev/null)" ]; then
             info 2 "Clearing RDR rules..."
             pfctl -a "rdr/${jail}" -Fn
         fi
