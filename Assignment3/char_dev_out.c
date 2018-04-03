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

#define DEVICE_NAME "chardev_out"
#define CLASS_NAME "chardev_out_class"
#define SIZE 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Austin Peace");
MODULE_DESCRIPTION("Assignment 3 Char device output");
MODULE_VERSION("1.0");

static int    major_number;
static struct class*  chardevClass  = NULL; ///< The device-driver class struct pointer

extern int char_dev_size_of_message;
extern char char_dev_message[SIZE];
extern struct mutex char_dev_mutex;

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
  printk(KERN_INFO "chardev: Initializing\n");

  major_number = register_chrdev(0, DEVICE_NAME, &fops);

  if(major_number < 0){
      printk(KERN_ALERT "chr dev out: Failed to register a major number\n");
  }

  printk(KERN_INFO "chr dev out: Registerd with major number %d", major_number);

  // Register the device class
  chardevClass = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(chardevClass)){                // Check for error and clean up if there is
     unregister_chrdev(major_number, DEVICE_NAME);
     printk(KERN_ALERT "char dev out: Failed to register device class\n");
     return PTR_ERR(chardevClass);          // Correct way to return an error on a pointer
  }
  printk(KERN_INFO "char dev out: device class registered correctly\n");

   return 0;
}

static void __exit chardev_exit(void){
   device_destroy(chardevClass, MKDEV(major_number, 0));     // remove the device
   class_unregister(chardevClass);                          // unregister the device class
   class_destroy(chardevClass);                             // remove the device class
   unregister_chrdev(major_number, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "char dev out: Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "char dev out: Device has been opened\n");
   return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
  return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0, i, amt_to_read = 0;
  mutex_lock(&char_dev_mutex);
   if(char_dev_size_of_message > len){
         error_count = copy_to_user(buffer, char_dev_message, len);
         amt_to_read = len - error_count;
   } else {
        error_count = copy_to_user(buffer, char_dev_message, char_dev_size_of_message);
        amt_to_read = char_dev_size_of_message - error_count;
  }
  printk(KERN_INFO "BUFFER: %c %c %c %c\n", char_dev_message[0], char_dev_message[1], char_dev_message[2], char_dev_message[3]);
  if(amt_to_read != 0 && error_count != len){
    for(i = amt_to_read; i < char_dev_size_of_message; i++){
      char_dev_message[i - amt_to_read] = char_dev_message[i];
    }
    char_dev_size_of_message -= amt_to_read;
  }

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "char dev out: Sent %d characters to the user\n", amt_to_read);
      mutex_unlock(&char_dev_mutex);
      return amt_to_read;  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "char dev out: Failed to send %d characters to the user\n", error_count);
      mutex_unlock(&char_dev_mutex);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "char dev out: Device successfully closed\n");
   return 0;
}

module_init(chardev_init);
module_exit(chardev_exit);
