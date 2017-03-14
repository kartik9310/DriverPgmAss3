#include <linux/module.h> /* For Device driver module Init and Exit */
#include <linux/kernel.h> /* For printk() function to print on the kernel */
#include <linux/init.h> /*  Module init */
#include <linux/slab.h> /* For kmalloc() function */
#include <linux/errno.h> /* For Returing Error codes Bash Errors */
#include <linux/types.h> /* For size_t datatype used for read and write methods return data type */
#include <linux/fs.h> /* For all the file operations going to be performed */
#include <linux/proc_fs.h> /* For File system structures implementation */
#include <asm/uaccess.h> /*copy_from_user and copy_to_user methods usage */

#define MAJOR_NUMBER 72 /* Device number to be used for registration */

/* Forward Declaration */

int onebyte_open(struct inode *inode, struct file *filep); //one byte memory allocate open

int onebyte_release(struct inode *inode, struct file *filep); //one byte memory allocate close - deallocate

ssize_t onebyte_read(struct file *filep,char *buf,size_t count,loff_t *f_pos);//reading from the device and storing to buffer

ssize_t onebyte_write(struct file *filep,const char *buf,size_t count,loff_t *f_pos);//writing one byte from the buffer to the device file

static void onebyte_exit(void); //device module exit method

static int onebyte_init(void);// device module initialization method

/* Definition of File Operation Structure */

struct file_operations onebyte_fops = {
	read: onebyte_read,
	write: onebyte_write,
	open: onebyte_open,
	release: onebyte_release
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

	onebyte_data = kmalloc(sizeof(char),GFP_KERNEL);
	
	if(!onebyte_data)
	{
		onebyte_exit();
		return -ENOMEM;
	}
	//memset(onebyte_data,0,1);
	*onebyte_data = 'X';
	printk(KERN_ALERT "This is a One Byte device Module\n");
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
	printk(KERN_ALERT "One Byte Device Module is unloaded\n");
}

//Reading the device file

ssize_t onebyte_read(struct file *filep,char *buf,size_t count,loff_t *f_pos)
{
	
	char *tmp;
	tmp=onebyte_data;
	
	//printk(KERN_INFO "%d",sizeof(onebyte_data));


	if(*f_pos == 0){
		copy_to_user(buf,tmp,1); //Transferring the data to the User Space
		*f_pos += 1;
		return 1;
	}
	else
		return 0; //End of the file so return 0
	
}

//Writing to a device file

ssize_t onebyte_write(struct file *filep,const char *buf,size_t count,loff_t *f_pos)
{
	char *tmp;
	if(count == 1){
	tmp = buf+count-1;
	printk(KERN_INFO "%d",count); // For knowing the number of bytes in the buffer in dmesg command
	copy_from_user(onebyte_data,tmp,1); // Transfering the data from user space to kernel space
	return 1;//Number of bytes written on the device returned as a constant
	}
	else{
	tmp = buf + count - count;
	printk(KERN_INFO "%d",count);
	copy_from_user(onebyte_data,tmp,1);
	return -ENOSPC;}// Returns the bash error to kernel that the buffer has more number of characters than expected. More than 1 (one byte device). Hence, Returns the bash Error No space left on the device.
}

MODULE_LICENSE("GPL");
module_init(onebyte_init); //Module Initialization method called
module_exit(onebyte_exit); //Module Exit method called
