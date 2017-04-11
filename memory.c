#include <linux/module.h> /* For Device driver module Init and Exit */
#include <linux/kernel.h> /* For printk() function to print on the kernel */
#include <linux/init.h> /*  Module init */
#include <linux/slab.h> /* For kmalloc() function */
#include <linux/errno.h> /* For Returing Error codes Bash Errors */
#include <linux/types.h> /* For size_t datatype used for read and write methods return data type */
#include <linux/fs.h> /* For all the file operations going to be performed */
#include <linux/proc_fs.h> /* For File system structures implementation */
#include <asm/uaccess.h> /*copy_from_user and copy_to_user methods usage */
#include <linux/ioctl.h>
#define SCULL_IOC_MAGIC 'k'
#define SCULL_HELLO_IO(SCULL_IOC_MAGIC,1);

#define MAJOR_NUMBER 72 /* Device number to be used for registration */
#define MAX_LENGTH 13

/* Forward Declaration */

int onebyte_open(struct inode *inode, struct file *filep); //one byte memory allocate open

int onebyte_release(struct inode *inode, struct file *filep); //one byte memory allocate close - deallocate

ssize_t onebyte_read(struct file *filep,char *buf,size_t count,loff_t *f_pos);//reading from the device and storing to buffer

ssize_t onebyte_write(struct file *filep,const char *buf,size_t count,loff_t *f_pos);//writing one byte from the buffer to the device file

static void onebyte_exit(void); //device module exit method

static int onebyte_init(void);// device module initialization method

static loff_t onebyte_lseek(struct file *file,loff_t offset,int origin);

/* Definition of File Operation Structure */

struct file_operations onebyte_fops = {
	read: onebyte_read,
	write: onebyte_write,
	open: onebyte_open,
	release: onebyte_release,
	llseek: onebyte_lseek
};

char *onebyte_data = NULL; //Buffer to store the data

//Opening the device as a File

int onebyte_open(struct inode *inode, struct file *filep)
{
	return 0; //always successful
}

//closing the device as a file

int onebyte_release(struct inode *inode, struct file *filep)
{
	return 0; //always successful
}


// Device Registration using the Device Major number and File structure defined above

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

	onebyte_data = kmalloc(MAX_LENGTH,GFP_KERNEL);
	
	if(!onebyte_data)
	{
		onebyte_exit();
		return -ENOMEM;
	}
	//memset(onebyte_data,0,1);
	*onebyte_data = 'X';
	printk(KERN_ALERT "This is a 4 MegaByte device Module\n");
	return 0;
}

//Freeing the buffer space and unregistering the device once the module is unloaded from kernel

static void onebyte_exit(void)
{
	if(onebyte_data)
	{
		kfree(onebyte_data);
		onebyte_data = NULL;
	}
	unregister_chrdev(MAJOR_NUMBER,"onebyte");
	printk(KERN_ALERT "4 MegaByte Device Module is unloaded\n");
}

//Reading the device file

ssize_t onebyte_read(struct file *filep,char *buf,size_t count,loff_t *f_pos)
{
	
	int max_bytes;
	int bytes_to_do;
	int nbytes;
	
		
	max_bytes = MAX_LENGTH-*f_pos;
	
	if(max_bytes >= count) bytes_to_do = count;
	else bytes_to_do = max_bytes;

	if(bytes_to_do == 0)
	{
		//printk("reached End of Device");
		return 0;
	}
	nbytes = bytes_to_do - copy_to_user(buf,onebyte_data+*f_pos,bytes_to_do);
	*f_pos+=nbytes;
	return nbytes;	
		

}

//Writing to a device file

ssize_t onebyte_write(struct file *filep,const char *buf,size_t count,loff_t *f_pos)
{



	int nbytes;
	int bytes_to_do;
	int max_bytes;
	
		
	max_bytes = MAX_LENGTH-*f_pos;
	if(max_bytes > count) bytes_to_do = count;
	else bytes_to_do = max_bytes;
	
	if(bytes_to_do == 0)
	{
		//printk("reached End of Device");
		return -ENOSPC;
	}
	
	nbytes = bytes_to_do - copy_from_user(onebyte_data+*f_pos,buf,bytes_to_do);
	*f_pos+=nbytes;
	printk("\nNumber of bytes written:%d",nbytes);
	return nbytes;
	
		
}

static loff_t onebyte_lseek(struct file *file,loff_t offset,int origin)
{
	loff_t new_pos = 0;
	printk("\n lseek system call working");
	switch(origin)
	{
		case 0: new_pos = offset;
			break;
		case 1: new_pos = file->f_pos + offset;
			break;
		case 2: new_pos = MAX_LENGTH - offset;
			break;
	}
	if(new_pos > MAX_LENGTH) new_pos = MAX_LENGTH;
	if(new_pos < 0)	new_pos = 0;
	file->f_pos = new_pos;
	return new_pos;
}


MODULE_LICENSE("GPL");
module_init(onebyte_init); //Module Initialization method called
module_exit(onebyte_exit); //Module Exit method called
