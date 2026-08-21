.include "${.CURDIR}/../nvidia-driver-headless/Makefile.version"
DISTVERSION=	${NVIDIA_HEADLESS_VERSION}
PORTREVISION=	1
PKGNAMESUFFIX=	-headless
MAINTAINER=	dev@freecore.org
COMMENT=	NVIDIA kernel modules for opt-in native headless telemetry

# Retain the upstream kernel build and FreeBSD KPI patches, but keep the
# independently pinned headless pair out of the desktop driver's versioning.
MASTERDIR=	${.CURDIR}/../nvidia-kmod
OPTIONS_EXCLUDE=	LINUX
OPTIONS_DEFAULT=	ACPI_PM

# The vendor source reaches for syscons to locate the console framebuffer when
# no EFI framebuffer is present.  This appliance kernel is vt(4)-only, so
# sc_get_softc() is undefined and nvidia.ko fails to link with
# "link_elf_obj: symbol sc_get_softc undefined".  A headless build has no
# console to protect, and UEFI systems take the EFI path above it, so the
# fallback is dropped here rather than adding syscons back to the kernel.
# Applied only to the headless slave; the desktop driver keeps vendor behaviour.
EXTRA_PATCHES=	${.CURDIR}/files/extra-patch-src-nvidia-nvidia_subr.c

.include "${MASTERDIR}/Makefile"

# Override the master's generated desktop/modeset-oriented install message.
PKGMESSAGE=	${.CURDIR}/pkg-message
