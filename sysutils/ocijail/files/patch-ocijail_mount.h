--- ocijail/mount.h.orig
+++ ocijail/mount.h
@@ -1,6 +1,7 @@
 #pragma once
 
 #include <filesystem>
+#include <optional>
 
 #include "nlohmann/json.hpp"
 
@@ -18,9 +19,13 @@ void mount_volumes(main_app& app,
                    bool root_read_only,
                    const nlohmann::json& mounts);
 
+// Unmount the volumes described by "mounts", in reverse order. If "mounted"
+// is given, only the first "mounted" entries are considered - used to roll
+// back a partially completed mount_volumes.
 void unmount_volumes(main_app& app,
                      runtime_state& state,
                      const std::filesystem::path& root_path,
-                     const nlohmann::json& mounts);
+                     const nlohmann::json& mounts,
+                     std::optional<std::size_t> mounted = std::nullopt);
 
 }  // namespace ocijail
