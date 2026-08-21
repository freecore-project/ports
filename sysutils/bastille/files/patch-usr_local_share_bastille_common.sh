--- usr/local/share/bastille/common.sh.orig	2026-02-19 00:00:00 UTC
+++ usr/local/share/bastille/common.sh
@@ -451,7 +451,7 @@ generate_vnet_jail_netblock() {
 	    cat <<-EOF
   vnet;
   vnet.interface = ${jail_epair};
-  exec.prestart += "epair0=\\\$(ifconfig epair create) && ifconfig \\\${epair0} up name ${host_epair} && ifconfig \\\${epair0%a}b up name ${jail_epair}";
+  exec.prestart += "trap '' HUP INT TERM; ifconfig epair create name ${host_epair} && epair_driver=\\\$(ifconfig -D ${host_epair} | sed -n 's/^[[:space:]]*drivername: //p') && test -n \"\\\${epair_driver}\" && ifconfig \"\\\${epair_driver}b\" up name ${jail_epair} && ifconfig ${host_epair} up; epair_status=\\\$?; trap - HUP INT TERM; exit \\\${epair_status}";
   exec.prestart += "ifconfig ${external_interface} addm ${host_epair}";
   exec.prestart += "ifconfig ${host_epair} ether ${macaddr}a";
   exec.prestart += "ifconfig ${jail_epair} ether ${macaddr}b";
