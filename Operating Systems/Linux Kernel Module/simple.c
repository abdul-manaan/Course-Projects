/*Abdul Manan 20100198 and Muhammad Usman Tariq*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/types.h>

struct birthday {
	int day;
	int month;
	int year;
	struct list_head list;
};

static LIST_HEAD(birthday_list);

void insert(void)
{
	
	struct birthday *person1;
	struct birthday *person2;
	struct birthday *person3;
	struct birthday *person4;
	struct birthday *person5;
	
	person1= kmalloc(sizeof(*person1), GFP_KERNEL);
	person1->day = 2;
	person1->month= 8;
	person1->year = 1995;
	INIT_LIST_HEAD(&person1->list);
	list_add_tail(&person1->list, &birthday_list);

	person2= kmalloc(sizeof(*person2), GFP_KERNEL);
	person2->day = 3;
	person2->month= 6;
	person2->year = 1999;
	INIT_LIST_HEAD(&person2->list);
	list_add_tail(&person2->list, &birthday_list);

	person3= kmalloc(sizeof(*person3), GFP_KERNEL);
	person3->day = 14;
	person3->month= 1;
	person3->year = 2001;
	INIT_LIST_HEAD(&person3->list);
	list_add_tail(&person3->list, &birthday_list);

	person4= kmalloc(sizeof(*person4), GFP_KERNEL);
	person4->day = 21;
	person4->month= 2;
	person4->year = 2007;
	INIT_LIST_HEAD(&person4->list);
	list_add_tail(&person4->list, &birthday_list);

	person5= kmalloc(sizeof(*person5), GFP_KERNEL);
	person5->day = 30;
	person5->month= 9;
	person5->year = 2018;
	INIT_LIST_HEAD(&person5->list);
	list_add_tail(&person5->list, &birthday_list);
	
}


void traverse(void)
{
	struct birthday *ptr;
	int x = 1;
	list_for_each_entry(ptr, &birthday_list,list){
		printk("Person%d's Birthday: Day %d, Month %d, Year %d\n",x++,ptr->day,ptr->month,ptr->year);		
	}
}

/* This function is called when the module is loaded. */
int simple_init(void)
{	
	insert();

       printk(KERN_INFO "Loading Module\n");

	traverse();
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

