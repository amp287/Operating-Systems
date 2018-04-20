//Austin Peace Cray Winfrey Nick Macri
//Group 17
#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/string.h>
#include <linux/mutex.h>

#define DEVICE_NAME "chardev_in"
#define CLASS_NAME "chardev_in_class"
#define SIZE 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Austin Peace");
MODULE_DESCRIPTION("Assignment 3 Char device input");
MODULE_VERSION("1.0");

static int    major_number;
static struct class*  chardevClass  = NULL; ///< The device-driver class struct pointer

int char_dev_size_of_message;
char char_dev_message[SIZE];
struct mutex char_dev_mutex;

EXPORT_SYMBOL(char_dev_size_of_message);
EXPORT_SYMBOL(char_dev_message);
EXPORT_SYMBOL(char_dev_mutex);

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t*);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static int __init chardev_init(void){
  printk(KERN_INFO "char dev in: Initializing\n");

  char_dev_size_of_message = 0;
  char_dev_message[0] = 0;
  major_number = register_chrdev(0, DEVICE_NAME, &fops);

  if(major_number < 0){
      printk(KERN_ALERT "char dev in: Failed to register a major number\n");
  }

  printk(KERN_INFO "char dev in: Registerd with major number %d", major_number);

  // Register the device class
  chardevClass = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(chardevClass)){                // Check for error and clean up if there is
     unregister_chrdev(major_number, DEVICE_NAME);
     printk(KERN_ALERT "char dev in: Failed to register device class\n");
     return PTR_ERR(chardevClass);          // Correct way to return an error on a pointer
  }
  printk(KERN_INFO "char dev in: device class registered correctly\n");

   mutex_init(&char_dev_mutex);

   return 0;
}

static void __exit chardev_exit(void){
   device_destroy(chardevClass, MKDEV(major_number, 0));     // remove the device
   class_unregister(chardevClass);                          // unregister the device class
   class_destroy(chardevClass);                             // remove the device class
   unregister_chrdev(major_number, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "char dev in: Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "char dev in: Device has been opened\n");
   return 0;
}
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
  return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
  char *keyword = "UCF";
  char *replace = "Undefeated 2018 National Champions UCF";
  int i;
  int r_len = strlen(replace);
  int len_added = 0;
  
  mutex_lock(&char_dev_mutex);

  for(i = 0; i < len; i++){
	  if(char_dev_size_of_message >= SIZE)
		  break;
	  
	  if(i + 2 < len && strncmp(&buffer[i], keyword, 3) == 0){
		  if(char_dev_size_of_message + r_len < SIZE){
			memcpy(&char_dev_message[char_dev_size_of_message], replace, r_len);
			char_dev_size_of_message += r_len;
			len_added += r_len;
		  } else {
			  memcpy(&char_dev_message[char_dev_size_of_message], replace, SIZE - r_len);
			  char_dev_size_of_message += SIZE - r_len;
			  len_added += SIZE - r_len;
		  }
		  i += 2;
	  } else {
		  char_dev_message[char_dev_size_of_message++] = buffer[i];
		  len_added += 1;
	  }
  }

  printk(KERN_INFO "char dev in: Received %zu characters from the user\n", len_added);
  mutex_unlock(&char_dev_mutex);
  return len_added;
}

static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "char dev in: Device successfully closed\n");
   return 0;
}

module_init(chardev_init);
module_exit(chardev_exit);
