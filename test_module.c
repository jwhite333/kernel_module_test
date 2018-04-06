#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>           // File operations structure
#include <linux/cdev.h>         // Character driver (as opposed to block driver)
#include <linux/semaphore.h>    // Access semaphores
#include <asm/uaccess.h>        // Copy to and from user space

#define VIRTUAL_DEVICE_SIZE 100

/**
 * Create fake device
 * 
 * There isn't a real device to write to at the moment so just a buffer is used
 */
struct fake_device {
  char data[VIRTUAL_DEVICE_SIZE];
  struct semaphore sem;
} virtual_device;


/**
 * Declare variables
 * 
 * Kernel stack is small so we don't want to declare a lot of variables in functions
 */
struct cdev * mcdev;                      // Pointer to driver (I think)
int major_number;                         // Stores out major number
int ret;                                  // Holds return values
dev_t dev_num;                            // Holds major number given to us by kernel
#define DEVICE_NAME   "test_device"   // Name for this driver

/**
 * Set file operations
 * 
 * Functions to execute when certian user-space commands are used
 */
int device_open(struct inode * inode, struct file * filep) {
  // Only allow one process to open this device
  if (down_interruptible(&virtual_device.sem) != 0) {
    printk(KERN_ALERT "test_device: could not lock device during open");
    return -1;
  }

  printk(KERN_INFO "test_device: opened device");
  return 0;
}

ssize_t device_read(struct file * filep, char * destination_buffer, size_t buffer_size, loff_t * offset) {
  // Send data from kernel to user
  ssize_t bytes_read;
  printk(KERN_INFO "test_device: reading %lld bytes from virtual device", *offset);
  ret = copy_to_user(destination_buffer, virtual_device.data, *offset);
  if (ret > 0) {
    printk(KERN_ALERT "Could not read %i bytes from virtual device", ret);
    bytes_read = *offset - ret;
  }
  else
    bytes_read = *offset;
  *offset = 0;
  return bytes_read;
}

ssize_t device_write(struct file * filep, const char * source_buffer, size_t buffer_size, loff_t * offset) {
  // Send data from user to kernel
  size_t bytes_to_write;
  printk(KERN_INFO "test_device: writing %lu bytes to virtual device (offset: %lld)", buffer_size, *offset);

  bytes_to_write = buffer_size;
  if (buffer_size > VIRTUAL_DEVICE_SIZE - *offset)
    bytes_to_write = VIRTUAL_DEVICE_SIZE - *offset;

  ret = copy_from_user(virtual_device.data + *offset, source_buffer, bytes_to_write);
  if (ret > 0)
  {
    printk(KERN_ALERT "Could not write %i bytes to virtual device", ret);
    bytes_to_write -= ret;
  }
  *offset += bytes_to_write;
  return bytes_to_write;
}

int device_close(struct inode * inode, struct file * filep) {
  // Release lock on device
  up(&virtual_device.sem);
  printk(KERN_INFO "test_device: closed device");
  return 0;
}

struct file_operations fops = {
  .owner = THIS_MODULE,           // Prevent unloading of this module when operations are in use
  .open = device_open,            // Points to the method to call when openeing the device
  .release = device_close,        // Points to the method to call when closing the device
  .write = device_write,          // Points to the method to call when writing to the device
  .read = device_read             // Points to the method to call when reading from the device
};

/**
 * Driver entry
 * 
 * Function called when the kernel loads the module
 */
static int driver_entry(void) {
  ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
  if (ret < 0) {
    printk(KERN_ALERT "test_device: failed to allocate major number");
    return ret;
  }

  major_number = MAJOR(dev_num);
  printk(KERN_INFO "test_device: major number is %d", major_number);
  printk(KERN_INFO "\tuse \"mknod /dev/%s c %d 0\" for device file", DEVICE_NAME, major_number);

  mcdev = cdev_alloc();
  mcdev->ops = &fops;
  mcdev->owner = THIS_MODULE;

  // Add cdev to the kernel
  ret = cdev_add(mcdev, dev_num, 1);
  if (ret < 0) {
    printk(KERN_ALERT "test_device: couldn't add cdev to the kernel");
    return ret;
  }

  // Initialize semaphore
  sema_init(&virtual_device.sem, 1);

  return 0;
}

/**
 * Driver exit
 * 
 * Function called when the kernel releases the module
 */
static void driver_exit(void) {
  // Unregister in reverse order
  cdev_del(mcdev);
  unregister_chrdev_region(dev_num, 1);
  printk(KERN_ALERT "test_device: unloaded module");
}

// Inform kernel where to start and stop
module_init(driver_entry);
module_exit(driver_exit);