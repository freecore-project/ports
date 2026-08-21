"""Package-contract tests; optional vendor archive checks never execute its ELFs."""

import hashlib
import os
from pathlib import Path
import re
import subprocess
import tarfile
import tempfile
import unittest


PORT = Path(__file__).resolve().parents[1]
KMOD = PORT.parent / 'nvidia-kmod-headless'
VERSION = '580.178.04'
ARCHIVE_SHA256 = '5d9f704562f43f05e30b2c99964b3b3ff6397740bbb4ebb61840b48352b1d347'
BINARY_HASHES = {
    'nvidia-smi': '4f055484a5c8eb4d0d498c56d4a8c2726e4210b282bc2f4d16bb161b02ffac78',
    'libnvidia-ml.so.1': '5d7254920a17ea0f0978389bdfae2fc4ec3969b545968dd457f9d828e79c8910',
}


class PackageContractTests(unittest.TestCase):
    def test_shared_pin_does_not_change_the_desktop_version(self):
        self.assertRegex((PORT / 'Makefile.version').read_text(), rf'NVIDIA_HEADLESS_VERSION=\s*{VERSION}')
        for makefile in [PORT / 'Makefile', KMOD / 'Makefile']:
            self.assertIn('DISTVERSION=\t${NVIDIA_HEADLESS_VERSION}', makefile.read_text())
        self.assertNotIn('NVIDIA_HEADLESS_VERSION',
                         (PORT.parent / 'nvidia-driver/Makefile.version').read_text())

    def test_exact_kernel_dependency_and_upstream_slave(self):
        makefile = (PORT / 'Makefile').read_text()
        self.assertIn('nvidia-kmod-headless=${DISTVERSION}.${OSVERSION}:x11/nvidia-kmod-headless', makefile)
        kernel = (KMOD / 'Makefile').read_text()
        for expected in ['PORTREVISION=\t0', 'PKGNAMESUFFIX=\t-headless',
                         'MASTERDIR=\t${.CURDIR}/../nvidia-kmod', 'OPTIONS_EXCLUDE=\tLINUX']:
            self.assertIn(expected, kernel)

    def test_payload_is_only_native_telemetry_and_notices(self):
        self.assertEqual((PORT / 'pkg-plist').read_text().splitlines(), [
            'bin/nvidia-smi', 'lib/libnvidia-ml.so', 'lib/libnvidia-ml.so.1',
            'share/man/man1/nvidia-smi.1.gz', '%%DATADIR%%/license.txt', '%%DATADIR%%/README.md',
        ])
        makefile = (PORT / 'Makefile').read_text()
        self.assertIn('DATADIR=\t${PREFIX}/share/nvidia-headless', makefile)
        self.assertRegex(makefile, r'(?m)^STRIP=$')
        self.assertIn('CONFLICTS_INSTALL=\tnvidia-driver* xlibre-nvidia-driver*', makefile)
        self.assertNotRegex(makefile, r'(?m)^(USE_RC_SUBR|LIB_DEPENDS|USE_XORG)=')
        self.assertNotIn('afterinstall', makefile)

    def test_distinfo_has_the_reviewed_vendor_identity(self):
        data = (PORT / 'distinfo').read_text()
        self.assertIn(f'SHA256 (NVIDIA-FreeBSD-x86_64-{VERSION}.tar.xz) = {ARCHIVE_SHA256}', data)
        self.assertIn(f'SIZE (NVIDIA-FreeBSD-x86_64-{VERSION}.tar.xz) = 289102996', data)

    def test_kernel_message_is_not_the_desktop_activation_recipe(self):
        kernel = (KMOD / 'Makefile').read_text()
        self.assertGreater(kernel.index('PKGMESSAGE='), kernel.index('.include "${MASTERDIR}/Makefile"'))
        self.assertIn('not enabled automatically', (KMOD / 'pkg-message').read_text())


@unittest.skipUnless(os.environ.get('NVIDIA_DISTFILE'), 'set NVIDIA_DISTFILE for real vendor checks')
class VendorArchiveTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.archive = Path(os.environ['NVIDIA_DISTFILE'])
        with cls.archive.open('rb') as stream:
            digest = hashlib.file_digest(stream, 'sha256').hexdigest()
        if digest != ARCHIVE_SHA256 or cls.archive.stat().st_size != 289102996:
            raise AssertionError('Vendor archive hash/size mismatch')
        cls.tmp = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.tmp.cleanup)
        cls.extracted = Path(cls.tmp.name)
        with tarfile.open(cls.archive, 'r:xz') as archive:
            for name in BINARY_HASHES:
                member = archive.getmember(f'NVIDIA-FreeBSD-x86_64-{VERSION}/obj/{name}')
                if not member.isfile():
                    raise AssertionError(f'Not a regular vendor binary: {name}')
                (cls.extracted / name).write_bytes(archive.extractfile(member).read())
            cls.license = archive.extractfile(f'NVIDIA-FreeBSD-x86_64-{VERSION}/doc/license.txt').read()

    def test_binary_hashes_and_freebsd_amd64_identity(self):
        for name, expected in BINARY_HASHES.items():
            data = (self.extracted / name).read_bytes()
            self.assertEqual(hashlib.sha256(data).hexdigest(), expected)
            self.assertEqual(data[:8], b'\x7fELF\x02\x01\x01\x09')
            self.assertEqual(int.from_bytes(data[18:20], 'little'), 62)
        self.assertIn(b'NVIDIA', self.license)

    @unittest.skipUnless(os.environ.get('NVIDIA_READELF'), 'set NVIDIA_READELF to inspect ELF closure')
    def test_complete_dt_needed_set_is_base_only(self):
        expected = {
            'nvidia-smi': {'libthr.so.3', 'libm.so.5', 'libc.so.7', 'librt.so.1'},
            'libnvidia-ml.so.1': {'libthr.so.3', 'libm.so.5', 'libc.so.7'},
        }
        for name, dependencies in expected.items():
            result = subprocess.run([os.environ['NVIDIA_READELF'], '-d', str(self.extracted / name)],
                                    check=True, capture_output=True, text=True)
            needed = set(re.findall(r'\(NEEDED\).*\[([^\]]+)\]', result.stdout))
            self.assertEqual(needed, dependencies)
            self.assertNotRegex(result.stdout, r'\((?:RUNPATH|RPATH)\)')


if __name__ == '__main__':
    unittest.main()
