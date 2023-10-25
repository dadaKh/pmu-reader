/*
 * Enable user-mode ARM performance counter access.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/smp.h>
#include <linux/version.h>

#if !defined(__aarch64__)
#error Module can only be compiled on ARM64 machines.
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0)
#define pmu_access_ok(x,y,z) access_ok((y),(z))
#else
#define pmu_access_ok(x,y,z) access_ok((x),(y),(z))
#endif


static void show_counters_configuration(void* data)
{
	u64 val;
	asm volatile("mrs %0, pmintenclr_el1" : "=r" (val));
	printk(KERN_INFO "pmintenclr_el1:0x%llx", val);
	asm volatile("mrs %0, pmcntenset_el0" : "=r" (val));
	printk(KERN_INFO "pmcntenset_el0:0x%llx", val);
	asm volatile("mrs %0, pmuserenr_el0" : "=r" (val));
	printk(KERN_INFO "pmuserenr_el0:0x%llx", val);
	asm volatile("mrs %0, pmcr_el0" : "=r" (val));
	printk(KERN_INFO "pmcr_el0:0x%llx", val);
	asm volatile("mrs %0, pmccfiltr_el0" : "=r" (val));
	printk(KERN_INFO "pmccfiltr_el0:0x%llx", val);
}

static void enable_counters_el0(void* data)
{
	u64 val;
	/* Disable counter overflow interrupt */
	/* 0-30 bit corresponds to different pmu counter */
	asm volatile("msr pmintenclr_el1, %0" : : "r" ((u64)(1 << 31) | BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0)));
	
	/* Enable cycle counter */
	/* 0-30 bit corresponds to different pmu counter */
    // 使能相应bit后，可使用PMEVTYPER<n>_EL0配置PMU事件，并读取PMEVCNTR<n>_EL0获得计数信息
	asm volatile("msr pmcntenset_el0, %0" : : "r" (BIT(31) | BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0)));
	
	/* Enable user-mode access to pmu counters. */
	asm volatile("msr pmuserenr_el0, %0" : : "r"(BIT(0) | BIT(2) | BIT(3)));
	
	/* Clear counters and start */
	asm volatile("mrs %0, pmcr_el0" : "=r" (val));
	val |= (BIT(0) | BIT(1) | BIT(2) | BIT(6));
	
	isb();
	asm volatile("msr pmcr_el0, %0" : : "r" (val));

	val = BIT(27);
	asm volatile("msr pmccfiltr_el0, %0" : : "r" (val));

	//show_counters_configuration(void* data);

	printk(KERN_INFO "enable finished: cpu%d", smp_processor_id());
}

static void disable_counters_el0(void* data)
{
	/* Disable all counters */
	asm volatile("msr pmcntenset_el0, %0" :: "r" ((u64)0));
	/* Disable user-mode access to counters. */
	u64 val;
	asm volatile("mrs %0, pmuserenr_el0" : "=r"(val));
	val &= ~(1);
	asm volatile("msr pmuserenr_el0, %0" : : "r"(val));
}

static int __init init(void)
{
	on_each_cpu(enable_counters_el0, NULL, 1);
	return 0;
}

static void __exit fini(void)
{
	on_each_cpu(disable_counters_el0, NULL, 1);
}

MODULE_DESCRIPTION("Enables user-mode access to ARMv8 PMU counters");
MODULE_LICENSE("GPL");
module_init(init);
module_exit(fini);