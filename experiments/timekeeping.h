#ifndef _time_keeping_c_
#define _time_keeping_c_

#include <time.h> 
#include <sys/time.h>
#include <sys/resource.h>

#ifdef _WIN32
inline static double second()
{
	return ((double)clock()/(double)CLK_TCK);
}

#else

#include <sys/resource.h>

#if defined(__MACH__) && defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

inline static double myWallTime()
{
#ifdef __APPLE__
	static double timeConvert = 0.0;
	if ( timeConvert == 0.0 )
	{
		mach_timebase_info_data_t timeBase;
		mach_timebase_info(&timeBase);
		timeConvert = (double)timeBase.numer / (double)timeBase.denom / 1000000000.0;
	}
	return mach_absolute_time() * timeConvert;
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + 1.0e-9*((double)ts.tv_nsec);
#endif // __APPLE__
}

inline static double second()
{
	double t = myWallTime();
	return(t);
}

#endif


// Measure peak memory usage
size_t measurePeakMemory(){
  struct rusage t;
  getrusage(RUSAGE_SELF, &t);
  return (size_t)t.ru_maxrss;
}



#endif
