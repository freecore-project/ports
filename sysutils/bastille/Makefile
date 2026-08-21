PORTNAME=	bastille
DISTVERSION=	1.4.0.260219
PORTREVISION=	5
CATEGORIES=	sysutils
MAINTAINER=	jdhurtado@orbiware.com
# co-maintainer: snarfingcode666@gmail.com
COMMENT=	Jail automation framework
WWW=		https://bastillebsd.org

LICENSE=	BSD3CLAUSE
LICENSE_FILE=	${WRKSRC}/LICENSE

USE_GITHUB=	yes
GH_ACCOUNT=	BastilleBSD

NO_ARCH=	yes
NO_BUILD=	yes

post-patch:
	@${REINPLACE_CMD} \
		-e 's|/usr/local/etc/bastille|${ETCDIR}|g' \
		-e 's|/usr/local|${PREFIX}|g' \
		${WRKSRC}/usr/local/etc/rc.d/bastille

do-install:
	${MKDIR} ${STAGEDIR}${ETCDIR}
	${MKDIR} ${STAGEDIR}${DATADIR}
	${INSTALL_SCRIPT} ${WRKSRC}/usr/local/bin/bastille ${STAGEDIR}${PREFIX}/bin
	${INSTALL_DATA} ${WRKSRC}/usr/local/etc/bastille/bastille.conf.sample \
		${STAGEDIR}${ETCDIR}/${PORTNAME}.conf.sample
	${INSTALL_SCRIPT} ${WRKSRC}/usr/local/etc/rc.d/bastille \
		${STAGEDIR}${PREFIX}/etc/rc.d
	(cd ${WRKSRC}/usr/local/share/bastille && \
		${COPYTREE_SHARE} . ${STAGEDIR}${DATADIR})
	${INSTALL_DATA} ${FILESDIR}/freecore-lifecycle-v3 \
		${STAGEDIR}${DATADIR}/freecore-lifecycle-v3
	(cd ${WRKSRC}/usr/local/share/man && \
		${COPYTREE_SHARE} . ${STAGEDIR}${PREFIX}/share/man)

do-test:
	@${SH} -n ${WRKSRC}/usr/local/etc/rc.d/bastille
	@${SH} -n ${WRKSRC}/usr/local/share/bastille/common.sh
	@${GREP} -Fq 'case "$$(${PREFIX}/bin/midclt call system.ready 2>/dev/null)" in' \
		${WRKSRC}/usr/local/etc/rc.d/bastille
	@${GREP} -Fq 'true|True) return 0 ;;' \
		${WRKSRC}/usr/local/etc/rc.d/bastille
	@${GREP} -Fq '${PREFIX}/bin/midclt call bastille.rc_preflight "$${1}" >/dev/null' \
		${WRKSRC}/usr/local/etc/rc.d/bastille
	@${TEST} "$$(${GREP} -Fc 'bastille_middleware_ready || return 0' \
		${WRKSRC}/usr/local/etc/rc.d/bastille)" -eq 3
	@${GREP} -Fq 'bastille_rc_preflight "start" || return 1' \
		${WRKSRC}/usr/local/etc/rc.d/bastille
	@${GREP} -Fq 'bastille_rc_preflight "stop" || return 1' \
		${WRKSRC}/usr/local/etc/rc.d/bastille
	@${GREP} -Fq 'bastille_rc_preflight "restart" || return 1' \
		${WRKSRC}/usr/local/etc/rc.d/bastille
	@${GREP} -Fq '${PREFIX}/bin/midclt call bastille.rc_postflight "$${1}" >/dev/null' \
		${WRKSRC}/usr/local/etc/rc.d/bastille
	@${GREP} -Fq 'bastille_rc_postflight "start"' \
		${WRKSRC}/usr/local/etc/rc.d/bastille
	@${GREP} -Fq 'bastille_rc_postflight "stop"' \
		${WRKSRC}/usr/local/etc/rc.d/bastille
	@${GREP} -Fq 'bastille_rc_postflight "restart"' \
		${WRKSRC}/usr/local/etc/rc.d/bastille
	@${TEST} "$$(${GREP} -Fc 'local _status=$$?' \
		${WRKSRC}/usr/local/etc/rc.d/bastille)" -eq 3
	@${TEST} "$$(${GREP} -Ec 'xargs .*\\|\\| return 1' \
		${WRKSRC}/usr/local/etc/rc.d/bastille)" -eq 0
	@${TEST} "$$(${GREP} -Fc '/usr/bin/env -i' \
		${WRKSRC}/usr/local/etc/rc.d/bastille)" -eq 3
	@${TEST} "$$(${GREP} -Fc 'BASTILLE_CONFIG="$${bastille_conf}"' \
		${WRKSRC}/usr/local/etc/rc.d/bastille)" -eq 3
	@${TEST} "$$(${GREP} -Fc 'LC_ALL=C' \
		${WRKSRC}/usr/local/etc/rc.d/bastille)" -eq 3
	@${TEST} "$$(${GREP} -Fc 'NO_COLOR=1' \
		${WRKSRC}/usr/local/etc/rc.d/bastille)" -eq 3
	@${TEST} "$$(${GREP} -Fc 'PATH=/sbin:/bin:/usr/sbin:/usr/bin:${PREFIX}/sbin:${PREFIX}/bin' \
		${WRKSRC}/usr/local/etc/rc.d/bastille)" -eq 3
	@${TEST} "$$(${GREP} -Fc 'SH=/bin/sh' \
		${WRKSRC}/usr/local/etc/rc.d/bastille)" -eq 3
	@${TEST} "$$(${GREP} -Fc 'UMASK=022' \
		${WRKSRC}/usr/local/etc/rc.d/bastille)" -eq 3
	@${PRINTF} '%s\n' \
		'freecore-bastille-lifecycle-v3' \
		'upstream=1.4.0.260219' \
		'portrevision=5' | \
		${DIFF} -u ${FILESDIR}/freecore-lifecycle-v3 -
	@${GREP} -Fq 'devfs_ruleset_value=4' \
		${WRKSRC}/usr/local/share/bastille/create.sh
	@${GREP} -Fq '[ "$${VNET_INTERFACE_TYPE}" = "standard" ]' \
		${WRKSRC}/usr/local/share/bastille/create.sh
	@${GREP} -Fq 'HUP INT TERM; ifconfig epair create name $${host_epair}' \
		${WRKSRC}/usr/local/share/bastille/common.sh
	@${GREP} -Fq 'ifconfig -D $${host_epair}' \
		${WRKSRC}/usr/local/share/bastille/common.sh
	@${GREP} -Fq 'SYSRC ifconfig_$${VNET}="$${IFCONFIG} -txcsum -txcsum6"' \
		${WRKSRC}/usr/local/share/bastille/templates/default/vnet/Bastillefile
	@${GREP} -Fq 'if [ -f "$${bastille_pf_conf}" ] && route -n get "$${ip}"' \
		${WRKSRC}/usr/local/share/bastille/start.sh
	@${GREP} -Fq 'if [ -f "$${bastille_pf_conf}" ] && route -6 -n get "$${ip}"' \
		${WRKSRC}/usr/local/share/bastille/start.sh
	@${GREP} -Fq 'if [ -f "$${bastille_pf_conf}" ] && [ -n "$$(pfctl' \
		${WRKSRC}/usr/local/share/bastille/destroy.sh

.include <bsd.port.mk>
