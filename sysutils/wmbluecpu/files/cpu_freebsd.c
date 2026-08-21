/*
 *  cpu_freebsd.c - get cpu usage
 *
 *  Copyright (C) 2003 Alexey Dokuchaev <danfe@regency.nsu.ru>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
 */

#include <osreldate.h>
#if __FreeBSD_version > 500100
#include <sys/time.h>
#include <sys/resource.h>
#else
#include <sys/dkstat.h>
#endif
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int	cp_time_mib[2];

void
cpu_init(void)
{
	size_t len = 2;
	if (sysctlnametomib("kern.cp_time", cp_time_mib, &len) != 0) {
		perror("sysctlnametomib, kern.cp_time");
		exit(1);
	}

	seteuid(getuid());
	setegid(getgid());

	if (geteuid() != getuid() || getegid() != getgid())
	{
		perror("sete?id");
		exit(1);
	}
}

long cpu_used;
long oldused;
long oldtotal;

int history[17];

void cpu_getusage()
{
	long cpu, nice, system, idle, used, total;
	long cpu_time[CPUSTATES], cpu_time_len=sizeof(cpu_time);
	int error;

	error = sysctl(cp_time_mib, 2, cpu_time, &cpu_time_len, NULL, 0);
	if (error) {
		perror("sysctl, kern.cp_time");
		exit(1);
	}

	cpu = cpu_time[CP_USER];
	nice = cpu_time[CP_NICE];
	system = cpu_time[CP_SYS];
	idle = cpu_time[CP_IDLE];

	used = cpu + nice + system;
	total = used + idle;

	if ((total - oldtotal) != 0)
		cpu_used = (100 * (double)(used - oldused)) /
			(double)(total - oldtotal);
	else
		cpu_used = 0;

	oldused = used;
	oldtotal = total;

	memmove(history + 1, history, 16 * sizeof(int));
	history[0] = (double)cpu_used * 16 / 100;
}
