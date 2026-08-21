# Native headless NVIDIA support

FreeCORE 15.1 packages a version-matched NVIDIA kernel driver, `nvidia-smi`
and NVML. This is an opt-in driver/telemetry foundation, not a desktop,
CUDA, NVENC/transcoding or shared-jail rendering implementation.

The userspace binaries are copied unchanged from NVIDIA's FreeBSD archive.
The matching `nvidia-kmod-headless` slave uses the existing FreeBSD kernel-port
patches and the appliance's kernel sources. Linux compatibility is excluded.
Neither package adds a service, automatic driver load or jail device access.
The image additionally blocks NVIDIA demand-loading through `devmatch`.

After installation on a disposable hardware canary, explicitly load `nvidia`
and compare `nvidia-smi -q -x` with the GPU dashboard. For boot persistence,
use the existing System > Tunables RC `kld_list` mechanism, preserving its
existing modules and appending `nvidia`. A LOADER `nvidia_load` entry alone is
not the supported path: the inherited loader blacklist deliberately blocks it.
Do not enable `nvidia-modeset` or `nvidia-drm` for this headless acceptance.

Removing the opt-in tunable and rebooting must return to the dormant state.
Do not unload a driver while a workload owns the device. GPU-less machines
must remain dormant before and after reboot. Hybrid/Optimus laptop attachment
still requires hardware proof even when the GPU appears in the supported list.

Keep unsupported counters unavailable. SMI success proves neither video
encoding nor graphics/compute workloads. Device passthrough remains separately
owned and must not be claimed by the host driver.

## Provenance

- NVIDIA R580 FreeBSD archive: https://download.nvidia.com/XFree86/FreeBSD-x86_64/580.178.04/
- Upstream FreeBSD version/hash update: https://cgit.freebsd.org/ports/commit/?id=5b5cb97ca80e3255c15c6bf7bcb300ad707216f4
- The redistribution agreement is installed beside this document as `license.txt`.
- No version is claimed vulnerability-free; review vendor advisories before releases.
