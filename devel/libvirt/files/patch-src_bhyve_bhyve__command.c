--- src/bhyve/bhyve_command.c.orig	2026-01-01 00:00:00 UTC
+++ src/bhyve/bhyve_command.c
@@ -310,12 +310,19 @@
         if (disk->rotation_rate)
             virBufferAsprintf(&device, ",nmrr=%u", disk->rotation_rate);
 
+        if (disk->blockio.logical_block_size) {
+            virBufferAsprintf(&device, ",sectorsize=%d", disk->blockio.logical_block_size);
+            if (disk->blockio.physical_block_size) {
+                virBufferAsprintf(&device, "/%d", disk->blockio.physical_block_size);
+            }
+        }
         virBufferAddBuffer(&buf, &device);
     }
 
     virCommandAddArg(cmd, "-s");
-    virCommandAddArgFormat(cmd, "%d:0,ahci%s",
+    virCommandAddArgFormat(cmd, "%d:%d,ahci%s",
                            controller->info.addr.pci.slot,
+                           controller->info.addr.pci.function,
                            virBufferCurrentContent(&buf));
 
     return 0;
@@ -416,6 +423,7 @@
                            virCommand *cmd)
 {
     const char *disk_source;
+    virBuffer opt = VIR_BUFFER_INITIALIZER;
 
     if (virDomainDiskTranslateSourcePool(disk) < 0)
         return -1;
@@ -435,10 +443,18 @@
 
     disk_source = virDomainDiskGetSource(disk);
 
+    virBufferAsprintf(&opt, "%d:%d,virtio-blk,%s", disk->info.addr.pci.slot,
+                      disk->info.addr.pci.function, disk_source);
+
+    if (disk->blockio.logical_block_size) {
+        virBufferAsprintf(&opt, ",sectorsize=%d", disk->blockio.logical_block_size);
+        if (disk->blockio.physical_block_size) {
+            virBufferAsprintf(&opt, "/%d", disk->blockio.physical_block_size);
+        }
+    }
+
     virCommandAddArg(cmd, "-s");
-    virCommandAddArgFormat(cmd, "%d:0,virtio-blk,%s",
-                           disk->info.addr.pci.slot,
-                           disk_source);
+    virCommandAddArgBuffer(cmd, &opt);
 
     return 0;
 }
@@ -512,9 +528,7 @@
                             "%s", _("only single ISA controller is supported"));
              return -1;
         }
-        virCommandAddArg(cmd, "-s");
-        virCommandAddArgFormat(cmd, "%d:0,lpc",
-                                controller->info.addr.pci.slot);
+        virCommandAddArgList(cmd, "-s", "31,lpc", NULL);
         break;
     case VIR_DOMAIN_CONTROLLER_TYPE_NVME:
         if (bhyveBuildNVMeControllerArgStr(def, controller, driver, cmd) < 0)
@@ -905,7 +919,6 @@
      * since it forces the guest to exit when it spins on a lock acquisition.
      */
     virCommandAddArg(cmd, "-H"); /* vmexit from guest on hlt */
-    virCommandAddArg(cmd, "-P"); /* vmexit from guest on pause */
 
     virCommandAddArgList(cmd, "-s", "0:0,hostbridge", NULL);
 
