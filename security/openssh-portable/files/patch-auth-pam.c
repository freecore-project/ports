FreeCORE local patch; not in FreeBSD ports. Keep it when
this port is synced with upstream.

Since OpenSSH 10.3, sshd refuses a password login when a PAM module changed
PAM_USER during authentication, comparing it with the name as typed.
pam_winbind always returns its canonical name, so an Active Directory user who
types another case of the name, or user@realm, is refused with the correct
password. Compare accounts instead of strings: when the names differ, accept
only if the new name resolves to the uid that was looked up for this login.

Same approach as the Fedora patch (Red Hat bug 2423900) and Ubuntu's
lp-2150273-openssh-pam-upn.patch (Launchpad bug 2150273). Upstream: OpenSSH
bug 3955. Drop it when upstream or the stock port carries an equivalent.

--- auth-pam.c.orig
+++ auth-pam.c
@@ -456,6 +456,7 @@
 check_pam_user(Authctxt *authctxt)
 {
 	const char *pam_user;
+	const struct passwd *pam_pw;
 
 	if (authctxt == NULL || authctxt->pw == NULL ||
 	    authctxt->pw->pw_name == NULL)
@@ -472,11 +473,23 @@
 
 	if (sshpam_initial_user == NULL)
 		fatal_f("internal error: sshpam_initial_user NULL");
-	if (strcmp(sshpam_initial_user, pam_user) != 0) {
+	if (strcmp(sshpam_initial_user, pam_user) == 0)
+		return PAM_SUCCESS;
+	/*
+	 * FreeCORE: a PAM module may return the name in
+	 * its canonical form (pam_winbind does, for another case of the same
+	 * name and for user@realm). Accept that when it is still the account
+	 * that was looked up for this login; a change to any other account
+	 * is refused as before.
+	 */
+	pam_pw = getpwnam(pam_user);
+	if (pam_pw == NULL || pam_pw->pw_uid != authctxt->pw->pw_uid) {
 		error_f("PAM user \"%s\" does not match previous \"%s\"",
 		      pam_user, sshpam_initial_user);
 		return PAM_USER_UNKNOWN;
 	}
+	debug_f("PAM user \"%s\" is the same account as \"%s\"",
+	    pam_user, sshpam_initial_user);
 	return PAM_SUCCESS;
 }
 
