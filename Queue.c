#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/smp.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#define SUCCESS 0
#define DEVICE_NAME "MODULE4"
static int delay = HZ;
static struct tasklet_struct our_tasklet2;
static struct tasklet_struct our_tasklet;
static struct tasklet_struct our_tasklet5;
static struct tasklet_struct our_tasklet6;
static char our_tasklet_argument[20] = DEVICE_NAME, string_argument[20];
static unsigned long before_delay, after_delay, after_after_delay;
static int i, j;
static unsigned long before_delay2, after_delay2, after_after_delay2;
static int i2, j2;
static unsigned long before_delay3, after_delay3, after_after_delay3;
static int i3, j3;
static unsigned long before_delay4, after_delay4, after_after_delay4;
static int i4, j4;
static unsigned long before_delay5, after_delay5, after_after_delay5;
static int i5, j5;
static unsigned long before_delay6, after_delay6, after_after_delay6;
static int i6, j6;
static struct workqueue_struct *our_workqueue;
static struct workqueue_struct *our_workqueue3;
static struct workqueue_struct *our_workqueue4;
static struct workqueue_struct *our_workqueue5;

static void our_work_function6(struct work_struct *our_work6){
	after_after_delay6 = jiffies;
	printk(KERN_INFO "TASKLET_SHAREDQUEUE : Work function is running on CPU %d \n", smp_processor_id());
	printk(KERN_INFO "TASKLET_SHAREDQUEUE : Work function scheduled on: %ld \n", before_delay6);
	printk(KERN_INFO "TASKLET_SHAREDQUEUE : Work function executed on: %ld \n", after_delay6);
	printk(KERN_ALERT "TASKLET_SHAREDQUEUE : RUNTIME=  %ld \n", after_after_delay6 - before_delay6);
}
static DECLARE_DELAYED_WORK(our_work6, our_work_function6);
static void our_work_function5(struct work_struct *our_work5){
	after_after_delay5 = jiffies;
	printk(KERN_INFO "TASKLET_WORKQUEUE : Work function is running on CPU %d \n", smp_processor_id());
	printk(KERN_INFO "TASKLET_WORKQUEUE : Work function scheduled on: %ld \n", after_delay5);
	printk(KERN_INFO "TASKLET_WORKQUEUE : Work function executed on: %ld \n", after_after_delay5);
	printk(KERN_ALERT "TASKLET_WORKQUEUE : RUNTIME=  %ld \n", after_after_delay5 - before_delay5);
}
static DECLARE_DELAYED_WORK(our_work5, our_work_function5);
static void our_tasklet_function5(unsigned long data){
	tasklet_trylock(&our_tasklet5);
	strcpy(string_argument, data);
	after_delay5 = jiffies;
	i5 = smp_processor_id();
	queue_delayed_work(our_workqueue5, &our_work5, delay);
	printk(KERN_INFO "TASKLET_WORKQUEUE : Tasklet function of %s is running on CPU %d \n", string_argument, i5);
	printk(KERN_INFO "TASKLET_WORKQUEUE : Tasklet function scheduled on: %ld \n", before_delay5);
	printk(KERN_INFO "TASKLET_WORKQUEUE : Tasklet function executed on: %ld \n", after_delay5);
	tasklet_unlock(&our_tasklet5);
}

static void our_work_function4w(struct work_struct *our_work4w){
	after_after_delay4 = jiffies;
	printk(KERN_INFO "SHAREDQUEUE_WORKQUE: Work Initialization.\n");
	printk(KERN_INFO "SHAREDQUEUE_WORKQUE: Work function is running on CPU %d \n", smp_processor_id());
	printk(KERN_INFO "SHAREDQUEUE_WORKQUE: Work function scheduled on: %ld \n", before_delay4);
	printk(KERN_INFO "SHAREDQUEUE_WORKQUE: Work function executed on: %ld \n", after_after_delay4);
	printk(KERN_ALERT "SHAREDQUEUE_WORKQUE: RUNTIME=  %ld \n", after_after_delay4 - before_delay4);
}
static DECLARE_DELAYED_WORK(our_work4w, our_work_function4w);
static void our_work_function4(struct work_struct *our_work4){
	after_delay4 = jiffies;
	printk(KERN_INFO "SHAREDQUEUE_WORKQUE: sharedqueue Work Initialization.\n");
	printk(KERN_INFO "SHAREDQUEUE_WORKQUE: sharedqueue Work function is running on CPU %d \n", smp_processor_id());
	printk(KERN_INFO "SHAREDQUEUE_WORKQUE: sharedqueue Work function scheduled on: %ld \n", before_delay4);
	printk(KERN_INFO "SHAREDQUEUE_WORKQUE: sharedqueue Work function executed on: %ld \n", after_delay4);
	queue_delayed_work(our_workqueue4, &our_work4w, delay);
	printk(KERN_INFO "SHAREDQUEUE_WORKQUE: our work function and workqueue initiated on CPU %d \n", smp_processor_id());
}
static DECLARE_DELAYED_WORK(our_work4, our_work_function4);

static void our_work_function3s(struct work_struct *our_work3s){
	after_after_delay3 = jiffies;
	printk(KERN_INFO "WORKQUEUE_SHAREDQUE: Shared queue Work function is running on CPU %d \n", smp_processor_id());
	printk(KERN_INFO "WORKQUEUE_SHAREDQUE: Shared queue Work function scheduled on: %ld \n", after_delay3);
	printk(KERN_INFO "WORKQUEUE_SHAREDQUE: Shared queue Work function executed on: %ld \n", after_after_delay3);
	printk(KERN_ALERT "WORKQUEUE_SHAREDQUE: RUNTIME=  %ld \n", after_after_delay3 - before_delay3);
}

static void our_tasklet_function(unsigned long data){
	tasklet_trylock(&our_tasklet);
	strcpy(string_argument, data);
	after_after_delay = jiffies;
	i = smp_processor_id();
	printk(KERN_INFO "WORKQUE_TASKLET: Tasklet function of %s is running on CPU %d \n", string_argument, i);
	printk(KERN_INFO "WORKQUE_TASKLET: Tasklet function scheduled on: %ld \n", after_delay);
	printk(KERN_INFO "WORKQUE_TASKLET: Tasklet function executed on: %ld \n", after_after_delay);
	printk(KERN_ALERT "WORKQUE_TASKLET: RUNTIME=  %ld \n", after_after_delay - before_delay);
	tasklet_unlock(&our_tasklet);
}

static void our_tasklet_function2(unsigned long data){
	tasklet_trylock(&our_tasklet2);
	strcpy(string_argument, data);
	after_after_delay2 = jiffies;
	i2 = smp_processor_id();
	printk(KERN_INFO "SHAREDQUE_TASKLET: Tasklet function of %s is running on CPU %d \n", string_argument, i2);
	printk(KERN_INFO "SHAREDQUE_TASKLET: Tasklet function scheduled on: %ld \n", after_delay2);
	printk(KERN_INFO "SHAREDQUE_TASKLET: Tasklet function executed on: %ld \n", after_after_delay2);
	printk(KERN_ALERT "SHAREDQUE_TASKLET: RUNTIME=  %ld \n", after_after_delay2 - before_delay2);
	tasklet_unlock(&our_tasklet2);
}
static void our_tasklet_function6(unsigned long data){
	tasklet_trylock(&our_tasklet6);
	strcpy(string_argument, data);
	after_delay6 = jiffies;
	i6 = smp_processor_id();
	schedule_delayed_work(&our_work6,delay);
	printk(KERN_INFO "TASKLET_SHAREDQUEUE : Tasklet function of %s is running on CPU %d \n", string_argument, i6);
	printk(KERN_INFO "TASKLET_SHAREDQUEUE : Tasklet function scheduled on: %ld \n", before_delay6);
	printk(KERN_INFO "TASKLET_SHAREDQUEUE : Tasklet function executed on: %ld \n", after_delay6);
	tasklet_unlock(&our_tasklet6);
}

static void our_work_function(struct work_struct *our_work){
	after_delay = jiffies;
	printk(KERN_INFO "WORKQUE_TASKLET: Work Initialization.\n");
	j = smp_processor_id();
	tasklet_init(&our_tasklet, &our_tasklet_function, (unsigned long) &our_tasklet_argument);
	printk(KERN_INFO "WORKQUE_TASKLET: Tasklet initiated on CPU %d \n", j);
	tasklet_schedule(&our_tasklet);
	printk(KERN_INFO "WORKQUE_TASKLET: Work function is running on CPU %d \n", smp_processor_id());
	printk(KERN_INFO "WORKQUE_TASKLET: Work function scheduled on: %ld \n", before_delay);
	printk(KERN_INFO "WORKQUE_TASKLET: Work function executed on: %ld \n", after_delay);
}

static void our_work_function2(struct work_struct *our_work2){
	after_delay2 = jiffies;
	printk(KERN_INFO "SHAREDQUE_TASKLET: Work Initialization.\n");
	j2 = smp_processor_id();
	tasklet_init(&our_tasklet2, &our_tasklet_function2, (unsigned long) &our_tasklet_argument);
	printk(KERN_INFO "SHAREDQUE_TASKLET: Tasklet initiated on CPU %d \n", j2);
	tasklet_schedule(&our_tasklet2);
	printk(KERN_INFO "SHAREDQUE_TASKLET: Work function is running on CPU %d \n", smp_processor_id());
	printk(KERN_INFO "SHAREDQUE_TASKLET: Work function scheduled on: %ld \n", before_delay2);
	printk(KERN_INFO "SHAREDQUE_TASKLET: Work function executed on: %ld \n", after_delay2);
}

static DECLARE_DELAYED_WORK(our_work3s, our_work_function3s);

static void our_work_function3(struct work_struct *our_work3){
	after_delay3 = jiffies;
	schedule_delayed_work(&our_work3s,delay);
	printk(KERN_INFO "WORKQUEUE_SHAREDQUE: Work function is running on CPU %d \n", smp_processor_id());
	printk(KERN_INFO "WORKQUEUE_SHAREDQUE: Work function scheduled on: %ld \n", before_delay3);
	printk(KERN_INFO "WORKQUEUE_SHAREDQUE: Work function executed on: %ld \n", after_delay3);
}

static DECLARE_DELAYED_WORK(our_work, our_work_function);
static DECLARE_DELAYED_WORK(our_work2, our_work_function2);
static DECLARE_DELAYED_WORK(our_work3, our_work_function3);



static int __init Module4_init(void){
	printk(KERN_INFO "MODULE4: Initialization.\n");
	before_delay = jiffies;
	our_workqueue = create_singlethread_workqueue("ourqueue");
	if(!our_workqueue){
		printk(KERN_ALERT "WORKQUE_TASKLET: Creating the workqueue has been failed!\n");
		return -EFAULT;
	}
	printk(KERN_INFO "WORKQUE_TASKLET: Workqueue has been created\n");
	queue_delayed_work(our_workqueue, &our_work, delay);
/////////////////////////////////////////////////
	printk(KERN_INFO "SHAREDQUE_TASKLET: our work function and workqueue initiated on CPU %d \n", smp_processor_id());
	before_delay2 = jiffies;
	printk(KERN_INFO "SHAREDQUE_TASKLET: Workqueue2 has been created\n");
	schedule_delayed_work(&our_work2,delay);
	printk(KERN_INFO "SHAREDQUE_TASKLET: our work function2 and workqueue2 initiated on CPU %d \n", smp_processor_id());
////////////////////////////////////////////////
	before_delay3 = jiffies;
	our_workqueue3 = create_singlethread_workqueue("ourqueue3");
	if(!our_workqueue3){
		printk(KERN_ALERT "WORKQUEUE_SHAREDQUE: Creating the workqueue has been failed!\n");
		return -EFAULT;
	}
	printk(KERN_INFO "WORKQUEUE_SHAREDQUE: Workqueue has been created\n");
	queue_delayed_work(our_workqueue3, &our_work3, delay);
	printk(KERN_INFO "WORKQUEUE_SHAREDQUE: our work function and workqueue initiated on CPU %d \n", smp_processor_id());
///////////////////////////////////////////////
	our_workqueue4 = create_singlethread_workqueue("ourqueue4");
	if(!our_workqueue4){
		printk(KERN_ALERT "SHAREDQUEUE_WORKQUE: Creating the workqueue has been failed!\n");
		return -EFAULT;
	}
	printk(KERN_INFO "SHAREDQUEUE_WORKQUE: Workqueue has been created\n");
	before_delay4 = jiffies;
	schedule_delayed_work(&our_work4,delay);
///////////////////////////////////////////////	
	j5 = smp_processor_id();
	tasklet_init(&our_tasklet5, &our_tasklet_function5, (unsigned long) &our_tasklet_argument);
	printk(KERN_INFO "TASKLET_WORKQUEUE: Tasklet initiated on CPU %d \n", j5);
	our_workqueue5 = create_singlethread_workqueue("ourqueue5");
	if(!our_workqueue5){
		printk(KERN_ALERT "TASKLET_WORKQUEUE: Creating the workqueue has been failed!\n");
		return -EFAULT;
	}
	printk(KERN_INFO "TASKLET_WORKQUEUE: Workqueue has been created\n");
	before_delay5 = jiffies;
	tasklet_schedule(&our_tasklet5);
////////////////////////////////////////////
	j6 = smp_processor_id();
	tasklet_init(&our_tasklet6, &our_tasklet_function6, (unsigned long) &our_tasklet_argument);
	printk(KERN_INFO "TASKLET_SHAREDQUEUE : Tasklet initiated on CPU %d \n", j6);
	printk(KERN_INFO "TASKLET_SHAREDQUEUE : Initialization.\n");
	before_delay6 = jiffies;
	tasklet_schedule(&our_tasklet6);


	
	return SUCCESS;
}

static void __exit Module4_exit(void){
	cancel_delayed_work(&our_work);
	if(our_workqueue)
		destroy_workqueue(our_workqueue);
	printk(KERN_INFO "WORKQUEUESAMPLE: WorkQueue GoodBye.\n");	
	tasklet_disable_nosync(&our_tasklet);
	tasklet_kill(&our_tasklet);
	printk(KERN_INFO "WORKQUE_TASKLET: Tasklet GoodBye.\n");
//////////////////////////////////////////
	cancel_delayed_work(&our_work2);
	tasklet_disable_nosync(&our_tasklet2);
	tasklet_kill(&our_tasklet2);
	printk(KERN_INFO "SHAREDQUE_TASKLET: Tasklet GoodBye.\n");
	printk(KERN_INFO "SHAREDQUEUESAMPLE: WorkQueue GoodBye.\n");
//////////////////////////////////////////
	cancel_delayed_work(&our_work3);
	printk(KERN_INFO "WORKQUEUE_SHAREDQUE: Tasklet GoodBye.\n");
	cancel_delayed_work(&our_work3s);
	if(our_workqueue3)
		destroy_workqueue(our_workqueue3);
	printk(KERN_INFO "WORKQUEUE_SHAREDQUE: WorkQueue GoodBye.\n");
/////////////////////////////////////////
	cancel_delayed_work(&our_work4);
	printk(KERN_INFO "SHAREDQUEUE_WORKQUE: SharedQueue GoodBye.\n");
	cancel_delayed_work(&our_work4w);
	if(our_workqueue4)
		destroy_workqueue(our_workqueue4);
	printk(KERN_INFO "SHAREDQUEUE_WORKQUE: WorkQueue GoodBye.\n");
/////////////////////////////////////////
	cancel_delayed_work(&our_work5);
	if(our_workqueue5)
		destroy_workqueue(our_workqueue5);
	printk(KERN_INFO "TASKLET_WORKQUEUE : WorkQueue GoodBye.\n");
	tasklet_disable_nosync(&our_tasklet5);
	tasklet_kill(&our_tasklet5);	
	printk(KERN_INFO "TASKLET_WORKQUEUE : Tasklet GoodBye.\n");
////////////////////////////////////////
	cancel_delayed_work(&our_work6);
	printk(KERN_INFO "TASKLET_SHAREDQUEUE :Sharedqueue GoodBye.\n");
	tasklet_disable_nosync(&our_tasklet6);
	tasklet_kill(&our_tasklet6);
	printk(KERN_INFO "TASKLET_SHAREDQUEUE :Tasklet GoodBye.\n");
	
	printk(KERN_INFO "MODULE4: GoodBye.\n");
}
module_init(Module4_init);
module_exit(Module4_exit);
MODULE_LICENSE("GPL");
