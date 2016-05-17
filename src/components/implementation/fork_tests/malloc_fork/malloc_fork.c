#include <cos_component.h>
#include <print.h>
#include <quarantine.h>
#include <test_malloc_comp.h>

#define ITER (1024)
u64_t meas[ITER];

/*
 * Warning - test assumes Composite can run at least ITER components.
 * (And this may not take into account other components for this test
 */

void cos_init(void)
{
	printc("Starting a fork test with dynamic memory allocation\n");

	// malloc some stuff in component
	// fork it here
	// component continues to malloc stuff.
	// need malloc to be called via methods
	call();
/*
	u64_t start, end, avg, tot = 0, dev = 0;
	int i, j;
	spdid_t f;
	int iter = ITER;

	// need to find cbuf manager instead. That's the one, right?
	call();	
	f = quarantine_fork(cos_spd_id(), cos_spd_id()-1);
	if (f == 0) printc("Error: initial fork failed\n");

	printc("cpu %ld, thd %d\n",cos_cpuid(), cos_get_thd_id());
	printc("Starting %d Invocations and Forks.\n", ITER);

	for (i = 0 ; i < ITER ; i++) {
		rdtscll(start);
		f = quarantine_fork(cos_spd_id(), f);
		printc("return value of f is %d\n", f);
		rdtscll(end);
		meas[i] = end-start;

		if (f == 0) break;
	}
	if (f == 0) {
		printc("Only finished %d forks\n", i+1);
		iter = i+1;
	}

	for (i = 0 ; i < iter ; i++) tot += meas[i];
	avg = tot/iter;
	printc("avg %lld\n", avg);
	for (tot = 0, i = 0, j = 0 ; i < iter ; i++) {
		if (meas[i] < avg*2) {
			tot += meas[i];
			j++;
		}
	}
	printc("avg w/o %d outliers %lld\n", iter-j, tot/j);

	for (i = 0 ; i < iter ; i++) {
		u64_t diff = (meas[i] > avg) ? 
			meas[i] - avg : 
			avg - meas[i];
		dev += (diff*diff);
	}
	dev /= iter;
	printc("deviation^2 = %lld\n", dev);

	printc("%d invocations took %lld\n", ITER, end-start);
*/
	return;
}
