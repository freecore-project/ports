# FreeCORE release delta — ports

[FreeCORE](https://freecore.org) carries the TrueNAS CORE 13.3 system
forward as an independently maintained operating system on FreeBSD.
TrueNAS CORE 13.3 systems upgrade straight to FreeCORE 15.0 in place,
then continue on the project’s update train.

Not affiliated with or endorsed by iXsystems, Inc.

## What this repository is

`0001-freecore-ports-release-delta.patch` is the reviewed FreeCORE release delta against
[`truenas/ports`](https://github.com/truenas/ports). The
multi-gigabyte upstream repository is not duplicated here.

| | |
|---|---|
| **Base** | TrueNAS 13.3-U1 ports tree, imported 17 April 2026 |
| **Base resolves publicly** | no — see below |
| **Licence** | BSD-2-Clause |

## Complete build source

The immutable `source/15.0-U1.1` branch contains the complete
FreeCORE release source as one parentless commit. It is the directly
cloneable build input and contains no private development history.

## Base limitation

The exact TrueNAS 13.3-U1 ports snapshot used by the build does not resolve as
a named public upstream commit. The release delta is retained for provenance
against that honestly described dated snapshot; it is not accompanied by an
apply command that cannot work. The complete, directly cloneable source used
by the public build is published separately on the history-free
`source/15.0-U1.1` branch in the same repository.

## Public history

This is one source-state delta, not an export of the development history.
Private commit subjects, bodies, issue references, dates, ordering, and
intermediate churn are not present. Release tags identify states that were
actually built and validated.

## Contributors represented in this delta

- FreeCORE

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md). Security reports go to
security@freecore.org, not to the issue tracker — see
[SECURITY.md](SECURITY.md).

## Licence and attribution

See [NOTICE](NOTICE) and [TRADEMARKS.md](TRADEMARKS.md). Nothing here is
relicensed; upstream copyright notices and licence texts are preserved.
