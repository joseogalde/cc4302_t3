/* Necessary includes for device drivers */
#include <linux/init.h>
/* #include <linux/config.h> */
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <asm/uaccess.h> /* copy_from/to_user */

#include "kmutex.h"

MODULE_LICENSE("Dual BSD/GPL");

/* Declaration of monitor.c functions */
static int monitor_open(struct inode *inode, struct file *filp);
static int monitor_release(struct inode *inode, struct file *filp);
static ssize_t monitor_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static ssize_t monitor_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void monitor_exit(void);
int monitor_init(void);

/* Structure that declares the usual file */
/* access functions */
struct file_operations monitor_fops = {
  read: monitor_read,
  write: monitor_write,
  open: monitor_open,
  release: monitor_release
};

/* Declaration of the init and exit functions */
module_init(monitor_init);
module_exit(monitor_exit);

/* Global variables of the driver */
/* Major number */
int monitor_major = 62;
/* Buffer to store data */
#define MAX_SIZE 8192
#define TRUE 1
#define FALSE 0
static char *monitor_buffer= NULL;
static size_t curr_size;
static size_t curr_pos;

/* The monitor */
static KMutex mutex;
static KCondition wait_queue;

int monitor_init(void) {
  int rc;

  /* Registering device */
  rc = register_chrdev(monitor_major, "badmonitor", &monitor_fops);
  if (rc < 0) {
    printk(
      "<1>badmonitor: cannot obtain major number %d\n", monitor_major);
    return rc;
  }

  /* Allocating monitor_buffer */
  monitor_buffer = kmalloc(MAX_SIZE, GFP_KERNEL); 
  if (!monitor_buffer) { 
    rc = -ENOMEM;
    goto fail; 
  } 
  memset(monitor_buffer, 0, MAX_SIZE);
  curr_size= 0;
  curr_pos= 0;
  m_init(&mutex);
  c_init(&wait_queue);

  printk("<1>Inserting badmonitor module\n"); 
  return 0;

  fail: 
    monitor_exit(); 
    return rc;
}

void monitor_exit(void) {
  /* Freeing the major number */
  unregister_chrdev(monitor_major, "badmonitor");

  /* Freeing buffer monitor */
  if (monitor_buffer) {
    kfree(monitor_buffer);
  }

  printk("<1>Removing badmonitor module\n");
}

static int monitor_open(struct inode *inode, struct file *filp) {
  printk("<1>open succeeded (%p)\n", filp);
  return 0;
}

static int monitor_release(struct inode *inode, struct file *filp) {
  printk("<1>close succeeded (%p)\n", filp);
  return 0;
}

static ssize_t monitor_read(struct file *filp, char *buf, 
                    size_t count, loff_t *f_pos) { 
  ssize_t rc= 0;
  m_lock(&mutex); 
  while (curr_pos <= *f_pos) {
    if (c_wait(&wait_queue, &mutex)) {
      printk("<1>read interrupted while waiting for data\n");
      rc= -EINTR;
      goto epilog;
    }
  }

  if (count > curr_size) {
    count= curr_size;
  }

  printk("<1>read %d bytes at %d (%p)\n", (int)count, (int)*f_pos, filp);

  /* Transfering data to user space */ 
  if (copy_to_user(buf, monitor_buffer, count)!=0) {
    rc= -EFAULT;
    goto epilog;
  }
  *f_pos= curr_pos - (curr_size-count);
  rc= count;

epilog:
  m_unlock(&mutex);

  return rc;
}

static ssize_t monitor_write( struct file *filp, const char *buf,
                      size_t count, loff_t *f_pos) {
  ssize_t rc;
  m_lock(&mutex);
 
  if (count>MAX_SIZE) {
    count = MAX_SIZE;
  }
  printk("<1>write %d bytes at %d (%p)\n", (int)count, curr_pos, filp);

  /* Transfering data from user space */ 
  if (copy_from_user(monitor_buffer, buf, count)!=0) {
    rc= -EFAULT;
    goto epilog;
  }
  curr_size = count;
  curr_pos += count;
  *f_pos= curr_pos;
  c_broadcast(&wait_queue);
  rc= count;

epilog:
  m_unlock(&mutex);

  return rc;
}

