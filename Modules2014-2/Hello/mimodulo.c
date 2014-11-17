#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

/*int init_module(void) {*/
static int hello_init(void){
 /* printk(KERN_ALERT "Hello world\n");*/
  printk("<1> Hello world\n");
  return 0;
}
/*void cleanup_module(void) {*/
static void hello_exit(void){
/*  printk(KERN_ALERT "Goodbye world\n");*/
  printk("<1> Goodbye world\n");
}

module_init(hello_init);
module_exit(hello_exit);
