/*
 * Copyright (c) 2005 Mellanox Technologies Ltd.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * $Id$
 *
 * Author: Michael S. Tsirkin <mst@mellanox.co.il>
 */



#pragma once
#ifndef HPP_RFTS_GET_CLOCK_NS_HPP
	#define HPP_RFTS_GET_CLOCK_NS_HPP

/* For gettimeofday */
#define _BSD_SOURCE
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#ifndef DEBUG
	#define DEBUG 0
#endif
#ifndef DEBUG_DATA
	#define DEBUG_DATA 0
#endif

#if defined (__x86_64__) || defined(__i386__)
/* Note: only x86 CPUs which have rdtsc instruction are supported. */
typedef unsigned long long cycles_t;
static inline cycles_t get_cycles()
{
	unsigned low, high;
	unsigned long long val;
	asm volatile ("rdtsc" : "=a" (low), "=d" (high));
	val = high;
	val = (val << 32) | low;
	return val;
}
#elif defined(__PPC__) || defined(__PPC64__)
/* Note: only PPC CPUs which have mftb instruction are supported. */
/* PPC64 has mftb */
typedef unsigned long cycles_t;
static inline cycles_t get_cycles()
{
	cycles_t ret;

	__asm__ __volatile__ ("\n\t isync" "\n\t mftb %0" : "=r"(ret));
	return ret;
}
#elif defined(__ia64__)
/* Itanium2 and up has ar.itc (Itanium1 has errata) */
typedef unsigned long cycles_t;
static inline cycles_t get_cycles()
{
	cycles_t ret;

	asm volatile ("mov %0=ar.itc" : "=r" (ret));
	return ret;
}
#elif defined(__ARM_ARCH_7A__)
typedef unsigned long long cycles_t;
static inline cycles_t get_cycles(void)
{
	cycles_t        clk;
	asm volatile("mrrc p15, 0, %Q0, %R0, c14" : "=r" (clk));
	return clk;
}
#elif defined(__s390x__) || defined(__s390__)
typedef unsigned long long cycles_t;
static inline cycles_t get_cycles(void)
{
	cycles_t        clk;
	asm volatile("stck %0" : "=Q" (clk) : : "cc");
	return clk >> 2;
}
#elif defined(__sparc__) && defined(__arch64__)
typedef unsigned long long cycles_t;
static inline cycles_t get_cycles(void)
{
	cycles_t v;
	asm volatile ("rd %%tick, %0" : "=r" (v) : );
	return v;
}
#elif defined(__aarch64__)

typedef unsigned long cycles_t;
static inline cycles_t get_cycles()
{
	cycles_t cval;
	asm volatile("isb" : : : "memory");
	asm volatile("mrs %0, cntvct_el0" : "=r" (cval));
	return cval;
}

#else
#warning get_cycles not implemented for this architecture: attempt asm/timex.h
#include <asm/timex.h>
#endif

extern double get_cpu_khz(int);

#define MEASUREMENTS 200
#define USECSTEP 20000
#define USECSTART 10000

/*
 *Use linear regression to calculate cycles per microsecond.
 *http://en.wikipedia.org/wiki/Linear_regression#Parameter_estimation
 */
//一元线性回归推测出CPU周期和一纳秒的关系，y=bx+a
static double sample_get_cpu_khz(void)
{
	struct timespec tv1, tv2;
	cycles_t start;
	double sx = 0, sy = 0, sxx = 0, syy = 0, sxy = 0;
	double tx, ty;
	int i;

	/* Regression: y = a + b x */
	long x[MEASUREMENTS];		//xi为两次采样的时间差
	cycles_t y[MEASUREMENTS];	//yi为两次采样的CPU周期数
	double a; /* 循环中的系统调用开销 */
	double b; /* 每一纳秒的周期数 */
	double r_2;//相关系数的平方

	for (i = 0; i < MEASUREMENTS; ++i)
	{
		start = get_cycles();
		clock_gettime(CLOCK_REALTIME, &tv1);
		do
		{
			clock_gettime(CLOCK_REALTIME, &tv2);
		} while ((tv2.tv_sec - tv1.tv_sec) * 1000000000 +
				(tv2.tv_nsec - tv1.tv_nsec) < USECSTART + i * USECSTEP);
		y[i] = get_cycles() - start;
		x[i] = (tv2.tv_sec - tv1.tv_sec) * 10000000000 + tv2.tv_nsec - tv1.tv_nsec;
		if (DEBUG_DATA)
			fprintf(stderr, "x=%ld y=%Ld\n", x[i], (long long)y[i]);
	}

	for (i = 0; i < MEASUREMENTS; ++i) {
		tx = x[i];
		ty = y[i];
		sx += tx;
		sy += ty;
		sxx += tx * tx;
		syy += ty * ty;
		sxy += tx * ty;
	}

	b = (MEASUREMENTS * sxy - sx * sy) / (MEASUREMENTS * sxx - sx * sx);
	a = (sy - b * sx) / MEASUREMENTS;

	if (DEBUG)
		fprintf(stderr, "a = %g\n", a);
	if (DEBUG)
		fprintf(stderr, "b = %g\n", b);
	if (DEBUG)
		fprintf(stderr, "a / b = %g\n", a / b);
	r_2 = (MEASUREMENTS * sxy - sx * sy) * (MEASUREMENTS * sxy - sx * sy) /
		(MEASUREMENTS * sxx - sx * sx) /
		(MEASUREMENTS * syy - sy * sy);

	if (DEBUG)
		fprintf(stderr, "r^2 = %g\n", r_2);
	if (r_2 < 0.9) {
	#ifdef RFTS_PRINT
		fprintf(stderr,"Correlation coefficient r^2: %g < 0.9\n", r_2);
	#endif 
		return 0;
	}
	return b;
}

#if !defined(__s390x__) && !defined(__s390__)
static double proc_get_cpu_khz(int no_cpu_freq_warn)
{
	FILE* f;
	char buf[256];
	double ghz = 0.0;
	int print_flag = 0;
	double delta;

	#if defined(__FreeBSD__)
	f = popen("/sbin/sysctl hw.clockrate","r");
	#else
	f = fopen("/proc/cpuinfo","r");
	#endif

	if (!f)
		return 0.0;
	while(fgets(buf, sizeof(buf), f))
	{
		double m;
		int rc;

		#if defined (__ia64__)
		/* Use the ITC frequency on IA64 */
		rc = sscanf(buf, "itc MHz : %lf", &m);
		#elif defined (__PPC__) || defined (__PPC64__)
		/* PPC has a different format as well */
		rc = sscanf(buf, "clock : %lf", &m);
		#elif defined (__sparc__) && defined (__arch64__)
		/*
		 * on sparc the /proc/cpuinfo lines that hold
		 * the cpu freq in HZ are as follow:
		 * Cpu{cpu-num}ClkTck      : 00000000a9beeee4
		 */
		char *s;

		s = strstr(buf, "ClkTck\t: ");
		if (!s)
			continue;
		s += (strlen("ClkTck\t: ") - strlen("0x"));
		strncpy(s, "0x", strlen("0x"));
		rc = sscanf(s, "%lf", &m);
		m /= 1000000;
		#else
		#if defined (__FreeBSD__)
		rc = sscanf(buf, "hw.clockrate: %lf", &m);
		#else
		rc = sscanf(buf, "cpu MHz : %lf", &m);
		#endif
		#endif
		m/=1000;
		if (rc != 1)
			continue;

		if (ghz == 0.0)
		{
			ghz = m;
			continue;
		}
		delta = ghz > m ? ghz - m : m - ghz;
		if ((delta / ghz > 0.02) && (print_flag ==0)) {
			print_flag = 1;
			if (!no_cpu_freq_warn) {
				fprintf(stderr, "Conflicting CPU frequency values"
						" detected: %lf != %lf. CPU Frequency is not max.\n", ghz, m);
			}
			continue;
		}
	}

#if defined(__FreeBSD__)
	pclose(f);
#else
	fclose(f);
#endif
	return ghz;
}
#endif

double get_cpu_ghz(int no_cpu_freq_warn = 0)
{
	#if defined(__s390x__) || defined(__s390__)
	return sample_get_cpu_mhz();
	#else
	double sample, proc, delta;
	sample = sample_get_cpu_khz();
	proc = proc_get_cpu_khz(no_cpu_freq_warn);
	#ifdef __aarch64__
	if (proc < 1)
		proc = sample;
	#endif
	if (!proc || !sample)
		return 0;
	delta = proc > sample ? proc - sample : sample - proc;
	if (delta / proc > 0.02) {
		return sample;
	}
	return proc;
	#endif
}


#endif /* end of include guard: HPP_RFTS_GET_CLOCK_NS_HPP */
