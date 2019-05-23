#include <linux/sched.h>
/*Abdul Manan 20100198 and Muhammad Usman Tariq*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/types.h>

struct birthday {
	struct task_struct *task;
	struct list_head list;
};

static LIST_HEAD(birthday_list);

void insert(struct task_struct* initTask)
{
	
	struct birthday *person1;

	person1= kmalloc(sizeof(*person1), GFP_KERNEL);
	person1->task = initTask;
	INIT_LIST_HEAD(&person1->list);
	list_add_tail(&person1->list, &birthday_list);

	struct birthday* ptr;
	struct task_struct *task; 
	struct list_head *list;

	list_for_each_entry(ptr, &birthday_list, list){
		printk("pid: %d - pname: %s - pstate: %d\n", ptr->task->pid, ptr->task->comm, ptr->task->state);		
		
		list_for_each(list, &ptr->task->children) { 
			task = list_entry(list, struct task_struct, sibling); /* task points to the next child in the list */ 
			struct birthday *person1;

			person1= kmalloc(sizeof(*person1), GFP_KERNEL);
			person1->task = task;
			INIT_LIST_HEAD(&person1->list);
			list_add_tail(&person1->list, &birthday_list);
		}
	}


}


// void traverse(void)
// {
// 	struct birthday *ptr;
// 	int x = 1;
// 	list_for_each_entry(ptr, &birthday_list,list){
// 		printk("Person%d's Birthday: Day %d, Month %d, Year %d\n",x++,ptr->day,ptr->month,ptr->year);		
// 	}
// }

/* This function is called when the module is loaded. */
int simple_init(void)
{	
    printk(KERN_INFO "Loading Module\n");
	insert(&init_task);


//	traverse();
       return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
	printk(KERN_INFO "Removing Module\n");

	struct birthday *ptr, *next;

	list_for_each_entry_safe(ptr,next,&birthday_list,list){
		list_del(&ptr->list);
		kfree(ptr);	
	}

}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

