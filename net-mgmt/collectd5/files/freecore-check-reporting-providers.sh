#!/bin/sh

set -eu

wrksrc=${1:?collectd source directory is required}

# Check the FreeBSD memory provider used by the inherited Reporting graph.
# Inspect the key array and its submitted value together: a laundry label over
# the old cache counter would create the right RRD name with the wrong data.
if ! awk '
    /^#else \/\* Other HAVE_SYSCTLBYNAME providers \*\// { provider = 1; next }
    /^#endif \/\* HAVE_SYSCTL && KERNEL_NETBSD \*\// { provider = 0 }
    provider { source = source " " $0 }
    END {
        if (!match(source, /const char \*sysctl_keys\[8\] = \{[^}]*\}/))
            exit 1
        split(substr(source, RSTART, RLENGTH), keys, /"/)
        if (keys[14] != "vm.stats.vm.v_laundry_count")
            exit 1
        if (!match(source, /MEMORY_SUBMIT\([^;]*;/))
            exit 1
        submit = substr(source, RSTART, RLENGTH)
        if (submit !~ /"laundry",[[:space:]]*\(gauge_t\)sysctl_vals\[6\]/)
            exit 1
    }
' "${wrksrc}/src/memory.c"; then
	echo "Reporting provider missing: memory-laundry from vm.stats.vm.v_laundry_count" >&2
	exit 1
fi

# The miss series must be dispatched with the same identifier the graph uses.
if ! awk '
    /^static int za_read\(void\)/ { provider = 1 }
    provider { source = source " " $0 }
    /^} \/\* int za_read \*\// { provider = 0 }
    END {
        if (source !~ /za_read_derive\(ksp,[[:space:]]*"demand_metadata_misses",[[:space:]]*"cache_result",[[:space:]]*"demand_metadata-miss"\)/)
            exit 1
    }
' "${wrksrc}/src/zfs_arc.c"; then
	echo "Reporting provider missing: zfs_arc/cache_result-demand_metadata-miss" >&2
	exit 1
fi
