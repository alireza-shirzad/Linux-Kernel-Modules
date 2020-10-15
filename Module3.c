#include<linux/module.h>
#include <linux/kernel.h>
#include<linux/init.h>
#include<linux/proc_fs.h>
#include<linux/sched.h>
#include<linux/uaccess.h>
#include<linux/fs.h>
#include<linux/seq_file.h>
#include<linux/slab.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include <asm/ioctl.h>
#include <linux/utsname.h>

#define SUCCESS 0
#define ProcName1 "plog"
#define ProcName2 "pfifo"
#define ProcName3 "pstack"
#define MODULE_NAME "Module3"
#define DEVICE_NAME "dmod"
#define BUF_LEN 4096
#define MAX_BUF_LEN 1024
#define MESSAGE_LEN 32

#define MAGIC 'T'
#define IOC_MAXNR 3
#define IOCTL_RESET _IOR(MAGIC, 0, char)
#define IOCTL_STACKRESET _IOR(MAGIC, 1, char)
#define IOCTL_FIFORESET _IOR(MAGIC, 2, char)

typedef char Message[MESSAGE_LEN] ;

static struct kobject *our_kobj;
static int sstack_write = 1;
static int sfifo_write = 0;
static int sstack_read = 1;
static int sfifo_read = 0;
static Message dev_stack[MAX_BUF_LEN];
static Message dev_list[MAX_BUF_LEN];
static unsigned long dev_buffer_size = MESSAGE_LEN , i = 0;
static int major, Device_Open = 0;
static int sp = -1;
static int lp = 0;
static int fp = 0;
static int ep = -1;
static int Stack_Counter = 0;
static int Fifo_Counter = 0;

long proc_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	static int err = 0, retval = 0;
	if(_IOC_TYPE(cmd) != MAGIC || _IOC_NR(cmd) > IOC_MAXNR)
		return -ENOTTY;
		
	
	switch(cmd){
		case IOCTL_RESET:
			sp = -1;
			lp = 0;
			fp = 0;
			ep = -1;
			Stack_Counter = 0;
			Fifo_Counter = 0;
			break;
		case IOCTL_STACKRESET:
			sp = -1;
			lp = 0;
			Stack_Counter = 0;
			break;
		case IOCTL_FIFORESET:
			lp = 0;
			fp = 0;
			ep = -1;
			Fifo_Counter = 0;
			break;
		default:
			printk(KERN_ALERT "IOCTLPROCFS: Invalid IOCTL Command!\n");
			return -ENOTTY;
	}
	return retval;
}


static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
	printk(KERN_INFO "HELLOWORLDSYSFS: Show Function, %s Attribute,  Process \"%s:%i\"\n",  attr->attr.name, current->comm, current->pid);

	//We have to decide on which attribute show wich content to the userspace
	if(strcmp(attr->attr.name, "sstack") == 0)
		return sprintf(buf, "%d %d\n", sstack_write, sstack_read);
	else if (strcmp(attr->attr.name, "sfifo") == 0)
		return sprintf(buf, "%d %d\n", sfifo_write, sfifo_read);
	else
		printk(KERN_INFO  "HELLOWORLDSYSFS: I don't know what you are doing, but it seems you are not doing it right!\n");
	return NULL;
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
	printk(KERN_INFO "HELLOWORLDSYSFS: Store Function, %s Attribute,  Process \"%s:%i\"\n",  attr->attr.name, current->comm, current->pid);
	if(strcmp(attr->attr.name, "sstack") == 0)
		sscanf(buf, "%d %d\n", &sstack_write, &sstack_read);
	else if (strcmp(attr->attr.name, "sfifo") == 0)
		sscanf(buf, "%d %d\n", &sfifo_write, &sfifo_read);
	else
	printk(KERN_ALERT "HELLOWORLDSYSFS: You have no Permission to write in %s attribute.\n", attr->attr.name);
	return count;
}


static struct kobj_attribute sfifo_attribute = __ATTR(sfifo, 0664, sysfs_show, sysfs_store);
static struct kobj_attribute sstack_attribute = __ATTR(sstack, 0664, sysfs_show, sysfs_store);

static struct attribute *attrs[] = {
	&sfifo_attribute.attr,
	&sstack_attribute.attr,
	NULL,	//To terminate the list of attributes
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};


static int my_proc_show1(struct seq_file *m,void *v){
	printk(KERN_INFO "Module3: Proc file 'plog' has been shown!\n");
	seq_printf(m,"Number of Items in Stack = %d\nNumber of Items in Fifo = %d\n\n\n",Stack_Counter,Fifo_Counter);
	seq_printf(m, "IOCTL_RESET, %lu\n", IOCTL_RESET);
	seq_printf(m, "IOCTL_STACKRESET, %lu\n", IOCTL_STACKRESET);
	seq_printf(m, "IOCTL_FIFORESET, %lu\n", IOCTL_FIFORESET);
	return 0;
}


static int my_proc_open1(struct inode *inode,struct file *file){
	return single_open(file,my_proc_show1,NULL);
	printk(KERN_INFO "Module3: Proc file 'plog' has been opened!\n");
}


static int my_proc_show2(struct seq_file *m,void *v){
	printk(KERN_INFO "Module3: Proc file 'pfifo' has been shown!\n");
	for (i=fp;i<ep+1;i++){
		seq_printf(m,"%s\n",dev_list[i]);
		}
	return 0;
}


static int my_proc_open2(struct inode *inode,struct file *file){
	return single_open(file,my_proc_show2,NULL);
	printk(KERN_INFO "Module3: Proc file 'pfifo' has been opened!\n");
}


static int my_proc_show3(struct seq_file *m,void *v){
	printk(KERN_INFO "Module3: Proc file 'pstack' has been shown!\n");
	for (i=0;i<sp+1;i++){
		seq_printf(m,"%s\n",dev_stack[i]);
		}
	return 0;
}

static int my_proc_open3(struct inode *inode,struct file *file){
	return single_open(file,my_proc_show3,NULL);
	printk(KERN_INFO "Module3: Proc file 'pstack' has been opened!\n");
}
int device_open(struct inode *inode, struct file *file){
	printk(KERN_INFO "Module3: Open Function, Process \"%s:%i\"\n", current->comm, current->pid);
	if (Device_Open)
		return -EBUSY;
	Device_Open++;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset){
	if(sstack_read==1 && sfifo_read==0){
		lp = sp ;
		if(lp<0)
			lp = 0;
	static int ret = 0;
	printk(KERN_INFO "Module3: Read Function, Process \"%s:%i\"\n", current->comm, current->pid);
	if(ret){
		sp--;
		Stack_Counter = sp+1;
		printk(KERN_INFO "Module3: /dev entry read has END\n");
		ret = 0;
		}
	else{
		if(raw_copy_to_user(buffer, dev_stack[lp], dev_buffer_size))
			return -EFAULT;

		printk(KERN_INFO "Module3: %lu bytes has read from /dev entry\n", dev_buffer_size);
		ret = dev_buffer_size;
	}

	return ret;
	}
	
	if(sfifo_read==1 && sstack_read==0){
		if(fp>ep)
			fp = ep;
		lp = fp;
		static int ret = 0;
		printk(KERN_INFO "Module3: Read Function, Process \"%s:%i\"\n", current->comm, current->pid);
		if(ret){
			fp++;
			Fifo_Counter = ep-fp+1;
			printk(KERN_INFO "Module3: /dev entry read has END\n");
			ret = 0;
		}
		else{
			if(raw_copy_to_user(buffer, dev_list[lp], dev_buffer_size))
				return -EFAULT;

			printk(KERN_INFO "Module3: %lu bytes has read from /dev entry\n", dev_buffer_size);
			ret = dev_buffer_size;
		}

		return ret;
	}
	
	
	
}

static ssize_t device_write(struct file *file, const char *buffer, size_t length, loff_t * off){
	if(sstack_write==1){
		if(sp<-1)
			sp = -1;
		printk(KERN_INFO "Module3: Stack Write Function, Process \"%s:%i\"\n", current->comm, current->pid);
		if (length > MAX_BUF_LEN)
			dev_buffer_size = MAX_BUF_LEN;
		else
			dev_buffer_size = length;
		if(raw_copy_from_user(dev_stack[++sp], buffer, dev_buffer_size))
			return -EFAULT;
		Stack_Counter = sp+1;
		printk(KERN_INFO "Module3: %lu bytes has wrote to /dev entry\n", dev_buffer_size);
		}
	if(sfifo_write==1){
		printk(KERN_INFO "Module3: Stack Write Function, Process \"%s:%i\"\n", current->comm, current->pid);
		if (length > MAX_BUF_LEN)
			dev_buffer_size = MAX_BUF_LEN;
		else
			dev_buffer_size = length;
		if(raw_copy_from_user(dev_list[++ep], buffer, dev_buffer_size))
			return -EFAULT;
		printk(KERN_INFO "Module3: %lu bytes has wrote to /dev entry\n", dev_buffer_size);
		Fifo_Counter = ep+1 ;
		}
		
		return dev_buffer_size;
}

int device_release(struct inode *inode, struct file *file){
	printk(KERN_INFO "Module3: Release Function, Process \"%s:%i\"\n", current->comm, current->pid);
	Device_Open--;
	module_put(THIS_MODULE);
	return SUCCESS;
}



static struct file_operations my_fops1={
	.owner = THIS_MODULE,
	.open = my_proc_open1,
	.release = single_release,
	.read = seq_read,
	.llseek = seq_lseek,
	.unlocked_ioctl = proc_ioctl,
};

static struct file_operations my_fops2={
	.owner = THIS_MODULE,
	.open = my_proc_open2,
	.release = single_release,
	.read = seq_read,
	.llseek = seq_lseek,
};

static struct file_operations my_fops3={
	.owner = THIS_MODULE,
	.open = my_proc_open3,
	.release = single_release,
	.read = seq_read,
	.llseek = seq_lseek,
};
static const struct file_operations devfops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,
};


static int __init kmodule_init(void){

	printk(KERN_INFO "Module3: %s Initialization.\n", DEVICE_NAME);
	printk(KERN_INFO "Module3: Init Module, Process \"%s:%i\"\n", current->comm, current->pid);
	
	major = register_chrdev(0, DEVICE_NAME, &devfops);
	if (major < 0) {
		printk(KERN_ALERT "Module3: Registration Filure, %d Major\n", major);
		return major;
		}
	printk(KERN_INFO "Module3: 'mknod /dev/%s c %d 0'\n", DEVICE_NAME, major);
	printk(KERN_INFO "Module3: 'chmod 777 /dev/%s'.\n", DEVICE_NAME);

	struct proc_dir_entry *entry1;
	entry1 = proc_create(ProcName1,0777,NULL,&my_fops1);
	if(!entry1){
		return -1;	
	}else{
		printk(KERN_INFO "Module3: Proc file 'plog' created successfully!\n");
	}


	struct proc_dir_entry *entry2;
	entry2 = proc_create(ProcName2,0777,NULL,&my_fops2);
	if(!entry2){
		return -1;	
	}else{
		printk(KERN_INFO "Module3: Proc file 'plog' created successfully!\n");
	}


	struct proc_dir_entry *entry3;
	entry3 = proc_create(ProcName3,0777,NULL,&my_fops3);
	if(!entry3){
		return -1;	
	}else{
		printk(KERN_INFO "Module3: Proc file 'plog' created successfully!\n");
	}
	
	our_kobj = kobject_create_and_add(MODULE_NAME, NULL);
	if (!our_kobj){
		printk(KERN_ALERT "Module3: KOBJECT Registration Failure.\n");
		//Because of this fact that sysfs is a ram filesystem, this error means the lack of enough memory
		return -ENOMEM;
	}
	int retval;
	retval = sysfs_create_group(our_kobj, &attr_group);
	if (retval){
		printk(KERN_ALERT "Module3: Creating attribute groupe has been failed.\n");
		//Because kobject creation actually increased the reference cont of it, we know in case of failure need to reduce it by one
		kobject_put(our_kobj);
	}
		printk(KERN_INFO "Module3: You can obtain the following information by reading files under /sys/kernel/%s\n", MODULE_NAME);
	
	
	return 0;
}

static void __exit kmodule_exit(void){
	remove_proc_entry(ProcName1,NULL);
	printk(KERN_INFO "Module3: Proc file 'plog' has been removed successfully!\n");
	remove_proc_entry(ProcName2,NULL);
	printk(KERN_INFO "Module3: Proc file 'pfifo' has been removed successfully!\n");
	remove_proc_entry(ProcName3,NULL);
	printk(KERN_INFO "Module3: Proc file 'pstack' has been removed successfully!\n");
	kobject_put(our_kobj);
	printk(KERN_INFO "Module3: /sys/kernel/%s and all its attributes has been removed.\n", MODULE_NAME);
	unregister_chrdev(major, DEVICE_NAME);
	printk(KERN_INFO "Module3: 'rm /dev/%s'\n", DEVICE_NAME);
}

module_init(kmodule_init);
module_exit(kmodule_exit);
MODULE_LICENSE("GPL");
