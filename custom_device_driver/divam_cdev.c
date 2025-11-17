#include <linux/module.h>      // Header for kernel modules
#include <linux/fs.h>          // File system functions
#include <linux/cdev.h>        // Character device registration
#include <linux/uaccess.h>     // For copy_to_user and copy_from_user

// Driver Constants 
#define DEVICE_NAME "divam_custom_gpio" // The name that appears in /proc/devices and dmesg
#define MAX_SIZE    256 

// Global Data Structures 
static dev_t dev_num;
static struct cdev my_cdev;
static char kernel_buffer[MAX_SIZE] = "Hello from Divam Kumar's custom kernel driver!\n";


// File Operations Implementations 

static int driver_open(struct inode *i, struct file *f)
{
    pr_info("%s: Device successfully opened.\n", DEVICE_NAME);
    return 0;
}

static ssize_t driver_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    int ret;
    ssize_t bytes_to_read = MAX_SIZE - *off;
    
    if (bytes_to_read <= 0)
        return 0;

    if (bytes_to_read > len)
        bytes_to_read = len;

    // Safely copy data from the Kernel's memory buffer to the User's memory buffer
    ret = copy_to_user(buf, kernel_buffer + *off, bytes_to_read);
    
    if (ret != 0) {
        pr_err("%s: Error copying data to user (EFAULT).\n", DEVICE_NAME);
        return -EFAULT;
    }

    *off += bytes_to_read;
    pr_info("%s: Read %zd bytes (offset: %lld).\n", DEVICE_NAME, bytes_to_read, *off);
    return bytes_to_read;
}

static ssize_t driver_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    int ret;
    
    if (len > MAX_SIZE)
        len = MAX_SIZE;

    // Safely copy data from the User's memory buffer to the Kernel's memory buffer
    ret = copy_from_user(kernel_buffer, buf, len);
    
    if (ret != 0) {
        pr_err("%s: Error copying data from user (EFAULT).\n", DEVICE_NAME);
        return -EFAULT;
    }
    kernel_buffer[len] = '\0'; // Null-terminate the string
    
    pr_info("%s: Wrote %zu bytes. Data: %s\n", DEVICE_NAME, len, kernel_buffer);
    return len;
}

static int driver_release(struct inode *i, struct file *f)
{
    pr_info("%s: Device closed.\n", DEVICE_NAME);
    return 0;
}

// Device Operations Mapping 
static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_release,
    .read = driver_read,
    .write = driver_write,
};

// Module Initialization 
static int __init divam_cdev_init(void)
{
    int ret;
    
    // Dynamically allocate a Major/Minor number range
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("%s: Failed to allocate char dev region.\n", DEVICE_NAME);
        return ret;
    }

    // Initialize the cdev structure and link file operations
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    // Add the device to the system
    ret = cdev_add(&my_cdev, dev_num, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev_num, 1);
        pr_err("%s: Failed to add cdev.\n", DEVICE_NAME);
        return ret;
    }

    pr_info("%s: Module loaded. Major: %d, Minor: %d.\n", DEVICE_NAME, MAJOR(dev_num), MINOR(dev_num));
    return 0;
}

// Module Exit 
static void __exit divam_cdev_exit(void)
{
    cdev_del(&my_cdev);           // 1. Remove the cdev
    unregister_chrdev_region(dev_num, 1); // 2. Free the Major/Minor numbers
    pr_info("%s: Module unloaded.\n", DEVICE_NAME);
}

module_init(divam_cdev_init);
module_exit(divam_cdev_exit);

// Module Metadata 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Divam Kumar");
MODULE_DESCRIPTION("Custom Character Device Driver for QEMU Embedded Linux");
