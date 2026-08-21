#!/bin/sh

set -eu

wrksrc=${1:?collectd source directory is required}
types_db=${wrksrc}/src/types.db

emitted_types=$(
	{
		${SED:-sed} -nE \
			's/^#define[[:space:]]+NFSSTAT_PLUGIN_NAME[[:space:]]+"([^"]+)".*/\1/p' \
			"${wrksrc}/src/nfsstat.c"
		${SED:-sed} -nE \
			's/.*geom_submit(_gauge|_trip)?[[:space:]]*\([[:space:]]*"([^"]+)".*/\2/p' \
			"${wrksrc}/src/geom_stat.c"
		${SED:-sed} -nE \
			's/^[[:space:]]*\{[[:space:]]*"[^"]+"[[:space:]]*,[[:space:]]*"([^"]+)".*/\1/p' \
			"${wrksrc}/src/zfs_arc_v2.c"
		${SED:-sed} -nE \
			's/.*za_v2_submit_gauge[[:space:]]*\([[:space:]]*"([^"]+)".*/\1/p' \
			"${wrksrc}/src/zfs_arc_v2.c"
	} | sort -u
)

count=$(printf '%s\n' "${emitted_types}" | awk 'NF { count++ } END { print count + 0 }')
if [ "${count}" -ne 32 ]; then
	echo "expected 32 custom collectd data-set types, found ${count}" >&2
	exit 1
fi

missing=0
for type in ${emitted_types}; do
	if ! grep -Eq "^${type}[[:space:]]+" "${types_db}"; then
		echo "custom collectd data-set is undefined: ${type}" >&2
		missing=1
	fi
done

exit "${missing}"
