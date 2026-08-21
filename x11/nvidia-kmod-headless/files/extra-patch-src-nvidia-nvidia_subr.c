--- src/nvidia/nvidia_subr.c.orig
+++ src/nvidia/nvidia_subr.c
@@ -22,7 +22,6 @@
 #include <sys/fbio.h>
 #include <sys/linker.h>
 #include <sys/firmware.h>
-#include <dev/syscons/syscons.h>
 #include <machine/metadata.h>
 
 uma_zone_t nvidia_stack_t_zone;
@@ -2412,34 +2411,15 @@
             *pFbPitch = efifb->fb_stride * (bpp / NBBY);
             *pFbSize = (NvU64)(*pFbHeight) * (NvU64)(*pFbPitch);
             return;
-        }
-    }
-    {
-        const sc_softc_t *sc = sc_get_softc(0, SC_KERNEL_CONSOLE);
-
-        if (sc)
-        {
-            const video_adapter_t *adp = sc->adp;
-
-            if (adp)
-            {
-                const struct video_info *vi = &adp->va_info;
-
-                /* Make sure base address is mapped to GPU BAR */
-                if (vi && (vi->vi_flags & V_INFO_LINEAR) &&
-                    NV_IS_CONSOLE_MAPPED(nv, vi->vi_buffer))
-                {
-                    *pPhysicalAddress = vi->vi_buffer;
-                    *pFbWidth = vi->vi_width;
-                    *pFbHeight = vi->vi_height;
-                    *pFbDepth = vi->vi_depth;
-                    *pFbPitch = adp->va_line_width;
-                    *pFbSize = (NvU64)(*pFbHeight) * (NvU64)(*pFbPitch);
-                    return;
-                }
-            }
         }
     }
+    /*
+     * FreeCORE: the syscons fallback is removed for the headless build.
+     * This appliance kernel is vt(4)-only and has no syscons, so sc_get_softc()
+     * is undefined and the module fails to link. The EFI framebuffer path above
+     * is the one that applies on UEFI systems; with no console framebuffer found
+     * the function correctly falls through to reporting none.
+     */
 
     *pPhysicalAddress = 0;
     *pFbWidth = *pFbHeight = *pFbDepth = *pFbPitch = *pFbSize = 0;
