--- ocijail/create.cpp.orig
+++ ocijail/create.cpp
@@ -4,8 +4,11 @@
 #include <sys/mount.h>
 #include <sys/socket.h>
 #include <sys/stat.h>
+#include <sys/sysctl.h>
 #include <sys/types.h>
+#include <sys/wait.h>
 #include <unistd.h>
+#include <cstdlib>
 #include <iostream>
 #include <sstream>
 #include <unordered_map>
@@ -221,6 +224,7 @@ void create::run() {
     std::optional<std::string> ip4_addr;
     std::optional<std::string> ip6_addr;
     std::vector<std::string> allow_params;
+    std::vector<std::pair<std::string, std::string>> vnet_sysctls;
 
     std::unordered_map<std::string, std::optional<jail::ns>> known_ns_params = {
         {"org.freebsd.jail.vnet", std::nullopt},
@@ -286,6 +290,26 @@ void create::run() {
             "allow.sysvipc",       "allow.unprivileged_parent_tampering",
             "allow.unprivileged_proc_debug",
         };
+        // FreeCORE: org.freebsd.jail.vnet.sysctl.<name>=<value> sets a net.* sysctl inside
+        // the container's own vnet before its process runs -- e.g. Docker's default that
+        // lets an unprivileged service bind ports below 1024
+        // (net.inet.ip.portrange.reservedhigh=0).
+        const std::string vnet_sysctl_prefix = "org.freebsd.jail.vnet.sysctl.";
+        for (auto& [key, value] : config_annotations.items()) {
+            if (key.starts_with(vnet_sysctl_prefix) && value.is_string()) {
+                std::string name = key.substr(vnet_sysctl_prefix.size());
+                bool valid = name.starts_with("net.") &&
+                             name.find_first_not_of(
+                                 "abcdefghijklmnopqrstuvwxyz0123456789_.") ==
+                                 std::string::npos;
+                if (valid) {
+                    vnet_sysctls.emplace_back(name, std::string(value));
+                } else {
+                    app_.log() << "warning: vnet sysctl annotation '" << key
+                               << "' is not a net.* name, ignoring";
+                }
+            }
+        }
         const std::string allow_prefix = "org.freebsd.jail.allow.";
         for (auto& [key, value] : config_annotations.items()) {
             if (key.starts_with(allow_prefix) && value.is_string()) {
@@ -465,6 +489,62 @@ void create::run() {
 
     auto j = jail::create(jconf);
 
+    // FreeCORE: apply the vnet sysctl annotations inside the new jail, which owns
+    // its vnet, before anything runs there. A short-lived child attaches to the
+    // jail and calls sysctlbyname(3) directly, so no FreeBSD binary is needed in
+    // the container's root (a Linux image has none). A failure is logged, not
+    // fatal: the container still starts as it would have without the annotation.
+    if (!vnet_sysctls.empty()) {
+        if (vnet != jail::NEW) {
+            app_.log() << "warning: vnet sysctl annotations need a container with "
+                          "its own vnet, ignoring";
+        } else {
+            auto sysctl_pid = ::fork();
+            if (sysctl_pid < 0) {
+                throw std::system_error{
+                    errno, std::system_category(), "fork for vnet sysctls"};
+            }
+            if (sysctl_pid == 0) {
+                int failed = 0;
+                try {
+                    j.attach();
+                } catch (...) {
+                    ::_exit(2);
+                }
+                for (const auto& [name, value] : vnet_sysctls) {
+                    size_t len = 0;
+                    if (::sysctlbyname(name.c_str(), nullptr, &len, nullptr, 0) < 0) {
+                        failed = 1;
+                        continue;
+                    }
+                    int rc;
+                    if (len == sizeof(int)) {
+                        char* end = nullptr;
+                        long v = std::strtol(value.c_str(), &end, 10);
+                        if (end == value.c_str() || *end != '\0') {
+                            failed = 1;
+                            continue;
+                        }
+                        int iv = int(v);
+                        rc = ::sysctlbyname(name.c_str(), nullptr, nullptr, &iv, sizeof(iv));
+                    } else {
+                        rc = ::sysctlbyname(name.c_str(), nullptr, nullptr,
+                                            value.c_str(), value.size() + 1);
+                    }
+                    if (rc < 0) {
+                        failed = 1;
+                    }
+                }
+                ::_exit(failed);
+            }
+            int sysctl_status = 0;
+            ::waitpid(sysctl_pid, &sysctl_status, 0);
+            if (!WIFEXITED(sysctl_status) || WEXITSTATUS(sysctl_status) != 0) {
+                app_.log() << "warning: not every vnet sysctl annotation could be set";
+            }
+        }
+    }
+
     // We record the container state including the bundle config. We
     // need to create the start fifo before forking - this will be
     // used to pause the container until start is called.
