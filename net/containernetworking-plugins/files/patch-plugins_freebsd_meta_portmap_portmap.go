--- plugins/freebsd/meta/portmap/portmap.go.orig	2025-06-18 15:47:32 UTC
+++ plugins/freebsd/meta/portmap/portmap.go
@@ -121,6 +121,15 @@
 				fmt.Sprintf(
 					"nat on %s %s proto %s from (lo0) to %s port %d -> (%s)",
 					config.BrName, containerAF, pmap.Protocol, containerIP, pmap.ContainerPort, config.BrName))
+			// Hairpin: a connection the container opens to its own
+			// published port is redirected back to it with its own
+			// address as the source, so the reply never leaves the
+			// container and the connection times out. Translate the
+			// source to the bridge address, as for (lo0) above.
+			res = append(res,
+				fmt.Sprintf(
+					"nat on %s %s proto %s from %s to %s port %d -> (%s)",
+					config.BrName, containerAF, pmap.Protocol, containerIP, containerIP, pmap.ContainerPort, config.BrName))
 		}
 	}
 	return res, nil
