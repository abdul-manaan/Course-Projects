#include <linux/sched.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/types.h>


struct task_struct *task;



/* This function is called when the module is loaded. */
int simple_init(void)
{	

       printk(KERN_INFO "Loading Module\n");

	for_each_process(task)
	{
	   printk("Name: %s PID: [%d] State: %d\n", task->comm, task->pid, task->state);
	}

       return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
	printk(KERN_INFO "Removing Module\n");

}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

