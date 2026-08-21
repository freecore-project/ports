--- src/memory.c.orig	2020-09-03 15:12:19 UTC
+++ src/memory.c
@@ -282,13 +282,13 @@ static int memory_read_internal(value_list_t *vl) {
    * vm.stats.vm.v_wire_count: 37526
    * vm.stats.vm.v_active_count: 55239
    * vm.stats.vm.v_inactive_count: 113730
-   * vm.stats.vm.v_cache_count: 10809
+   * vm.stats.vm.v_laundry_count: 10809
    */
   const char *sysctl_keys[8] = {
       "vm.stats.vm.v_page_size",    "vm.stats.vm.v_page_count",
       "vm.stats.vm.v_free_count",   "vm.stats.vm.v_wire_count",
       "vm.stats.vm.v_active_count", "vm.stats.vm.v_inactive_count",
-      "vm.stats.vm.v_cache_count",  NULL};
+      "vm.stats.vm.v_laundry_count", NULL};
   double sysctl_vals[8];
 
   for (int i = 0; sysctl_keys[i] != NULL; i++) {
@@ -309,7 +309,7 @@ static int memory_read_internal(value_list_t *vl) {
 
   MEMORY_SUBMIT("free", (gauge_t)sysctl_vals[2], "wired",
                 (gauge_t)sysctl_vals[3], "active", (gauge_t)sysctl_vals[4],
-                "inactive", (gauge_t)sysctl_vals[5], "cache",
+                "inactive", (gauge_t)sysctl_vals[5], "laundry",
                 (gauge_t)sysctl_vals[6]);
 
 #endif /* HAVE_SYSCTL && KERNEL_NETBSD */
