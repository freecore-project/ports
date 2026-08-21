--- usr/local/share/bastille/create.sh.orig	2026-02-19 00:00:00 UTC
+++ usr/local/share/bastille/create.sh
@@ -284,6 +284,9 @@ generate_vnet_jail_conf() {
 
     if [ "$(sysctl -n security.jail.jailed)" -eq 1 ]; then
         devfs_ruleset_value=0
+    elif [ "${VNET_INTERFACE_TYPE}" = "bridge" ] && \
+         ! echo "${IP4_ADDR}" | grep -Eq '^(0[.]0[.]0[.]0|DHCP|SYNCDHCP)$'; then
+        devfs_ruleset_value=4
     else
         devfs_ruleset_value=13
     fi
@@ -581,7 +584,8 @@ create_jail() {
         # VNET specific
         if [ "${VNET_JAIL}" -eq 1 ]; then
             # VNET requires jib or jng script
-            if [ "${bastille_network_vnet_type}" = "if_bridge" ]; then
+            if [ "${bastille_network_vnet_type}" = "if_bridge" ] && \
+               [ "${VNET_INTERFACE_TYPE}" = "standard" ]; then
                 if [ ! "$(command -v jib)" ]; then
                     if [ -f /usr/share/examples/jails/jib ] && [ ! -f /usr/local/bin/jib ]; then
                         install -m 0544 /usr/share/examples/jails/jib /usr/local/bin/jib
