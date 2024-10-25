// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file apps/pilot/kalngyk/cubic.hh
/// @author YK Ng & SC Li (kalngyk@gmail.com)

#ifndef apps_pilot_kalngyk_cubic_HH
#define apps_pilot_kalngyk_cubic_HH

#ifndef PYROSETTA
#endif
#if !defined(__WIN32__) && !defined(WIN32)
#endif
#include <core/types.hh>

namespace protocols {
namespace cluster {
namespace calibur {

//- = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = -

#ifdef __TEST_CUBIC__
#ifndef __WIN32__
#ifndef PYOSETTA
static core::Real
__timeval_difference(struct timeval * x, struct timeval * y)
{
	core::Real elapsed;
	if (x->tv_usec < y->tv_usec)
	{
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000)
	{
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	elapsed = x->tv_sec - y->tv_sec;
	elapsed += (x->tv_usec - y->tv_usec)/(core::Real)1000000;
	return elapsed;
}

static core::Real
_get_elapsed(int set_start)
{
	static struct rusage last;
	struct rusage now;
	core::Real elapsed = 0;
	if (set_start)
		getrusage(RUSAGE_SELF, &last);
	else
	{
		getrusage(RUSAGE_SELF, &now);
		elapsed += __timeval_difference(&(now.ru_utime), &(last.ru_utime));
		elapsed += __timeval_difference(&(now.ru_stime), &(last.ru_stime));
		last = now;
	}
	return elapsed;
}
#endif
#endif
#endif

//- = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = -


void cubic_roots1(core::Real a2, core::Real a1, core::Real a0, core::Real * z);

/**
* This is faster but leaves no option for obtaining imaginary roots.
* (After "-O" in GCC, the speed gain is not much, only ~4%)
*/
void cubic_roots2(core::Real a2, core::Real a1, core::Real a0, core::Real* z);

}
}
}

#endif
