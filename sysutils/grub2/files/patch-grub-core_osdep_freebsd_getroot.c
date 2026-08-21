--- grub-core/osdep/freebsd/getroot.c.orig	2026-10-21 02:45:00 UTC
+++ grub-core/osdep/freebsd/getroot.c
@@ -339,7 +339,10 @@
 	    free (name_tmp);
 	    LIST_FOREACH (config, &provider->lg_config, lg_config)
 	      if (strcasecmp (config->lg_name, "offset") == 0)
-		off += strtoull (config->lg_val, 0, 10) / provider->lg_sectorsize;
+		/* GRUB counts partition starts in 512-byte sectors whatever the disk's own
+		   sector size; dividing by lg_sectorsize gave 4 KiB units on a 4Kn disk
+		   (4096-byte logical sectors).  */
+		off += strtoull (config->lg_val, 0, 10) >> GRUB_DISK_SECTOR_BITS;
 	    if (off_out)
 	      *off_out = off;
 	    return;
