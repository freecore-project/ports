--- nasl/nasl_frame_forgery.c	2022-04-12 18:39:11.965973000 -0500
+++ nasl/nasl_frame_forgery.c	2022-04-12 22:42:28.026027000 -0500
@@ -19,12 +19,22 @@
 
 #include <errno.h>
 #include <gvm/base/networking.h>
+#if defined(linux)
 #include <linux/if_packet.h>
+#include <netinet/ether.h>
+#endif
 #include <net/ethernet.h>
 #include <net/if.h>
 #include <net/if_arp.h>
-#include <netinet/ether.h>
+#include <net/if_dl.h>
+#include <net/if_types.h>
+#if defined(__FreeBSD__)
+#include <net/bpf.h>
+#include <fcntl.h>
+#endif
 #include <netinet/if_ether.h>
+#include <netinet/in.h>
+#include <arpa/inet.h>
 #include <stdint.h>
 #include <stdio.h>
 #include <stdlib.h>
@@ -37,6 +47,13 @@
  */
 #define G_LOG_DOMAIN "lib  misc"
 
+#if defined(__FreeBSD__)
+#define ETH_ALEN ETHER_ADDR_LEN
+#define ETH_P_ARP ETHERTYPE_ARP
+#define ETH_P_ALL 0x0003
+#define ETH_HLEN ETHER_HDR_LEN
+#endif
+
 struct pseudo_eth_arp
 {
   struct arphdr arp_header;
@@ -49,7 +66,11 @@
 
 struct pseudo_frame
 {
+#if defined(__FreeBSD__)
+  struct ether_header framehdr;
+#else
   struct ethhdr framehdr;
+#endif
   u_char *payload;
 } __attribute__ ((packed));
 
@@ -81,6 +102,8 @@
  * @param[in] ifindex The interface index to be use for capturing.
  * @param[in] ether_dst_addr The dst MAC address.
  */
+
+#if !defined(__FreeBSD__)
 static void
 prepare_sockaddr_ll (struct sockaddr_ll *soc_addr_ll, int ifindex,
                      const u_char *ether_dst_addr)
@@ -121,6 +144,7 @@
   memcpy (msg, (u_char *) message, sizeof (struct msghdr) + payload_sz);
   g_free (message);
 }
+#endif /* !__FreeBSD__ */
 
 /** @brief Send a frame and listen to the answer
  *
@@ -142,20 +166,14 @@
             char *filter, struct in6_addr *ipaddr, u_char **answer)
 {
   int soc;
+#if !defined(__FreeBSD__)
   u_char *message;
+#endif
   int ifindex;
   int bpf = -1;
   int frame_and_payload = 0;
   int answer_sz = -1;
 
-  // Create the raw socket
-  soc = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
-  if (soc == -1)
-    {
-      g_debug ("%s: %s", __func__, strerror (errno));
-      return -1;
-    }
-
   // We will need the eth index. We get it depending on the target's IP..
   if (get_iface_index (ipaddr, &ifindex) < 0)
     {
@@ -163,13 +181,47 @@
       return -1;
     }
 
-  // Prepare sockaddr_ll. This is necessary for further captures
-  u_char dst_haddr[ETHER_ADDR_LEN];
-  memcpy (&dst_haddr, (struct pseudo_frame *) frame, ETHER_ADDR_LEN);
+  // Create the raw socket
+#if defined(__FreeBSD__)
+  /* Use BPF for raw Ethernet frame transmission on FreeBSD */
+  {
+    char bpf_dev[32];
+    int i;
+    struct ifreq bpf_ifr;
+    int hdr_complete = 1;
 
-  struct sockaddr_ll soc_addr;
-  memset (&soc_addr, '\0', sizeof (struct sockaddr_ll));
-  prepare_sockaddr_ll (&soc_addr, ifindex, dst_haddr);
+    soc = -1;
+    for (i = 0; i < 255; i++)
+      {
+        snprintf (bpf_dev, sizeof (bpf_dev), "/dev/bpf%d", i);
+        soc = open (bpf_dev, O_RDWR);
+        if (soc >= 0)
+          break;
+      }
+    if (soc < 0)
+      {
+        g_debug ("%s: Could not open BPF device: %s", __func__,
+                 strerror (errno));
+        return -1;
+      }
+    memset (&bpf_ifr, 0, sizeof (bpf_ifr));
+    if_indextoname (ifindex, bpf_ifr.ifr_name);
+    if (ioctl (soc, BIOCSETIF, &bpf_ifr) < 0)
+      {
+        g_debug ("%s: BIOCSETIF failed: %s", __func__, strerror (errno));
+        close (soc);
+        return -1;
+      }
+    ioctl (soc, BIOCSHDRCMPLT, &hdr_complete);
+  }
+#else
+  soc = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
+#endif
+  if (soc == -1)
+    {
+      g_debug ("%s: %s", __func__, strerror (errno));
+      return -1;
+    }
 
   /* Init capture */
   if (use_pcap != 0 && bpf < 0)
@@ -179,7 +231,7 @@
           struct in_addr sin, this_host;
           memset (&sin, '\0', sizeof (struct in_addr));
           memset (&this_host, '\0', sizeof (struct in_addr));
-          sin.s_addr = ipaddr->s6_addr32[3];
+          memcpy(&sin.s_addr, &ipaddr->s6_addr[12], sizeof(sin.s_addr));
           bpf = init_capture_device (sin, this_host, filter);
         }
       else
@@ -191,11 +243,25 @@
     }
 
   // Prepare the message and send it
-  message = g_malloc0 (sizeof (struct msghdr) + frame_sz);
-  prepare_message (message, &soc_addr, (u_char *) frame, frame_sz);
+#if defined(__FreeBSD__)
+  /* BPF write sends the complete Ethernet frame directly */
+  int b = write (soc, frame, frame_sz);
+#else
+  {
+    u_char dst_haddr[ETHER_ADDR_LEN];
+    struct sockaddr_ll soc_addr;
 
+    memcpy (&dst_haddr, (struct pseudo_frame *) frame, ETHER_ADDR_LEN);
+    memset (&soc_addr, '\0', sizeof (soc_addr));
+    prepare_sockaddr_ll (&soc_addr, ifindex, dst_haddr);
+
+    message = g_malloc0 (sizeof (struct msghdr) + frame_sz);
+    prepare_message (message, &soc_addr, (u_char *) frame, frame_sz);
+  }
+
   int b = sendmsg (soc, (struct msghdr *) message, 0);
   g_free (message);
+#endif
   if (b == -1)
     {
       g_message ("%s: Error sending message: %s", __func__, strerror (errno));
@@ -235,10 +301,15 @@
 
   *frame = (struct pseudo_frame *) g_malloc0 (sizeof (struct pseudo_frame)
                                               + payload_sz);
-
+#if defined(__FreeBSD__)
+  memcpy ((*frame)->framehdr.ether_dhost, ether_dst_addr, ETHER_ADDR_LEN);
+  memcpy ((*frame)->framehdr.ether_shost, ether_src_addr, ETHER_ADDR_LEN);
+  (*frame)->framehdr.ether_type = htons (ether_proto);
+#else
   memcpy ((*frame)->framehdr.h_dest, ether_dst_addr, ETHER_ADDR_LEN);
   memcpy ((*frame)->framehdr.h_source, ether_src_addr, ETHER_ADDR_LEN);
   (*frame)->framehdr.h_proto = htons (ether_proto);
+#endif
   (*frame)->payload = payload;
 
   frame_sz = ETH_HLEN + payload_sz;
@@ -402,7 +473,34 @@
   strncpy (ifr.ifr_name, if_name, sizeof (ifr.ifr_name) - 1);
   g_free (if_name);
   ifr.ifr_name[sizeof (ifr.ifr_name) - 1] = '\0';
-
+#if defined(__FreeBSD__)
+  {
+    struct ifaddrs *ifaddr, *ifa;
+    int found = 0;
+    if (getifaddrs (&ifaddr) == -1)
+      {
+        g_debug ("%s: getifaddrs() failed", __func__);
+        return -1;
+      }
+    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
+      {
+        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_LINK
+            && strcmp (ifa->ifa_name, ifr.ifr_name) == 0)
+          {
+            struct sockaddr_dl *sdl = (struct sockaddr_dl *) ifa->ifa_addr;
+            memcpy (mac, LLADDR (sdl), ETHER_ADDR_LEN);
+            found = 1;
+            break;
+          }
+      }
+    freeifaddrs (ifaddr);
+    if (!found)
+      {
+        g_debug ("%s: MAC not found for %s", __func__, ifr.ifr_name);
+        return -1;
+      }
+  }
+#else
   sock = socket (PF_INET, SOCK_STREAM, 0);
   if (-1 == sock)
     {
@@ -418,6 +516,7 @@
 
   memcpy (mac, (u_char *) ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN);
   close (sock);
+#endif
 
   return 0;
 }
@@ -490,7 +589,7 @@
     return retc;
 
   memset (&dst_inaddr, '\0', sizeof (struct in_addr));
-  dst_inaddr.s_addr = dst->s6_addr32[3];
+  memcpy(&dst_inaddr.s_addr, &dst->s6_addr[12], sizeof(dst_inaddr.s_addr));
   routethrough (&dst_inaddr, &src_inaddr);
   ipv4_as_ipv6 (&src_inaddr, &src);
 
