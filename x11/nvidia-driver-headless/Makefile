PORTNAME=	nvidia-driver
PKGNAMESUFFIX=	-headless
.include "${.CURDIR}/Makefile.version"
DISTVERSION=	${NVIDIA_HEADLESS_VERSION}
PORTREVISION=	1
CATEGORIES=	x11
MASTER_SITES=	NVIDIA/XFree86/FreeBSD-${ARCH_SUFX}/${DISTVERSION}
DISTNAME=	NVIDIA-FreeBSD-${ARCH_SUFX}-${DISTVERSION}

MAINTAINER=	dev@freecore.org
COMMENT=	Native NVIDIA SMI and NVML without a desktop graphics stack
WWW=		https://www.nvidia.com/object/unix.html

LICENSE_FILE=	${WRKSRC}/doc/license.txt
.include "${.CURDIR}/../nvidia-driver/Makefile.common"

# USES=kmod adds the build's OSVERSION to the kernel package version, so this
# keeps the vendor version and the kernel ABI matched.  It must be ">=", not
# "=": a PORTREVISION on the kmod appends _N to its package version, which an
# exact pin can never satisfy, and the run-depends phase then fails even though
# the kmod built correctly.  ">=" still rejects a kmod built for a different
# OSVERSION, and both halves are built from this tree in the same run.
RUN_DEPENDS=	nvidia-kmod-headless>=${DISTVERSION}.${OSVERSION}:x11/nvidia-kmod-headless

USES=		tar:xz
USE_LDCONFIG=	yes
NO_BUILD=	yes
# Preserve the vendor binaries byte-for-byte, including during staging.
STRIP=

CONFLICTS_INSTALL=	nvidia-driver* xlibre-nvidia-driver*
# Appliance slimming removes share/doc; the redistribution license must stay.
DATADIR=	${PREFIX}/share/nvidia-headless

do-install:
	${INSTALL_PROGRAM} ${WRKSRC}/obj/nvidia-smi ${STAGEDIR}${PREFIX}/bin
	${INSTALL_LIB} ${WRKSRC}/obj/libnvidia-ml.so.1 ${STAGEDIR}${PREFIX}/lib
	${LN} -s libnvidia-ml.so.1 ${STAGEDIR}${PREFIX}/lib/libnvidia-ml.so
	${INSTALL_MAN} ${WRKSRC}/nvml/man/nvidia-smi.1 ${STAGEDIR}${PREFIX}/share/man/man1
	@${MKDIR} ${STAGEDIR}${DATADIR}
	${INSTALL_DATA} ${LICENSE_FILE} ${STAGEDIR}${DATADIR}/license.txt
	${INSTALL_DATA} ${.CURDIR}/README.md ${STAGEDIR}${DATADIR}/README.md

.include <bsd.port.mk>
