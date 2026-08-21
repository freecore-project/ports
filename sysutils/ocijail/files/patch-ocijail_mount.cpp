--- ocijail/mount.cpp.orig
+++ ocijail/mount.cpp
@@ -5,6 +5,7 @@
 #include <sys/uio.h>
 #include <sys/wait.h>
 #include <iostream>
+#include <optional>
 
 #include "ocijail/main.h"
 #include "ocijail/mount.h"
@@ -477,8 +478,23 @@ static void unmount_volume(main_app& app,
         }
     } else {
         if (::unmount(destination.c_str(), MNT_FORCE) < 0) {
-            // unmount will return EINVAL if the mount doesn't exist
+            // unmount returns EINVAL if the kernel cannot find a mount
+            // registered on this path. Normally that just means the mount is
+            // already gone, but it also happens when the mount is real and
+            // simply unreachable by path - the kernel stores a mount point
+            // path that does not match the one we can name. A nullfs file
+            // mount created inside a jail is the known case: vfs_domount()
+            // records it relative to the jail root rather than as a global
+            // path, so no path in any namespace resolves to it and it can
+            // only be removed with "umount -f <fsid>" from the host.
+            //
+            // We cannot tell the two apart here, so carry on with the
+            // teardown but leave a trace, because in the second case we have
+            // just leaked a mount.
             if (errno == EINVAL) {
+                app.log() << "unmounting " << destination
+                          << ": not a known mount point; if a mount is still "
+                             "present here it has been leaked";
                 return;
             }
             throw std::system_error{
@@ -495,6 +511,13 @@ void mount_volumes(main_app& app,
                    bool prepare_only,
                    const json& mounts) {
     bool file_mount_supported = true;
+    std::size_t mounted = 0;
+
+    // Publish this before mounting anything. unmount_volumes() reads it back
+    // out of the state and the rollback below runs before we would otherwise
+    // have stored it, which used to make the rollback throw
+    // json::type_error and silently do nothing at all.
+    state["file_mount_supported"] = file_mount_supported;
 
     try {
         for (auto& mount : mounts) {
@@ -504,30 +527,47 @@ void mount_volumes(main_app& app,
                                                 root_path,
                                                 prepare_only,
                                                 mount);
+            state["file_mount_supported"] = file_mount_supported;
+            mounted++;
         }
     } catch (const std::exception& e) {
-        // Attempt to clean up in case we mounted something
+        // Attempt to clean up whatever we managed to mount
         try {
-            unmount_volumes(app, state, root_path, mounts);
+            unmount_volumes(app, state, root_path, mounts, mounted);
         } catch (...) {
         }
         throw;
     }
-    state["file_mount_supported"] = file_mount_supported;
 }
 
 void unmount_volumes(main_app& app,
                      runtime_state& state,
                      const fs::path& root_path,
-                     const json& mounts) {
-    bool file_mount_supported = state["file_mount_supported"];
+                     const json& mounts,
+                     std::optional<std::size_t> mounted) {
+    bool file_mount_supported = true;
+    if (state.contains("file_mount_supported")) {
+        file_mount_supported = state["file_mount_supported"];
+    }
+
+    // Only the first "mounted" entries were actually mounted. The default is
+    // the whole list, which is what delete wants.
+    auto count = mounted.value_or(mounts.size());
 
     // Remember the first exception (if any) but try to unmount
-    // everything
+    // everything.
+    //
+    // Unmount in reverse order. Mounts can be nested - podman mounts fdescfs
+    // on /dev/fd inside the devfs it mounted on /dev - and a mount must come
+    // down before the one it sits inside. Unmounting the outer one first does
+    // not fail: MNT_FORCE tears it down anyway and strands the inner mount on
+    // a covered vnode whose filesystem is gone, after which it has no path in
+    // any namespace and only "umount -f <fsid>" from the host can remove it.
     std::exception_ptr eptr{nullptr};
-    for (auto& mount : mounts) {
+    for (std::size_t i = count; i > 0; i--) {
         try {
-            unmount_volume(app, file_mount_supported, state, root_path, mount);
+            unmount_volume(
+                app, file_mount_supported, state, root_path, mounts[i - 1]);
         } catch (const std::exception&) {
             if (!eptr) {
                 eptr = std::current_exception();
@@ -548,8 +588,13 @@ void unmount_volumes(main_app& app,
         }
         std::sort(paths.begin(), paths.end(), std::greater<std::string>());
         for (auto& dir : paths) {
-            if (fs::exists(dir)) {
-                fs::remove(dir);
+            // Removing a mount point that is still mounted fails with EBUSY.
+            // Report it and carry on: the remaining entries still need to be
+            // removed, and failing the whole teardown over one leftover mount
+            // point loses the container's exit status in podman.
+            std::error_code ec;
+            if (fs::exists(dir, ec) && !fs::remove(dir, ec) && ec) {
+                app.log() << "removing " << dir << ": " << ec.message();
             }
         }
     } catch (...) {
