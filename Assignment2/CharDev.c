#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function

#define DEVICE_NAME "MARIO"
#define CLASS_NAME "ITSAME"
#define SIZE 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Austin Peace");
MODULE_DESCRIPTION("Assignment 2 Char device");
MODULE_VERSION("1.0");

static int    major_number;
static struct class*  chardevClass  = NULL; ///< The device-driver class struct pointer
static struct device* chardevDevice = NULL; ///< The device-driver device struct pointer

static int size_of_message;
static char   message[SIZE] = {0};

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
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

  size_of_message = 0;

  major_number = register_chrdev(0, DEVICE_NAME, &fops);

  if(major_number < 0){
      printk(KERN_ALERT "chrdev: Failed to register a major number\n");
  }

  printk(KERN_INFO "chrdev: Registerd with major number %d", major_number);

  // Register the device class
  chardevClass = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(chardevClass)){                // Check for error and clean up if there is
     unregister_chrdev(major_number, DEVICE_NAME);
     printk(KERN_ALERT "Failed to register device class\n");
     return PTR_ERR(chardevClass);          // Correct way to return an error on a pointer
  }
  printk(KERN_INFO "EBBChar: device class registered correctly\n");

  // Register the device driver
   chardevDevice = device_create(chardevClass, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
   if (IS_ERR(chardevDevice)){               // Clean up if there is an error
      class_destroy(chardevClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(major_number, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(chardevDevice);
   }
   printk(KERN_INFO "EBBChar: device class created correctly\n"); // Made it! device was initialized

   return 0;
}

static void __exit chardev_exit(void){
   device_destroy(chardevClass, MKDEV(major_number, 0));     // remove the device
   class_unregister(chardevClass);                          // unregister the device class
   class_destroy(chardevClass);                             // remove the device class
   unregister_chrdev(major_number, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "chardev: Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "chardev: Device has been opened\n");
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0, i, amt_to_read = 0;

   if(size_of_message > len){
         error_count = copy_to_user(buffer, message, len);
         amt_to_read = len - error_count;
   } else {
        error_count = copy_to_user(buffer, message, size_of_message);
        amt_to_read = size_of_message - error_count;
  }

  if(amt_to_read != 0){
    for(i = amt_to_read; i < size_of_message; i++){
      message[i - amt_to_read] = message[i];
    }
    size_of_message -= amt_to_read;
  }

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "chardev: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "chardev: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
  int i;

  if(len + size_of_message > SIZE)
    len = SIZE - size_of_message;

  snprintf(message + size_of_message, len, "%s", buffer[i]);

  size_of_message = size_of_message + len;                 // store the length of the stored message
  printk(KERN_INFO "chardev: Received %zu characters from the user\n", len);
  return len;
}

static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "chardev: Device successfully closed\n");
   return 0;
}

module_init(chardev_init);
module_exit(chardev_exit);
