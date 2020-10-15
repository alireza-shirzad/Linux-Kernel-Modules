#include<linux/module.h>
#include<linux/init.h>
#include<linux/proc_fs.h>
#include<linux/sched.h>
#include<linux/uaccess.h>
#include<linux/fs.h>
#include<linux/seq_file.h>
#include<linux/slab.h>

static char *str1 = NULL;
static char *str2 = NULL;
static struct proc_dir_entry *parent;
static struct proc_dir_entry *entry1;
static struct proc_dir_entry *entry2;

static int my_proc_show1(struct seq_file *m,void *v){
	seq_printf(m,"%s\n",str2);
	return 0;
}
static int my_proc_show2(struct seq_file *m,void *v){
	seq_printf(m,"%s\n",str1);
	return 0;
}

static ssize_t my_proc_write1(struct file* file,const char __user *buffer,size_t count,loff_t *f_pos){
	char *tmp = kzalloc((count+1),GFP_KERNEL);
	if(!tmp)return -ENOMEM;
	if(copy_from_user(tmp,buffer,count)){
		kfree(tmp);
		return EFAULT;
	}
	kfree(str1);
	str1=tmp;
	return count;
}
static ssize_t my_proc_write2(struct file* file,const char __user *buffer,size_t count,loff_t *f_pos){
	char *tmp = kzalloc((count+1),GFP_KERNEL);
	if(!tmp)return -ENOMEM;
	if(copy_from_user(tmp,buffer,count)){
		kfree(tmp);
		return EFAULT;
	}
	kfree(str2);
	str2=tmp;
	return count;
}

static int my_proc_open1(struct inode *inode,struct file *file){
	return single_open(file,my_proc_show1,NULL);
}
static int my_proc_open2(struct inode *inode,struct file *file){
	return single_open(file,my_proc_show2,NULL);
}

static struct file_operations my_fops1={
	.owner = THIS_MODULE,
	.open = my_proc_open1,
	.release = single_release,
	.read = seq_read,
	.llseek = seq_lseek,
	.write = my_proc_write1
};
static struct file_operations my_fops2={
	.owner = THIS_MODULE,
	.open = my_proc_open2,
	.release = single_release,
	.read = seq_read,
	.llseek = seq_lseek,
	.write = my_proc_write2
};

static int __init hello_init(void){
	parent = proc_mkdir("proc_folder",NULL);
	entry1 = proc_create("proc1",0777,parent,&my_fops1);
	entry2 = proc_create("proc2",0777,parent,&my_fops2);
	if(!entry1 || !entry2){
		return -1;	
	}else{
		printk(KERN_INFO "Module2: Proc directory created succesfully!\n");
	}
	return 0;
}

static void __exit hello_exit(void){
	remove_proc_entry("proc1",parent);
	remove_proc_entry("proc2",parent);
	remove_proc_entry("proc_folder",NULL);
	printk(KERN_INFO "Module2: Proc directory removed succesfully!\n");
}

module_init(hello_init);
module_exit(hello_exit);
