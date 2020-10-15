#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cred.h>
#include <asm/uaccess.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <generated/utsrelease.h>
#include <linux/version.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#define SUCCESS 0
#define DEVICE_NAME "Module1"
#define BUF_LEN 4096
#define MAX_BUF_LEN 4096
#define Num 10

static char dev_buffer[MAX_BUF_LEN];
static unsigned long dev_buffer_size = 0, i = 0;
static int gmt_hour = 3;
static int gmt_minute = 30;
static char gmt_sign = '+';
static int major, Device_Open = 0;
static char msg[BUF_LEN], *msg_Ptr;
static int counter = 0;
int hour_gmt[Num], minute_gmt[Num], second_gmt[Num];
int hour_loc[Num], minute_loc[Num], second_loc[Num];
int byteNum[Num] ;
static int device_open(struct inode *inode, struct file *file){
	if (Device_Open)
		return -EBUSY;
	Device_Open++;
	sprintf(msg, "There is nothing to show!\n");

	msg_Ptr = msg;

	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset){

	int bytes_read = 0;

	if (*msg_Ptr == 0)
		return 0;

	while (length && *msg_Ptr) {
		put_user(*(msg_Ptr++), buffer++);

		length--;
		bytes_read++;
		}

	return bytes_read;
}


static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t * off){
	counter++;
	if (length > MAX_BUF_LEN)
		dev_buffer_size = MAX_BUF_LEN;
	else
		dev_buffer_size = length;
	if(raw_copy_from_user(dev_buffer, buffer, dev_buffer_size))
		return -EFAULT;
	struct timespec my_timeofday_gmt = current_kernel_time();
	struct timespec my_timeofday_loc;
	getnstimeofday(&my_timeofday_loc);
	byteNum[counter-1] = dev_buffer_size ;
	second_gmt[counter-1] = (int) my_timeofday_gmt.tv_sec;
	hour_gmt[counter-1] = (second_gmt[counter-1] / 3600) % 24;
	minute_gmt[counter-1] = (second_gmt[counter-1] / 60) % 60;
	second_gmt[counter-1] %= 60;

	second_loc[counter] = (int) my_timeofday_loc.tv_sec;
	hour_loc[counter-1] = ((second_loc[counter-1] / 3600) % 24 + gmt_hour) % 24;
	minute_loc[counter-1] = (second_loc[counter-1] / 60) % 60 + gmt_minute;
	if(minute_loc[counter-1]>=60){
		hour_loc[counter-1] = (hour_loc[counter-1] + 1) % 24;
		minute_loc[counter-1] %= 60;
		}
	second_loc[counter-1] %= 60;
	return dev_buffer_size;
}
static int device_release(struct inode *inode, struct file *file){
	Device_Open--;
	module_put(THIS_MODULE);
	return SUCCESS;
}


static struct proc_dir_entry* our_proc_file;
static int proc_show(struct seq_file *m, void *v){
	for(i=0 ; i<counter ; i++){
	seq_printf(m, "GMT %d:%d:%d___Local Time %d:%d:%d (GMT %c%d:%d)______NumberOfBytes=%d\n",hour_gmt[i], minute_gmt[i], second_gmt[i], hour_loc[i], minute_loc[i], second_loc[i], gmt_sign, gmt_hour, gmt_minute,byteNum[i]);
	}
	return SUCCESS;
}


static int proc_open(struct inode *inode, struct file *file){
	return single_open(file, proc_show, NULL);
}

static int module_permission(struct inode *inode, int operation){
	if(operation == 4 || (operation == 2 && get_current_user()->uid.val == 0))
		return 0;
	return -EACCES;
}

static struct file_operations fopsdev = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};


static const struct file_operations fopsproc = {
	.owner = THIS_MODULE,
	.open = proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static const struct inode_operations iops = {
	.permission = module_permission,
};

static int __init helloworld_chardev_init(void){

	major = register_chrdev(0, DEVICE_NAME, &fopsdev);
	if (major < 0) {
		return major;
		}

	printk(KERN_INFO "Module1: 'mknod /dev/%s c %d 0'\n", DEVICE_NAME, major);
	printk(KERN_INFO "Module2: 'chmod 777 /dev/%s'\n", DEVICE_NAME);

		#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 10)
		our_proc_file = create_proc_entry(DEVICE_NAME, 0644 , NULL);
		our_proc_file->proc_fops = &fopsproc;
		our_proc_file->proc_iops = &iops;
	#else

		our_proc_file = proc_create(DEVICE_NAME, 0644 , NULL, &fopsproc);

	#endif
	
	if(!our_proc_file){
		return -ENOMEM;
	}
	
	
	return SUCCESS;
}

static void __exit helloworld_chardev_exit(void){

	unregister_chrdev(major, DEVICE_NAME);

	remove_proc_entry(DEVICE_NAME, NULL);

}

module_init(helloworld_chardev_init);
module_exit(helloworld_chardev_exit);
