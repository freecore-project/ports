--- src/openvpn/ssl_verify_openssl.c.orig
+++ src/openvpn/ssl_verify_openssl.c
@@ -241,10 +241,18 @@
     {
         return FAILURE;
     }
-    if (ASN1_STRING_to_UTF8(&buf, asn1) < 0)
+    /* FreeCORE: backport of OpenVPN 2.7.8 cf4384eef4:
+     * a field with an embedded NUL byte is refused, not truncated. */
+    int length = ASN1_STRING_to_UTF8(&buf, asn1);
+    if (length < 0)
     {
         return FAILURE;
     }
+    if ((size_t)length != strlen((char *)buf))
+    {
+        OPENSSL_free(buf);
+        return FAILURE;
+    }
 
     strncpynt(out, (char *)buf, size);
 
@@ -372,10 +380,34 @@
 
     subject_bio = BIO_new(BIO_s_mem());
     if (subject_bio == NULL)
+    {
+        goto err;
+    }
+
+    /* FreeCORE: backport of OpenVPN 2.7.8 cf4384eef4:
+     * print the subject with control characters unescaped first, so that
+     * a NUL byte in it can be seen, and refuse such a certificate. */
+    X509_NAME_print_ex(subject_bio, X509_get_subject_name(cert),
+                       0, XN_FLAG_SEP_CPLUS_SPC | XN_FLAG_FN_SN
+                       |ASN1_STRFLGS_UTF8_CONVERT);
+
+    if (BIO_eof(subject_bio))
+    {
+        goto err;
+    }
+
+    BIO_get_mem_ptr(subject_bio, &subject_mem);
+    if (memchr(subject_mem->data, 0, subject_mem->length) != NULL)
     {
+        msg(M_WARN, "ERROR: Certificate subject contains a '\\0' byte.");
         goto err;
     }
 
+    /* Now output the subject with escaped control characters. */
+    if (BIO_reset(subject_bio) != 1)
+    {
+        goto err;
+    }
     X509_NAME_print_ex(subject_bio, X509_get_subject_name(cert),
                        0, XN_FLAG_SEP_CPLUS_SPC | XN_FLAG_FN_SN
                        |ASN1_STRFLGS_UTF8_CONVERT | ASN1_STRFLGS_ESC_CTRL);
