PORTNAME=	ugreen-led-ctl
DISTVERSION=	g20260730
CATEGORIES=	sysutils

MAINTAINER=	dev@freecore.org
COMMENT=	Experimental FreeBSD CLI for UGREEN NASync LED validation
WWW=		https://github.com/aloli/ugreen-hal

LICENSE=	BSD2CLAUSE
LICENSE_FILE=	${WRKSRC}/LICENSE

USES=		uidfix
USE_GITHUB=	yes
GH_ACCOUNT=	aloli
GH_PROJECT=	ugreen-hal
GH_TAGNAME=	e8e41449b929cc6af17631629604856fd940c9e0

WRKSRC_SUBDIR=	src/cli
MAKE_ARGS=	BINDIR=${PREFIX}/bin

PLIST_FILES=	bin/ugreen-led-ctl

.include <bsd.port.mk>
