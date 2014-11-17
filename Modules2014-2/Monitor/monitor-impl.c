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
int monitor_major = 61;
/* Buffer to store data */
#define MAX_SIZE 8192
#define TRUE 1
#define FALSE 0
static char *monitor_buffer= NULL;
static size_t curr_size;
static size_t curr_pos;
static int busy;
static int waiting;
static int readers;

/* The monitor */
static KMutex mutex;
static KCondition read;
static KCondition own;

int monitor_init(void) {
  int rc;

  /* Registering device */
  rc = register_chrdev(monitor_major, "monitor", &monitor_fops);
  if (rc < 0) {
    printk(
      "<1>monitor: cannot obtain major number %d\n", monitor_major);
    return rc;
  }

  /* Allocating monitor_buffer */
  monitor_buffer = kmalloc(MAX_SIZE, GFP_KERNEL); 
  if (!monitor_buffer) { 
    rc = -ENOMEM;
    goto fail; 
  } 
  memset(monitor_buffer, 0, MAX_SIZE); /* asigna un valor constante (2do argumento) a una cantidad de bytes (3er arg) apuntados por un puntero (1er arg) */
  curr_size= 0; /* tamaño inicial 0 */
  curr_pos= 0; /* posicion inicial 0 */
  busy = FALSE;
  waiting = 0;
  readers = 0;
  m_init(&mutex); /* Se inicializa el monitor */
  c_init(&read);
  c_init(&own);

  printk("<1>Inserting monitor module\n"); 
  return 0;

  fail: 
    monitor_exit(); 
    return rc;
}

void monitor_exit(void) {
  /* Freeing the major number */
  unregister_chrdev(monitor_major, "monitor");

  /* Freeing buffer monitor */
  if (monitor_buffer) {
    kfree(monitor_buffer);
  }

  printk("<1>Removing monitor module\n");
}

static int monitor_open(struct inode *inode, struct file *filp) {
  int rc;
  m_lock(&mutex);
  while (busy == TRUE) { 
 // while (readers > 0 || writers > 0 ){
    waiting++;
    if (c_wait(&own, &mutex)) { /* lo hago esperar en ese caso */
      printk("<1>open interrupted while for access\n"); /* si el wait tuvo un error */
      rc= -EINTR;
      goto epilog;
    }
  }
  printk("<1>open succeeded (%p)\n", filp);
  rc = 0;
  busy = TRUE;
  waiting--;

  epilog:
  m_unlock(&mutex);
  return rc;
}

static int monitor_release(struct inode *inode, struct file *filp) {
  int rc;
  m_lock(&mutex);
  if(readers==0){
    busy = FALSE; //el proceso que llega aqui no necesariamente tiene la propiedad del monitor 
  }
  c_signal(&own); 
  printk("<1>close succeeded (%p)\n", filp);
  rc = 0;

  m_unlock(&mutex);
  return rc;
}

static ssize_t monitor_read(struct file *filp, char *buf, 
                    size_t count, loff_t *f_pos) { 

  ssize_t rc= 0; /* codigo de retorno */
  m_lock(&mutex); /* asume que "este" proceso tiene la proiedad del monitor */
  if(*f_pos > 0 ){ /* si es la primera lectura (*f_pos==0) lee sin bloquearse */
    busy = FALSE;
    readers++;
    while (curr_pos <= *f_pos || busy){  /* condiciones para bloquearse: alguien posee el monitor + nadie ha escrito o fpos fuera de rango */
      c_signal(&own); /* como me ire a dormir, despierto a los que estan en el open */
      if (c_wait(&read, &mutex)) { /* me pongo a dormir en el read */
        printk("<1>read interrupted while waiting for data\n"); /* si el wait tuvo un error */
        rc= -EINTR;
        goto epilog;
      }
    }
    readers--;
  }
  busy = TRUE;
  /* desperte del wait, hay algo para leer */
  if (count > curr_size) { /* evitar desbordes de lectura en el buffer */
    count= curr_size;
  }
  else if(count > curr_pos-*f_pos){
    count=curr_pos-*f_pos;
  }

  printk("<1>read %d bytes at %d (%p)\n", (int)count, (int)*f_pos, filp);

  /* Transfering data to user space */ 
  if (copy_to_user(buf, monitor_buffer, count)!=0) {
    rc= -EFAULT;
    goto epilog;
  }
  *f_pos= curr_pos - (curr_size-count); /* actualizo  */
 /* if(count < curr_size){
    curr_size-=count;
  } */
  rc= count;

epilog:
  m_unlock(&mutex);

  return rc;
}

static ssize_t monitor_write( struct file *filp, const char *buf,
                      size_t count, loff_t *f_pos) {
/* filp es el archivo /dev/monitor
   buf es el buffer a partir del cual escribiremos "count" bytes al /dev/monitor 
   count es la cantidad de bytes a escribir 
   f_pos es la posicion de partida desde la cual se comenzara a escribir */
  ssize_t rc;
  m_lock(&mutex);
  if (count>MAX_SIZE) {
    count = MAX_SIZE;
  }
  printk("<1>write %d bytes at %d (%p)\n", (int)count,(int) curr_pos, filp);

  /* Transfering data from user space */ 
  if (copy_from_user(monitor_buffer, buf, count)!=0) {
    rc= -EFAULT;
    goto epilog;
  }
/* si elo copy_from_user resulto existoso */
  curr_size = count; /* en cada lectura se reescribe completamente el monitor */
  curr_pos += count; /* actualiza la posicion */
  *f_pos= curr_pos; /*el f_pos del write es distinto al f_pos del read */
  c_broadcast(&read); /* lo hago esperar en ese caso */
  rc= count;

epilog:
  m_unlock(&mutex);

  return rc;
}

