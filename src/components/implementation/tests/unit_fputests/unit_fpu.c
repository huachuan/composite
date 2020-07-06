/*
 * Copyright 2016, Phani Gadepalli and Gabriel Parmer, GWU, gparmer@gwu.edu.
 *
 * This uses a two clause BSD License.
 */

#include <cos_defkernel_api.h>
#include <llprint.h>
#include <res_spec.h>
#include <sl.h>

#define FIXED_PRIORITY 10

static int thd1_reg[NUM_CPU] = { 0 };
static int thd2_fpu[NUM_CPU] = { 0 };
static int thd3_fpu[NUM_CPU] = { 0 };

static void
reg_thd_fn()
{
	thd1_reg[cos_cpuid()] = 1;
	while (1);
}

static void
pi_thd_fn()
{
	thd2_fpu[cos_cpuid()] = 1;
	float    PI = 3.0;
	int      flag = 1, i;
	for (i = 2; i < 100000; i += 2) {	
		if (flag) {
			PI += (4.0 / (i * (i + 1) * (i + 2)));
		} else {
			PI -= (4.0 / (i * (i + 1) * (i + 2)));
		}
		flag = !flag;
	}
        /*PRINTC("\tpi = %f: \t\t\tFinish calculate Pi\n", PI);*/
	sl_thd_exit();
}

static void
euler_thd_fn()
{
	thd3_fpu[cos_cpuid()] = 1;
	float    E = 1.0, fact = 1.0;
	int    i;
	for (i = 1; i < 1000; i++) {	
		fact *= i;
		E += (1.0 / fact);
	}
        /*PRINTC("\te = %f: \t\t\tFinish calculate E\n", E);*/
	sl_thd_exit();
}
static void
allocator_ffthread_fn()
{
	struct sl_thd *thd1, *thd2;
	cycles_t wakeup;

	thd1 = sl_thd_alloc(pi_thd_fn, NULL);
	sl_thd_param_set(thd1, sched_param_pack(SCHEDP_PRIO, FIXED_PRIORITY));

	thd2 = sl_thd_alloc(euler_thd_fn, NULL);
	sl_thd_param_set(thd2, sched_param_pack(SCHEDP_PRIO, FIXED_PRIORITY));

	wakeup = sl_now() + sl_usec2cyc(1000 * 1000);
	sl_thd_block_timeout(0, wakeup);

	sl_thd_free(thd1);
	sl_thd_free(thd2);

	sl_thd_exit();
}

static void
allocator_frthread_fn()
{
	struct sl_thd *thd1, *thd2;
	cycles_t wakeup;

	thd1 = sl_thd_alloc(reg_thd_fn, NULL);
	sl_thd_param_set(thd1, sched_param_pack(SCHEDP_PRIO, FIXED_PRIORITY));

	thd2 = sl_thd_alloc(pi_thd_fn, NULL);
	sl_thd_param_set(thd2, sched_param_pack(SCHEDP_PRIO, FIXED_PRIORITY));

	wakeup = sl_now() + sl_usec2cyc(1000 * 1000);
	sl_thd_block_timeout(0, wakeup);

	sl_thd_free(thd1);
	sl_thd_free(thd2);

	sl_thd_exit();
}
static void
test_swapping(int d)
{
	struct sl_thd *allocator_thread;
	cycles_t wakeup;
	if (d == 0) {
		allocator_thread = sl_thd_alloc(allocator_ffthread_fn, NULL);
	} else if (d == 1) {
		allocator_thread = sl_thd_alloc(allocator_frthread_fn, NULL);
	}
	sl_thd_param_set(allocator_thread, sched_param_pack(SCHEDP_PRIO, FIXED_PRIORITY));

	wakeup = sl_now() + sl_usec2cyc(100 * 1000);
	sl_thd_block_timeout(0, wakeup);
	sl_thd_free(allocator_thread);
}

static void
run_tests()
{
	test_swapping(0);
	PRINTC("%s: Swap back and forth between fpu and fpu thread!\n", (thd2_fpu[cos_cpuid()] && thd3_fpu[cos_cpuid()]) ? "SUCCESS" : "FAILURE");
	test_swapping(1);
	PRINTC("%s: Swap back and forth between fpu and reg thread!\n", (thd1_reg[cos_cpuid()] && thd2_fpu[cos_cpuid()]) ? "SUCCESS" : "FAILURE");
	PRINTC("Unit-test done!\n");
	sl_thd_exit();
}

void
cos_init(void)
{
	int i;
	static unsigned long first = NUM_CPU + 1, init_done[NUM_CPU] = { 0 };
	struct sl_thd *testing_thread;
	struct cos_defcompinfo *defci = cos_defcompinfo_curr_get();
	struct cos_compinfo    *ci    = cos_compinfo_get(defci);
        cos_hw_cycles_per_usec(BOOT_CAPTBL_SELF_INITHW_BASE);
	PRINTC("Unit-test for the fpu\n");

	if (ps_cas(&first, NUM_CPU + 1, cos_cpuid())) {
		cos_meminfo_init(&(ci->mi), BOOT_MEM_KM_BASE, COS_MEM_KERN_PA_SZ, BOOT_CAPTBL_SELF_UNTYPED_PT);
		cos_defcompinfo_init();
	} else {
		while (!ps_load(&init_done[first])) ;

		cos_defcompinfo_sched_init();
	}
	ps_faa(&init_done[cos_cpuid()], 1);
	for (i = 0; i < NUM_CPU; i++) {
		while (!ps_load(&init_done[i])) ;
	}

	sl_init(SL_MIN_PERIOD_US);

	testing_thread = sl_thd_alloc(run_tests, NULL);
	sl_thd_param_set(testing_thread, sched_param_pack(SCHEDP_PRIO, FIXED_PRIORITY));

	sl_sched_loop();

	assert(0);

	return;
}
