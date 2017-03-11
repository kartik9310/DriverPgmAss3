#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define MAJOR_NUMBER 61

/* Forward Declaration */

int onebyte_open(struct inode *inode, struct file *filep); //one byte memory allocate open

int onebyte_release(struct inode *inode, struct file *filep); //one byte memory allocate close - deallocate

ssize_t onebyte_read(struct file *filep,char *buf,size_t count,loff_t *f_pos);//reading one byte from the file and storing to buffer

ssize_t onebyte_write(struct file *filep,const char *buf,size_t count,loff_t *f_pos);

static void onebyte_exit(void);

static int onebyte_init(void);

/* Definition of File Operation Structure */

struct file_operations onebyte_fops = {
	read: onebyte_read,
	write: onebyte_write,
	open: onebyte_open,
	release: onebyte_release
};

char *onebyte_data = NULL;

int onebyte_open(struct inode *inode, struct file *filep)
{
	return 0; //always successful
}

int onebyte_release(struct inode *inode, struct file *filep)
{
	return 0; //always successful
}

static int onebyte_init(void)
{
	int result;
	//Register the character Device
	result = register_chrdev(MAJOR_NUMBER,"onebyte",&onebyte_fops);

	if(result<0)
	{
		return result;
	}
	
	//Allocating one byte of memory using kmalloc and freeing using kfree

	onebyte_data = kmalloc(sizeof(char),GFP_KERNEL);
	
	if(!onebyte_data)
	{
		onebyte_exit();
		return -ENOMEM;
	}
	*onebyte_data = 'X';
	printk(KERN_ALERT "This is a One Byte device Module\n";
	return 0;
}


static void onebyte_exit(void)
{
	if(onebyte_data)
	{
		kfree(onebyte_data);
		onebyte_data = NULL;
	}
	unregister_chrdev(MAJOR_NUMBER,"onebyte");
	printk(KERN_ALERT "One Byte Device Module is unloaded\n";
}

MODULE_LICENSE("GPL");
module_init(onebyte_init);
module_exit(onebyte_exit);
