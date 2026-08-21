PORTNAME=			${SAMBA4_BASENAME}424
PORTVERSION=			${SAMBA4_VERSION}
# The vfs_noacl source changed under the same version; the
# revision makes poudriere rebuild instead of keeping the cached package
PORTREVISION=			1
PORTEPOCH=			1
CATEGORIES?=			net
# TrueNAS fork: source comes from /usr/nas_source/samba, not distfiles
MASTER_SITES=			# empty
DISTFILES=			# empty
EXTRACT_ONLY=			# empty

MAINTAINER=			samba@FreeBSD.org
COMMENT=			Free SMB/CIFS and AD/DC server and client for Unix
WWW=				https://gitlab.com/samba-freebsd/

LICENSE=			GPLv3+
LICENSE_FILE=			${WRKSRC}/COPYING

USES=				cpe

CONFLICTS_INSTALL?=		samba4*

# Patches already pre-applied in our TrueNAS samba fork
EXTRA_PATCHES=			# none — all patches in source

SAMBA4_BASENAME=		samba
SAMBA4_PORTNAME=		${SAMBA4_BASENAME}4
# TrueNAS builds from the bundled /usr/nas_source/samba checkout.  Keep the
# package version aligned with that checkout's VERSION file, not the upstream
# distfile version from the FreeBSD ports tree this port was forked from.
SAMBA4_VERSION=			4.24.5
SAMBA4_DISTNAME=		${SAMBA4_BASENAME}-${SAMBA4_VERSION:S|.p|pre|:S|.r|rc|:S|.t|tp|:S|.a|alpha|}

# TrueNAS fork: build from local source tree
WRKSRC=				/usr/nas_source/samba
PLIST?=				${PKGDIR}/pkg-plist

CPE_VENDOR=			samba
CPE_PRODUCT=			samba
# Directories
VARDIR=				${DESTDIR}/var
# TrueNAS keeps persistent Samba databases on the system dataset and volatile
# lock/cache state in tmpfs.  These must match middlewared's SMBPath contract.
SAMBA4_RUNDIR=			${VARDIR}/run/${SAMBA4_PORTNAME}
SAMBA4_LOGDIR=			${VARDIR}/log/${SAMBA4_PORTNAME}
SAMBA4_LOCKDIR=			${SAMBA4_RUNDIR}
SAMBA4_STATEDIR=		${VARDIR}/db/system/${SAMBA4_PORTNAME}
SAMBA4_CACHEDIR=		${SAMBA4_RUNDIR}
SAMBA4_BINDDNSDIR=		${SAMBA4_RUNDIR}/bind-dns
SAMBA4_PRIVATEDIR=		${SAMBA4_STATEDIR}/private
SAMBA4_PAMDIR=			${PREFIX}/lib
SAMBA4_LIBDIR=			${PREFIX}/lib/${SAMBA4_PORTNAME}
SAMBA4_INCLUDEDIR=		${PREFIX}/include/${SAMBA4_PORTNAME}
SAMBA4_CONFDIR=			${PREFIX}/etc
SAMBA4_CONFIG=			smb4.conf
SAMBA4_MODULES_CLASS=		auth bind9 gensec gpext idmap ldb nss_info \
				pdb perfcount process_model service vfs

CONFIGURE_ARGS=			--mandir="${PREFIX}/share/man" \
				--sysconfdir="${SAMBA4_CONFDIR}" \
				--includedir="${SAMBA4_INCLUDEDIR}" \
				--datadir="${DATADIR}" \
				--libdir="${SAMBA4_LIBDIR}" \
				--with-privatelibdir="${SAMBA4_LIBDIR}/private" \
				--with-pammodulesdir="${SAMBA4_PAMDIR}" \
				--with-modulesdir="${SAMBA4_MODULEDIR}" \
				--with-pkgconfigdir="${PKGCONFIGDIR}" \
				--localstatedir="${VARDIR}" \
				--with-piddir="${SAMBA4_RUNDIR}" \
				--with-sockets-dir="${SAMBA4_RUNDIR}" \
				--with-privileged-socket-dir="${SAMBA4_RUNDIR}" \
				--with-lockdir="${SAMBA4_LOCKDIR}" \
				--with-statedir="${SAMBA4_STATEDIR}" \
				--with-cachedir="${SAMBA4_CACHEDIR}" \
				--with-bind-dns-dir=${SAMBA4_BINDDNSDIR} \
				--with-privatedir="${SAMBA4_PRIVATEDIR}" \
				--with-logfilebase="${SAMBA4_LOGDIR}"
# XXX: Flags
CONFIGURE_ENV=			PTHREAD_LDFLAGS="-lpthread" \
				PYTHONHASHSEED=1
MAKE_ENV=			PYTHONHASHSEED=1

USES=				compiler:c++11-lang iconv localbase:ldflags \
				perl5 pkgconfig shebangfix waf gettext-runtime
USE_PERL5=			build
USE_LDCONFIG=			${SAMBA4_LIBDIR}
WAF_CMD=			buildtools/bin/waf
CONFIGURE_LOG=			bin/config.log

# Make sure that the right version of Python is used by the tools
# https://bugzilla.samba.org/show_bug.cgi?id=7305
SHEBANG_FILES=			source3/script* source4/scripting/bin/* selftest/*

PKGCONFIGDIR?=			${PREFIX}/libdata/pkgconfig
PKGCONFIGDIR_REL?=		${PKGCONFIGDIR:S,^${PREFIX}/,,}
PLIST_SUB=			PKGCONFIGDIR=${PKGCONFIGDIR_REL}
SUB_LIST=			PKGCONFIGDIR=${PKGCONFIGDIR_REL}
##############################################################################
OPTIONS_SUB=			yes

OPTIONS_DEFINE=			AD_DC ADS CLUSTER CUPS DOCS FAM GPGME \
				LDAP MANPAGES PROFILE PYTHON3 QUOTAS \
				SYSLOG UTMP
#OPTIONS_DEFINE+=		DEVELOPER MEMORY_DEBUG

OPTIONS_GROUP=			VFS
OPTIONS_GROUP_VFS=		FRUIT GLUSTERFS

OPTIONS_SINGLE=			GSSAPI ZEROCONF

OPTIONS_SINGLE_GSSAPI=		GSSAPI_BUILTIN GSSAPI_MIT
#GSSAPI_HEIMDAL
OPTIONS_SINGLE_ZEROCONF=	ZEROCONF_NONE AVAHI MDNSRESPONDER

# Make those default options
OPTIONS_DEFAULT=		AD_DC ADS DOCS FAM LDAP MANPAGES \
				PROFILE PYTHON3 QUOTAS SYSLOG UTMP \
				FRUIT GSSAPI_BUILTIN AVAHI
##############################################################################
ADS_DESC=			Active Directory client(implies LDAP)
AD_DC_DESC=			Active Directory Domain Controller(implies PYTHON3)
CLUSTER_DESC=			Clustering support
DEVELOPER_DESC=			With developer framework
FAM_DESC=			File Alteration Monitor
GPGME_DESC=			GpgME support
LDAP_DESC=			LDAP client
LIBZFS_DESC=			LibZFS
MEMORY_DEBUG_DESC=		Debug memory allocator
PICKY_DEVELOPER_DESC=		Treat compiler warnings as errors(implies DEVELOPER)
PROFILE_DESC=			Profiling data
QUOTAS_DESC=			Disk quota support
UTMP_DESC=			UTMP accounting

VFS_DESC=			VFS modules
FRUIT_DESC=			MacOSX and TimeMachine support
GLUSTERFS_DESC=			GlusterFS support

GSSAPI_BUILTIN_DESC=		GSSAPI support via bundled Heimdal

ZEROCONF_DESC=			Zero configuration networking
ZEROCONF_NONE_DESC=		Zeroconf support is absent
##############################################################################
# XXX: Unconditional dependencies which can't be switched off(if present in
# the system)

# Iconv(picked up unconditionaly)
LIB_DEPENDS=			libiconv.so:converters/libiconv
# unwind
LIB_DEPENDS+=			libunwind.so:devel/libunwind
# Readline(sponsored by Python)
# XXX: USES=readline pollutes CPPFLAGS, so we explicitly put dependency
LIB_DEPENDS+=			libreadline.so:devel/readline
# popt
LIB_DEPENDS+=			libpopt.so:devel/popt
# inotify
LIB_DEPENDS+=			libinotify.so:devel/libinotify
# GNUTLS
LIB_DEPENDS+=			libgnutls.so:security/gnutls
LIB_DEPENDS+=			libgcrypt.so:security/libgcrypt
# NFSv4 ACL glue
LIB_DEPENDS+=			libsunacl.so:sysutils/libsunacl
# Jansson
BUILD_DEPENDS+=			jansson>=2.10:devel/jansson
RUN_DEPENDS+=			jansson>=2.10:devel/jansson
# tasn1
BUILD_DEPENDS+=			libtasn1>=3.8:security/libtasn1
RUN_DEPENDS+=			libtasn1>=3.8:security/libtasn1
# External Samba dependencies
# Needed for IDL compiler
BUILD_DEPENDS+=			p5-Parse-Yapp>=0:devel/p5-Parse-Yapp

# Libarchive
SAMBA4_BUNDLED_LIBS=		!libarchive
BUILD_DEPENDS+=			libarchive>=3.1.2:archivers/libarchive
RUN_DEPENDS+=			libarchive>=3.1.2:archivers/libarchive

# Force poudriere dep ordering: samba's waf check_system_package('ldb') needs
# ldb28 (and its tevent dep) installed before samba's configure runs.  The
# .else BUILD_DEPENDS below are gated on SAMBA4_BUNDLED_*, which poudriere
# cannot evaluate when building the dep graph — so declare lib-path deps up
# front.
BUILD_DEPENDS+=			${LOCALBASE}/lib/libldb.so:databases/ldb28 \
				${LOCALBASE}/lib/libtevent.so:devel/tevent

### Bundled libraries
SAMBA4_BUNDLED_CMOCKA?=		no
# TrueNAS 4.24: bundle talloc + tdb — the 4.24.5 floors (talloc 2.4.4,
# tdb 1.4.15) exceed every port in this tree and nothing else in the
# appliance set consumes them; same posture as tevent/ldb below.
SAMBA4_BUNDLED_TALLOC?=		yes
# TrueNAS: bundle tevent — our fork adds tevent_kqueue for vfs_aio_fbsd
SAMBA4_BUNDLED_TEVENT?=		yes
SAMBA4_BUNDLED_TDB?=		yes
SAMBA4_BUNDLED_LDB?=		yes
# cmocka
.if defined(SAMBA4_BUNDLED_CMOCKA) && ${SAMBA4_BUNDLED_CMOCKA} == yes
SAMBA4_BUNDLED_LIBS+=		cmocka
CONFLICTS_INSTALL+=		cmocka-1.*
PLIST_SUB+=			SAMBA4_BUNDLED_CMOCKA=""
SUB_LIST+=			SAMBA4_BUNDLED_CMOCKA=""
.else
SAMBA4_BUNDLED_LIBS+=		!cmocka
BUILD_DEPENDS+=			cmocka>=1.1.3:sysutils/cmocka
TEST_DEPENDS+=			cmocka>=1.1.3:sysutils/cmocka
PLIST_SUB+=			SAMBA4_BUNDLED_CMOCKA="@comment "
SUB_LIST+=			SAMBA4_BUNDLED_CMOCKA="@comment "
.endif
# talloc
.if defined(SAMBA4_BUNDLED_TALLOC) && ${SAMBA4_BUNDLED_TALLOC} == yes
SAMBA4_BUNDLED_LIBS+=		talloc
PLIST_SUB+=			SAMBA4_BUNDLED_TALLOC=""
SUB_LIST+=			SAMBA4_BUNDLED_TALLOC=""
.else
SAMBA4_BUNDLED_LIBS+=		!talloc
BUILD_DEPENDS+=			talloc>=2.4.1:devel/talloc
RUN_DEPENDS+=			talloc>=2.4.1:devel/talloc
PLIST_SUB+=			SAMBA4_BUNDLED_TALLOC="@comment "
SUB_LIST+=			SAMBA4_BUNDLED_TALLOC="@comment "
.endif
# tevent
.if defined(SAMBA4_BUNDLED_TEVENT) && ${SAMBA4_BUNDLED_TEVENT} == yes
SAMBA4_BUNDLED_LIBS+=		tevent
CONFLICTS_INSTALL+=		tevent-* tevent1-*
PLIST_SUB+=			SAMBA4_BUNDLED_TEVENT=""
SUB_LIST+=			SAMBA4_BUNDLED_TEVENT=""
.else
SAMBA4_BUNDLED_LIBS+=		!tevent
BUILD_DEPENDS+=			tevent>=0.15.0:devel/tevent
RUN_DEPENDS+=			tevent>=0.15.0:devel/tevent
PLIST_SUB+=			SAMBA4_BUNDLED_TEVENT="@comment "
SUB_LIST+=			SAMBA4_BUNDLED_TEVENT="@comment "
.endif
# tdb
.if defined(SAMBA4_BUNDLED_TDB) && ${SAMBA4_BUNDLED_TDB} == yes
SAMBA4_BUNDLED_LIBS+=		tdb
CONFLICTS_INSTALL+=		tdb-* tdb1-*
PLIST_SUB+=			SAMBA4_BUNDLED_TDB=""
SUB_LIST+=			SAMBA4_BUNDLED_TDB=""
.else
SAMBA4_BUNDLED_LIBS+=		!tdb
BUILD_DEPENDS+=			tdb>=1.4.9:databases/tdb
RUN_DEPENDS+=			tdb>=1.4.9:databases/tdb
PLIST_SUB+=			SAMBA4_BUNDLED_TDB="@comment "
SUB_LIST+=			SAMBA4_BUNDLED_TDB="@comment "
.endif
# ldb
.if defined(SAMBA4_BUNDLED_LDB) && ${SAMBA4_BUNDLED_LDB} == yes
SAMBA4_BUNDLED_LDB=		yes
SAMBA4_BUNDLED_LIBS+=		ldb
PLIST_SUB+=			SAMBA4_BUNDLED_LDB=""
SUB_LIST+=			SAMBA4_BUNDLED_LDB=""
SAMBA4_MODULEDIR=		${SAMBA4_LIBDIR}/modules
.else
SAMBA4_BUNDLED_LIBS+=		!ldb
BUILD_DEPENDS+=			ldb28>=2.8.1:databases/ldb28
RUN_DEPENDS+=			ldb28>=2.8.1:databases/ldb28
PLIST_SUB+=			SAMBA4_BUNDLED_LDB="@comment "
SUB_LIST+=			SAMBA4_BUNDLED_LDB="@comment "
SAMBA4_MODULEDIR=		${PREFIX}/lib/shared-modules
.endif

.if (defined(SAMBA4_BUNDLED_TALLOC) && ${SAMBA4_BUNDLED_TALLOC} == yes) \
	|| (defined(SAMBA4_BUNDLED_TDB) && ${SAMBA4_BUNDLED_TDB} == yes) \
	|| (defined(SAMBA4_BUNDLED_LDB) && ${SAMBA4_BUNDLED_LDB} == yes) \
	|| (defined(SAMBA4_BUNDLED_TEVENT) && ${SAMBA4_BUNDLED_TEVENT} == yes)
SAMBA4_BUNDLED_LIBS+=		replace
.endif
# Don't use external libcom_err
SAMBA4_BUNDLED_LIBS+=		com_err
# Set the test environment variables
TEST_USES=			python
TEST_ENV=			PYTHON="${PYTHON_CMD}" \
				SHA1SUM=/sbin/sha1 \
				SHA256SUM=/sbin/sha256 \
				MD5SUM=/sbin/md5 \
				PYTHONDONTWRITEBYTECODE=1

TEST_DEPENDS=			bash:shells/bash \
				tshark:net/wireshark@nox11
# External Python modules
TEST_BUILD_DEPENDS=		${PYTHON_PKGNAMEPREFIX}iso8601>=0.1.11:devel/py-iso8601@${PY_FLAVOR}
TEST_RUN_DEPENDS=		${PYTHON_PKGNAMEPREFIX}iso8601>=0.1.11:devel/py-iso8601@${PY_FLAVOR}
##############################################################################
CONFIGURE_ARGS+=		\
				--with-pam \
				--with-iconv \
				--with-winbind \
				--with-regedit \
				--disable-rpath \
				--without-lttng \
				--without-gettext \
				--enable-pthreadpool \
				--without-fake-kaserver \
				--without-systemd \
				--with-libarchive \
				--with-acl-support \
				--with-sendfile-support \
				--disable-spotlight
#				${ICONV_CONFIGURE_BASE}
##############################################################################
FRUIT_PREVENTS=			ZEROCONF_NONE
FRUIT_PREVENTS_MSG=		MacOSX support requires Zeroconf(AVAHI or MDNSRESPONDER)
FRUIT_VARS=			SAMBA4_MODULES+=vfs_fruit

GLUSTERFS_CONFIGURE_ENABLE=	glusterfs
GLUSTERFS_LIB_DEPENDS=		libglusterfs.so:net/glusterfs
GLUSTERFS_VARS=			SAMBA4_MODULES+=vfs_glusterfs

ZEROCONF_NONE_MAKE_ENV=		ZEROCONF=none
##############################################################################
AVAHI_CONFIGURE_ENABLE=		avahi
AVAHI_LIB_DEPENDS=		libavahi-client.so:net/avahi-app
AVAHI_VARS=			SAMBA4_SERVICES+=avahi_daemon

# Samba 4.19 dropped --enable-dnssd/--disable-dnssd — dnssd is auto-detected
# from dns_sd.h. Keep LIB_DEPENDS so the mDNSResponder library is present.
MDNSRESPONDER_LIB_DEPENDS=	libdns_sd.so:net/mDNSResponder
MDNSRESPONDER_VARS=		SAMBA4_SERVICES+=mdnsd
##############################################################################
MEMORY_DEBUG_IMPLIES=		DEBUG
MEMORY_DEBUG_CONFIGURE_ENV=	ADDITIONAL_CFLAGS="-DENABLE_JEMALLOC `pkg-config --cflags jemalloc`" ADDITIONAL_LDFLAGS="`pkg-config --libs jemalloc`"
MEMORY_DEBUG_LIB_DEPENDS=	libjemalloc.so.2:devel/jemalloc
# https://bugs.freebsd.org/bugzilla/show_bug.cgi?id=194046
GDB_CMD?=			${LOCALBASE}/bin/gdb
# https://bugzilla.samba.org/show_bug.cgi?id=8969
PICKY_DEVELOPER_IMPLIES=	DEVELOPER
PICKY_DEVELOPER_CONFIGURE_ON=	--picky-developer

DEVELOPER_CONFIGURE_ON=		--enable-developer --enable-selftest --abi-check-disable
DEVELOPER_CONFIGURE_ENV=	WAF_CMD_FORMAT=string
DEVELOPER_BUILD_DEPENDS=	${SAMBA4_LMDB_DEPENDS} \
				${GDB_CMD}:devel/gdb
DEVELOPER_RUN_DEPENDS=		${SAMBA4_LMDB_DEPENDS}
DEVELOPER_TEST_DEPENDS=		${GDB_CMD}:devel/gdb
DEVELOPER_VARS_OFF=		GDB_CMD=true
##############################################################################
AD_DC_IMPLIES=			PYTHON3
AD_DC_CONFIGURE_OFF=		--without-ad-dc
AD_DC_BUILD_DEPENDS=		${SAMBA4_LMDB_DEPENDS}
AD_DC_RUN_DEPENDS=		${SAMBA4_LMDB_DEPENDS}
AD_DC_VARS=			PLIST+=${PKGDIR}/pkg-plist.ad_dc
# samba-tool requires those for *upgrade
AD_DC_BUILD_DEPENDS+=		${PYTHON_PKGNAMEPREFIX}markdown>=3.3.7:textproc/py-markdown@${PY_FLAVOR} \
				${PYTHON_PKGNAMEPREFIX}dnspython>=2.2.1:dns/py-dnspython@${PY_FLAVOR}
AD_DC_RUN_DEPENDS+=		${PYTHON_PKGNAMEPREFIX}markdown>=3.3.7:textproc/py-markdown@${PY_FLAVOR} \
				${PYTHON_PKGNAMEPREFIX}dnspython>=2.2.1:dns/py-dnspython@${PY_FLAVOR}

ADS_IMPLIES=			LDAP
ADS_CONFIGURE_WITH=		ads

CLUSTER_CONFIGURE_WITH=		cluster-support
CLUSTER_VARS=			PLIST+=${PKGDIR}/pkg-plist.cluster

CUPS_CONFIGURE_ENABLE=		cups iprint
CUPS_LIB_DEPENDS=		libcups.so:print/cups
# https://bugzilla.samba.org/show_bug.cgi?id=9545
FAM_LIB_DEPENDS=		libfam.so.0:devel/gamin
FAM_CONFIGURE_WITH=		fam

GPGME_CONFIGURE_WITH=		gpgme
GPGME_LIB_DEPENDS=		libgpgme.so:security/gpgme
GPGME_RUN_DEPENDS=		${PYTHON_PKGNAMEPREFIX}gpg>=2.0.0:security/py-gpg@${PY_FLAVOR}

# FB15: MIT Kerberos is in-base at /usr/libexec/krb5kdc
.if exists(/usr/libexec/krb5kdc)
GSSAPI_BUILTIN_USES=		bison gssapi
GSSAPI_BUILTIN_BUILD_DEPENDS=	p5-JSON>=4.0:converters/p5-JSON
GSSAPI_MIT_CONFIGURE_ON=	--with-system-mitkrb5 ${GSSAPIBASEDIR} \
				--with-system-mitkdc=/usr/libexec/krb5kdc \
				--with-experimental-mit-ad-dc
.else
GSSAPI_BUILTIN_USES=		bison
GSSAPI_BUILTIN_BUILD_DEPENDS=	p5-JSON>=4.0:converters/p5-JSON
GSSAPI_MIT_CONFIGURE_ON=	--with-system-mitkrb5 ${GSSAPIBASEDIR} \
				--with-system-mitkdc=${GSSAPIBASEDIR}/sbin/krb5kdc \
				--with-experimental-mit-ad-dc
.endif
GSSAPI_MIT_USES=		gssapi:mit

GSSAPI_HEIMDAL_CONFIGURE_ON=	--with-system-heimdalkrb5 ${GSSAPIBASEDIR}
GSSAPI_HEIMDAL_USES=		gssapi:heimdal
GSSAPI_HEIMDAL_PREVENTS=	AD_DC
GSSAPI_HEIMDAL_PREVENTS_MSG=	GSSAPI_HEIMDAL and AD_DC enable conflicting options

LDAP_CONFIGURE_WITH=		ldap
# Samba 4.19 dropped --with-openldap=PREFIX; openldap headers/libs found via USES=ldap paths
LDAP_USES=			ldap
LDAP_VARS=			SAMBA4_MODULES+=idmap_ldap

# TrueNAS: libzfs is always on (in-base OpenZFS on FreeBSD 15)
CONFIGURE_ARGS+=		--with-libzfs
# In-base OpenZFS: libs in /usr/lib.  <libzfs.h> is in the jail's
# /usr/include but pulls <sys/mnttab.h>, which the FreeCORE source layout
# deliberately keeps OUT of the global include set.  See post-extract.
CONFIGURE_ENV+=			LIBZFS_LIBS="-L/usr/lib -lzfs -lzfs_core -lnvpair"
CFLAGS+=			-DHAVE_LIBZFS=1 -I${WRKDIR}/zfs-include

MANPAGES_BUILD_DEPENDS=		${LOCALBASE}/share/xsl/docbook/manpages/docbook.xsl:textproc/docbook-xsl \
				xsltproc:textproc/libxslt
MANPAGES_CONFIGURE_ENV_OFF=	XSLTPROC="false"

PROFILE_CONFIGURE_WITH=		profiling-data

QUOTAS_CONFIGURE_WITH=		quotas

SYSLOG_CONFIGURE_WITH=		syslog

UTMP_CONFIGURE_WITH=		utmp

##############################################################################
.include <bsd.port.options.mk>
##############################################################################
.if !${PORT_OPTIONS:MADS} && ${PORT_OPTIONS:MAD_DC}
IGNORE=To disable ADS option you also need to disable AD_DC option
.endif

.if !defined(WANT_EXP_MODULES) || empty(WANT_EXP_MODULES)
WANT_EXP_MODULES=		vfs_cacheprime
.endif

.if ${WANT_EXP_MODULES:Mvfs_snapper}
# snapper needs dbus
LIB_DEPENDS+=			libdbus-1.so:devel/dbus
LIB_DEPENDS+=			libdbus-glib-1.so:devel/dbus-glib
.endif

SAMBA4_MODULES+=		krb5_async_dns_krb5_locator krb5_winbind_krb5_locator idmap_nss idmap_autorid \
				idmap_rid idmap_hash idmap_tdb idmap_tdb2 idmap_script \
				nss-info_hash
# List of extra modules taken from RHEL build
# https://bugs.freebsd.org/bugzilla/show_bug.cgi?id=197320
.if ${PORT_OPTIONS:MADS}
SAMBA4_MODULES+=		idmap_ad idmap_rfc2307 nss-info_template \
				nss-info_rfc2307 nss-info_sfu nss-info_sfu20
.endif
# vfs_freebsd was supplied by port patch 0027 in 13.3; the patch was never
# merged into the truenas/samba fork so the source file is absent in 15.0.
# Dropping the module reference (and the plist/manpage entries) until the
# module is carried into the fork.
#SAMBA4_MODULES+=		vfs_freebsd

# TrueNAS custom VFS modules (from truenas/samba fork)
SAMBA4_MODULES+=		vfs_ixnas vfs_shadow_copy_zfs vfs_zfs_core \
				vfs_zfs_fsrvp vfs_tmprotect vfs_noacl vfs_winmsa \
				vfs_zfs_space vfs_aio_fbsd
# smb_libzfs/zfsacl private libs are listed in pkg-plist under their
# 4.24 -private-samba.so names.

SAMBA4_MODULES+=		vfs_acl_tdb vfs_acl_xattr vfs_aio_fork vfs_aio_pthread \
				vfs_audit vfs_cap vfs_catia vfs_commit vfs_crossrename \
				vfs_default_quota vfs_dirsort vfs_expand_msdfs \
				vfs_extd_audit vfs_fake_perms vfs_full_audit \
				vfs_linux_xfs_sgid vfs_media_harmony vfs_offline \
				vfs_preopen vfs_readahead vfs_readonly vfs_recycle \
				vfs_shadow_copy vfs_shadow_copy2 vfs_shell_snap \
				vfs_streams_depot vfs_streams_xattr vfs_syncops \
				vfs_time_audit vfs_unityed_media vfs_virusfilter \
				vfs_widelinks vfs_worm vfs_xattr_tdb vfs_zfsacl

.if ${PORT_OPTIONS:MDEVELOPER}
SAMBA4_MODULES+=		auth_skel pdb_test gpext_security gpext_registry \
				gpext_scripts perfcount_test vfs_fake_dfq \
				vfs_skel_opaque vfs_skel_transparent \
				vfs_shadow_copy_test vfs_fake_acls \
				vfs_nfs4acl_xattr vfs_error_inject vfs_delay_inject
.endif
# Python bindings
.if ! ${PORT_OPTIONS:MPYTHON3} || defined(NO_PYTHON)
USES+=				python:build,test
CONFIGURE_ARGS+=		--disable-python
.else
USES+=				python
PLIST+=				${PKGDIR}/pkg-plist.python
# Don't cache Python modules
CONFIGURE_ARGS+=		--nopycache
MAKE_ENV+=			PYTHONDONTWRITEBYTECODE=1

.	if defined(SAMBA4_BUNDLED_TALLOC) && ${SAMBA4_BUNDLED_TALLOC} == yes
SAMBA4_BUNDLED_LIBS+=		pytalloc-util
.	else
SAMBA4_BUNDLED_LIBS+=		!pytalloc-util
.	endif

.	if defined(SAMBA4_BUNDLED_TEVENT) && ${SAMBA4_BUNDLED_TEVENT} == yes
SAMBA4_BUNDLED_LIBS+=		pytevent
.	else
SAMBA4_BUNDLED_LIBS+=		!pytevent
.	endif

.	if defined(SAMBA4_BUNDLED_TDB) && ${SAMBA4_BUNDLED_TDB} == yes
SAMBA4_BUNDLED_LIBS+=		pytdb
.	else
SAMBA4_BUNDLED_LIBS+=		!pytdb
.	endif

.	if defined(SAMBA4_BUNDLED_LDB) && ${SAMBA4_BUNDLED_LDB} == yes
SAMBA4_BUNDLED_LIBS+=		pyldb pyldb-util
.	else
SAMBA4_BUNDLED_LIBS+=		!pyldb !pyldb-util
.	endif
.endif

.if defined(WANT_EXP_MODULES) && !empty(WANT_EXP_MODULES)
SAMBA4_MODULES+=		${WANT_EXP_MODULES}
.endif

.if defined(SAMBA4_BUNDLED_LIBS) && !empty(SAMBA4_BUNDLED_LIBS)
CONFIGURE_ARGS+=		--bundled-libraries="${SAMBA4_BUNDLED_LIBS:Q:C|(\\\\ )+|,|g:S|\\||g}"
# Force bundled ldb/tevent/pyldb-util to build as private libs
# (${PREFIX}/lib/samba4/private/lib*-samba4.so). Upstream samba 4.19 waf
# defaults these to private_library=False in lib/{ldb,tevent,pyldb}/wscript;
# --private-libraries flips the wafsamba LIB_MUST_BE_PRIVATE check to True,
# matching the 4.16 behaviour pkg-plist still assumes.
# pyldb-util goes through pyembed_libname() which suffixes .cpython-<ver>
# (samba_python.py:149); LIB_MUST_BE_PRIVATE does a strict 'in' match, so the
# libname listed here must include that suffix to match the SAMBA_LIBRARY call.
CONFIGURE_ARGS+=		--private-libraries="talloc,tdb,ldb,tevent,pyldb-util.cpython-311,pytalloc-util.cpython-311"
.endif

.if defined(SAMBA4_MODULES) && !empty(SAMBA4_MODULES)
CONFIGURE_ARGS+=		--with-shared-modules="${SAMBA4_MODULES:C|-|_|:Q:C|(\\\\ )+|,|g:S|\\||g}"
.endif
# XXX: Hack for nss-info_* -> nss_info/* modules
# Add selected modules to the plist
.for module in ${SAMBA4_MODULES}
PLIST_FILES+=			${SAMBA4_MODULEDIR}/${module:C|_|/|:C|-|_|}.so
.endfor

.for module_class in ${SAMBA4_MODULES_CLASS}
PLIST_DIRS+=			${SAMBA4_MODULEDIR}/${module_class}
.endfor
PLIST_DIRS+=			${SAMBA4_MODULEDIR}

.if defined(WITH_DEBUG)
CONFIGURE_ARGS+=		--verbose --enable-debug
MAKE_ARGS+=			--verbose
DEBUG_FLAGS?=			-g -ggdb3 -O0
.endif

##############################################################################
.include <bsd.port.pre.mk>
##############################################################################
# Only for 64-bit architectures
.if ${ARCH} != armv6 && ${ARCH} != armv7 && ${ARCH} != i386 && ${ARCH} != mips && ${ARCH} != powerpc && ${ARCH} != powerpcspe
.	if defined(SAMBA4_BUNDLED_LDB) && ${SAMBA4_BUNDLED_LDB} == yes && (${PORT_OPTIONS:MAD_DC} || ${PORT_OPTIONS:MDEVELOPER})
# LMDB
SAMBA4_LMDB_DEPENDS=		lmdb>=0.9.16:databases/lmdb
PLIST_FILES+=			${SAMBA4_LIBDIR}/private/libldb-mdb-int-samba4.so \
				${SAMBA4_MODULEDIR}/ldb/mdb.so
.	endif
.endif

.if ${PORT_OPTIONS:MGSSAPI_MIT}
PLIST_FILES+=			${SAMBA4_MODULEDIR}/krb5/winbind_krb5_localauth.so
.	if ${PORT_OPTIONS:MMANPAGES}
PLIST_FILES+=			share/man/man8/winbind_krb5_localauth.8.gz
.	endif
.	if ${PORT_OPTIONS:MAD_DC}
PLIST_FILES+=			${SAMBA4_LIBDIR}/krb5/plugins/kdb/samba.so
.	endif
.endif
.if ${PORT_OPTIONS:MFRUIT}
.	if ${PORT_OPTIONS:MMANPAGES}
PLIST_FILES+=			share/man/man8/vfs_fruit.8.gz
.	endif
.endif
.if ${PORT_OPTIONS:MGLUSTERFS}
.	if ${PORT_OPTIONS:MMANPAGES}
PLIST_FILES+=			share/man/man8/vfs_glusterfs.8.gz
.	endif
.endif
# for libexecinfo: (so that __builtin_frame_address() finds the top of the stack)
CFLAGS_amd64+=			-fno-omit-frame-pointer
# No fancy color error messages
CFLAGS+=			${CFLAGS_${CHOSEN_COMPILER_TYPE}}
CFLAGS_clang=			-fno-color-diagnostics
CONFIGURE_ENV+=			NOCOLOR=yes WAF_LOG_FORMAT='%(c1)s%(zone)s%(c2)s %(message)s'
MAKE_ENV+=			NOCOLOR=yes WAF_LOG_FORMAT='%(c1)s%(zone)s%(c2)s %(message)s'
# Allow rpcgen to find proper CPP
MAKE_ENV+=			RPCGEN_CPP="${CPP}"
#.if ${readline_ARGS} == port
#CFLAGS+=			-D_FUNCTION_DEF
#.endif
# Some symbols in samba's linker version scripts are not defined, but since the
# scripts are generated dynamically, suppress errors with lld >= 17 due to these
# undefined symbols.
LDFLAGS+=			-Wl,--undefined-version

SAMBA4_SUB=			SAMBA4_LOGDIR="${SAMBA4_LOGDIR}" \
				SAMBA4_RUNDIR="${SAMBA4_RUNDIR}" \
				SAMBA4_LOCKDIR="${SAMBA4_LOCKDIR}" \
				SAMBA4_STATEDIR="${SAMBA4_STATEDIR}" \
				SAMBA4_CACHEDIR="${SAMBA4_CACHEDIR}" \
				SAMBA4_LIBDIR="${SAMBA4_LIBDIR}" \
				SAMBA4_MODULEDIR="${SAMBA4_MODULEDIR}" \
				SAMBA4_BINDDNSDIR="${SAMBA4_BINDDNSDIR}" \
				SAMBA4_PRIVATEDIR="${SAMBA4_PRIVATEDIR}" \
				SAMBA4_CONFDIR="${SAMBA4_CONFDIR}" \
				SAMBA4_CONFIG="${SAMBA4_CONFIG}" \
				SAMBA4_SERVICES="${SAMBA4_SERVICES}"

PLIST_SUB+=			${SAMBA4_SUB}
SUB_LIST+=			${SAMBA4_SUB}

USE_RC_SUBR=			samba_server
SUB_FILES=			pkg-message README.FreeBSD

PORTDOCS=			README.FreeBSD

# TrueNAS: source is local, skip distfile operations
do-fetch:
	@${ECHO_CMD} "Using local source tree at ${WRKSRC}"

makesum checksum:
	@${ECHO_CMD} "Checksum not needed for local source"

do-extract:
	@${ECHO_CMD} "Extract not needed for local source"

# Skip auto-patching — all patches pre-applied in our fork
do-patch:
	@${ECHO_CMD} "Patches not needed — pre-applied in source"

post-extract:
				@${RM} -r ${WRKSRC}/pidl/lib/Parse/Yapp
# the FreeCORE source layout keeps libspl's sys/mnttab.h and sys/vfs.h out of the jail's
# global /usr/include -- exposed there the shims break every port whose
# configure probes them.  smb_libzfs.c still needs mnttab.h via <libzfs.h>, so
# stage that ONE header into a private dir and point CFLAGS only at it.
# Adding libspl's include dir wholesale would shadow sys/param.h, sys/stat.h,
# sys/mount.h and fcntl.h across all ~4150 samba compile units.  mnttab.h
# itself includes only system headers, so it stands alone.
				@${MKDIR} ${WRKDIR}/zfs-include/sys
				@${CP} ${SRC_BASE}/sys/contrib/openzfs/lib/libspl/include/os/freebsd/sys/mnttab.h \
					${WRKDIR}/zfs-include/sys/mnttab.h

post-patch:
				@${REINPLACE_CMD} -e 's|$${PKGCONFIGDIR}|${PKGCONFIGDIR}|g' \
					${PATCH_WRKSRC}/buildtools/wafsamba/pkgconfig.py
				@${REINPLACE_CMD} -e 's|%%LOCALBASE%%|${LOCALBASE}|g' \
					${PATCH_WRKSRC}/buildtools/wafsamba/wafsamba.py
				@${REINPLACE_CMD} -e 's|%%GDB_CMD%%|${GDB_CMD}|g' \
					${PATCH_WRKSRC}/buildtools/scripts/abi_gen.sh
				@${REINPLACE_CMD} -e 's|%%SAMBA4_CONFIG%%|${SAMBA4_CONFIG}|g' \
					${PATCH_WRKSRC}/dynconfig/wscript

# Use threading (or multiprocessing) but not thread (renamed in python 3+).
pre-configure:
.if (!${PORT_OPTIONS:MPYTHON3} || defined(NO_PYTHON)) && ${PORT_OPTIONS:MAD_DC}
				@${ECHO_CMD}; \
				${ECHO_MSG} "===>  AD_DC option requires PYTHON3 to be set"; \
				${ECHO_CMD}; \
				${FALSE}
.endif

post-install-rm-junk:
				${RM} -r ${STAGEDIR}${PYTHON_SITELIBDIR}/samba/third_party
				${FIND} ${STAGEDIR}${PYTHON_SITELIBDIR} -name __pycache__ \
					-type d -print0 | ${XARGS} -0 -n 1 -t ${RM} -r
				${FIND} ${STAGEDIR} -type f -empty -delete

post-install-fix-manpages:
.if ${PORT_OPTIONS:MMANPAGES}
.for f in vfs_aio_linux.8 vfs_btrfs.8 vfs_ceph.8 vfs_gpfs.8
				${RM} ${STAGEDIR}${PREFIX}/share/man/man8/${f}
.endfor
.if defined(SAMBA4_BUNDLED_LDB) && ${SAMBA4_BUNDLED_LDB} == yes
.	for f in ldbadd.1 ldbdel.1 ldbedit.1 ldbmodify.1 ldbrename.1 ldbsearch.1
				${MV} ${STAGEDIR}${PREFIX}/share/man/man1/${f} ${STAGEDIR}${PREFIX}/share/man/man1/samba-${f}
.	endfor
.endif

.if defined(SAMBA4_BUNDLED_TALLOC) && ${SAMBA4_BUNDLED_TALLOC} == yes
.	for f in talloc.3
				${MV} ${STAGEDIR}${PREFIX}/share/man/man3/${f} ${STAGEDIR}${PREFIX}/share/man/man3/samba-${f}
.	endfor
.endif

.if defined(SAMBA4_BUNDLED_TDB) && ${SAMBA4_BUNDLED_TDB} == yes
.	for f in tdbbackup.8 tdbdump.8 tdbrestore.8 tdbtool.8
				${MV} ${STAGEDIR}${PREFIX}/share/man/man8/${f} ${STAGEDIR}${PREFIX}/share/man/man8/samba-${f}
.	endfor
.endif
.endif

# 13.3 relied on port patches (0006, patch-lib_ldb_wscript) to install nss_*
# plugins at ${PREFIX}/lib and rename ldb tool binaries to samba-ldb*.
# Those patches are still in files/ but the port no longer applies them
# (do-patch is a no-op), so do the equivalent at post-install time.
post-install-fix-bundled-ldb-bins:
.if defined(SAMBA4_BUNDLED_LDB) && ${SAMBA4_BUNDLED_LDB} == yes
.	for f in ldbadd ldbdel ldbedit ldbmodify ldbrename ldbsearch
				@if [ -f ${STAGEDIR}${PREFIX}/bin/${f} ] && [ ! -e ${STAGEDIR}${PREFIX}/bin/samba-${f} ]; then \
					${MV} ${STAGEDIR}${PREFIX}/bin/${f} ${STAGEDIR}${PREFIX}/bin/samba-${f}; \
				fi
.	endfor
.endif

# waf installs nss_winbind.so.1 / nss_wins.so.1 under --libdir (=SAMBA4_LIBDIR)
# when the SAMBA_PLUGIN() has no install_path; pkg-plist expects them at
# ${PREFIX}/lib so libc's nsswitch can find them. Move them.
post-install-fix-nss-modules:
				@for f in nss_winbind.so.1 nss_wins.so.1; do \
					if [ -f ${STAGEDIR}${SAMBA4_LIBDIR}/$$f ] && [ ! -e ${STAGEDIR}${PREFIX}/lib/$$f ]; then \
						${MV} ${STAGEDIR}${SAMBA4_LIBDIR}/$$f ${STAGEDIR}${PREFIX}/lib/$$f; \
					fi; \
				done

post-install: post-install-rm-junk post-install-fix-manpages \
		post-install-fix-bundled-ldb-bins post-install-fix-nss-modules
.if ${PORT_OPTIONS:MMANPAGES}
				${LN} -sf smb.conf.5.gz ${STAGEDIR}${PREFIX}/share/man/man5/smb4.conf.5.gz
.endif
# Run post-install script
.for dir in			${SAMBA4_LOGDIR} ${SAMBA4_RUNDIR} ${SAMBA4_STATEDIR} ${SAMBA4_MODULEDIR}
					${INSTALL} -d -m 0755 "${STAGEDIR}${dir}"
.endfor
				${INSTALL} -d -m 0750 "${STAGEDIR}${SAMBA4_BINDDNSDIR}"
				${INSTALL} -d -m 0750 "${STAGEDIR}${SAMBA4_PRIVATEDIR}"
.for module_class in			${SAMBA4_MODULES_CLASS}
					${INSTALL} -d -m 0755 "${STAGEDIR}${SAMBA4_MODULEDIR}/${module_class}"
.endfor
.if !defined(WITH_DEBUG)
				-${FIND} ${STAGEDIR}${PREFIX}/bin ${STAGEDIR}${PREFIX}/sbin ${STAGEDIR}${PREFIX}/libexec \
					-type f -print0 | ${XARGS} -0 -n 1 -t ${STRIP_CMD}
				-${FIND} ${STAGEDIR}${PREFIX}/lib -name '*.so*' \
					-type f -print0 | ${XARGS} -0 -n 1 -t ${STRIP_CMD}
.endif

post-install-FRUIT-off:
				${RM} ${STAGEDIR}${SAMBA4_MODULEDIR}/vfs/fruit.so
.if ${PORT_OPTIONS:MMANPAGES}
				${RM} ${STAGEDIR}${PREFIX}/share/man/man8/vfs_fruit.8
.endif

post-install-DOCS-on:
				${MKDIR} ${STAGEDIR}${DOCSDIR}
.for doc in			${PORTDOCS}
				${INSTALL_DATA} ${WRKDIR}/${doc} ${STAGEDIR}${DOCSDIR}
.endfor

post-install-CLUSTER-on:
				${LN} -nfs ../../../../share/ctdb/events/legacy/00.ctdb.script		${STAGEDIR}${PREFIX}/etc/ctdb/events/legacy/00.ctdb.script
				${LN} -nfs ../../../../share/ctdb/events/legacy/10.interface.script	${STAGEDIR}${PREFIX}/etc/ctdb/events/legacy/10.interface.script
				${LN} -nfs ../../../../share/ctdb/events/legacy/05.system.script	${STAGEDIR}${PREFIX}/etc/ctdb/events/legacy/05.system.script
				${LN} -nfs ../../../../share/ctdb/events/legacy/01.reclock.script	${STAGEDIR}${PREFIX}/etc/ctdb/events/legacy/01.reclock.script

.include <bsd.port.post.mk>
