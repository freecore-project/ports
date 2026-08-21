--- usr/local/share/bastille/start.sh.orig	2026-02-19 00:00:00 UTC
+++ usr/local/share/bastille/start.sh
@@ -148,7 +148,7 @@ for jail in ${JAILS}; do
                         warn 1 "[WARNING]: IP address (${ip}) already in use, continuing..."
                     fi
                     ## add ip to firewall table if it is not reachable through local interface (assumes NAT/rdr is needed)
-                    if route -n get ${ip} | grep "gateway" >/dev/null; then
+                    if [ -f "${bastille_pf_conf}" ] && route -n get "${ip}" | grep "gateway" >/dev/null; then
                         pfctl -q -t "${bastille_network_pf_table}" -T add "${ip}"
                     fi
                 else
@@ -171,7 +171,7 @@ for jail in ${JAILS}; do
                         warn 1 "[WARNING]: IP address (${ip}) already in use, continuing..."
                     fi
                     ## add ip to firewall table if it is not reachable through local interface (assumes NAT/rdr is needed)
-                    if route -6 -n get ${ip} | grep "gateway" >/dev/null; then
+                    if [ -f "${bastille_pf_conf}" ] && route -6 -n get "${ip}" | grep "gateway" >/dev/null; then
                         pfctl -q -t "${bastille_network_pf_table}" -T add "${ip}"
                     fi
                 else
