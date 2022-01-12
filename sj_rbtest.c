/*
  Red Black Tree test code
  2020-2 LINUX kernel lect. CAUCSE
  test module for Linux kernel 5.4.0 version.
*/
//kernel.h는 rbtree_augmented.h에 rbtree.h랑 같이 include된다.

#include <linux/kernel.h>
#include <linux/rbtree_augmented.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/slab.h>

#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/time.h>
// #include "rbtreeMuki.h"

#define COMPARE(x,y) ( ((x)<(y))? -1 : ((x)==(y))? 0 : 1)
#define __param(type, name, init, msg)		\
	static type name = init;				\
	module_param(name, type, 0444);			\
	MODULE_PARM_DESC(name, msg);

__param(int, num_nodes, 10000, "Number of nodes in the rb-tree");

#define NUM_OF_THREAD 1
#define BILLION 1000000000
void calclock(struct timespec *myclock, unsigned long long *total_time, unsigned long long *total_count) {
	long temp,temp_n;
	unsigned long long timedelay=0;
	if(myclock[1].tv_nsec >=myclock[0].tv_nsec){
		temp=myclock[1].tv_sec - myclock[0].tv_sec;
		temp_n=myclock[1].tv_nsec - myclock[0].tv_nsec;
		timedelay = temp*BILLION+temp_n;

	}else{
		temp=myclock[1].tv_sec - myclock[0].tv_sec -1;
		temp_n= BILLION + myclock[1].tv_nsec - myclock[0].tv_nsec;
		timedelay = temp*BILLION+temp_n;
	}
	__sync_fetch_and_add(total_time,timedelay);
	__sync_fetch_and_add(total_count,1);
}
unsigned long long insertTime,searchTime,deleteTime,insertCount,searchCount,deleteCount;

struct test_node{
	int key;
	int value;
	struct rb_node rb;	//containing rbt color property and pointers
};

//struct rb_root root = RB_ROOT; //sj
struct test_node *nodes = NULL;

/*function prototypes*/
/*
void insert(struct test_node *, struct rb_root *);
//void erase(struct test_node *, struct rb_root *);
void erase(int,  struct rb_root *);
struct test_node *search(int , struct rb_root *);

void Enqueue(struct rb_node *);
struct rb_node *Dequeue(void);
bool queue_full(void);
bool queue_empty(void);
int level_order_print(struct rb_root *);
*/

//self
//(kernel source only provides you tools to implement rbtree insert)
//1. bst insert
//2. operations for maintaining rbtree properties
void insert(struct test_node *node)
{
	struct rb_node **new_node = &(tree_root->rb_node);
	struct rb_node *parent = NULL;
	int temp_key = node->key; 
	
	while(*new_node){
		parent = *new_node;
		if(temp_key < rb_entry(parent, struct test_node, rb)->key)
			new_node = &parent->rb_left;
		else
			new_node = &parent->rb_right;
	}
	printk(KERN_NOTICE "      :: node INSERT finished :: key = %d", node->key);

	rb_link_node(&node->rb, parent, new_node);
	rb_insert_color(&node->rb, tree_root);
}

//insert function_muki
static int insert_function(void *_data)
{
	struct timespec ck[2];
	int* data =(int*)_data;
	/* rb_node create and insert */
	int i = 0;
	int chunkSize = num_nodes/NUM_OF_THREAD;

	getnstimeofday(&ck[0]);
	
	//down_write(&rwse);
	for (;i<chunkSize;i++) {
		struct test_node *new = kmalloc(sizeof(struct test_node), GFP_KERNEL);
               	if(!new)
               	        return -1;
               	new->value = i*10;
               	new->key = i+(*data)*chunkSize;
               	insert(new);
	}
	//up_write(&rwse);
	printk("(%d)[Insert %d ~ %d nodes] insert %d nodes in the reb black tree\n",*data, (*data)*chunkSize, ((*data)+1)*chunkSize-1, i);

	getnstimeofday(&ck[1]);
	calclock(ck,&insertTime,&insertCount);

	do_exit(0);
}

//self
struct test_node *search(int find_key)
{
	struct rb_node *curr_node = tree_root->rb_node;
	struct test_node *result;	

	while(curr_node){
		switch ( COMPARE( find_key, rb_entry(curr_node, struct test_node, rb)->key ) ){
			case -1:
				curr_node = curr_node->rb_left;
				printk(KERN_INFO "L->");
				break;
			case 0:
				result = rb_entry(curr_node, struct test_node, rb);
				printk(KERN_INFO "found");
				return result;
			case 1:
				curr_node = curr_node->rb_right;
				printk(KERN_INFO "R->");
		}
	}
	printk(KERN_INFO "*NO SEARCH RESULT*");
	return NULL;
}

//search function_muki
static int search_function(void *_data)
{
	struct timespec ck[2];
	int* data = (int*)_data;
	/* rb_tree find node */
	int j = 0;
	int chunkSize = num_nodes/NUM_OF_THREAD;
	getnstimeofday(&ck[0]);
	//down_read(&rwse);
	for (;j<chunkSize;j++) {
		struct test_node *find_node;
		find_node = search(j+(*data)*chunkSize);
        	if(!find_node)
				return -1;
	}

	//up_read(&rwse);
	getnstimeofday(&ck[1]);
	calclock(ck,&searchTime,&searchCount);
	
	do_exit(0);
}

//kernel provided
//void erase(struct test_node *node, struct rb_root *tree_root)
void erase(int key, struct rb_root *tree_root)
{
	struct test_node *data;
	data = search(key);
	//rb_erase(&node->rb, tree_root);
	if (data) {
		rb_erase(&data->rb, tree_root);
		//kfree(data);
	}
	//else
	//	__sync_fetch_and_add(&UNDELETED,1);
	//printk(KERN_NOTICE "      :: node DELETE finished :: key = %d", node->key);
}

//delete function_muki
static int delete_function(void *_data)
{
	struct timespec ck[2];
	int* data = (int*)_data;
	/* rb_tree delete node */
	int k = 0;
	int chunkSize = num_nodes/NUM_OF_THREAD;
	getnstimeofday(&ck[0]);
	//down_write(&rwse);

	for (;k<chunkSize;k++) {
		erase(k+(*data)*chunkSize, tree_root);
	}
	printk("(%d)[Delete %d ~ %d nodes] delete %d nodes in the red black tree\n",*data, (*data)*chunkSize, ((*data)+1)*chunkSize-1, k);
	//up_write(&rwse);

	getnstimeofday(&ck[1]);
	calclock(ck,&deleteTime,&deleteCount);
	
	do_exit(0);
}
/*
//traverse in level_order and print out at each traversal of node.
int level_order_print(struct rb_root *tree_root)
{
	int a = 0;
	struct rb_node *curr_node = NULL;
	struct rb_node *temp_node = NULL;

	printk(KERN_INFO "    - Level order print -");
	printk(KERN_INFO "count  #:  key  :  color");

	curr_node = tree_root->rb_node;	
	
	if(!curr_node){
		printk(KERN_ALERT "**NO input (rb_node)**");
		return 0;
	}

	//Enqueue initial rb_node (root of entire tree)
	Enqueue(curr_node);

	while(curr_node){
		a++;
		curr_node = Dequeue();
		temp_node = curr_node;
		if(temp_node != NULL){
			printk(KERN_INFO "count %2d: %4d  : %4ld \n",
				        a,
					rb_entry(temp_node, struct test_node, rb)->key,
					rb_color(temp_node) );
		}else
			break;

		if(curr_node->rb_left){
			temp_node = curr_node->rb_left;
			Enqueue(temp_node);
		}
		if(curr_node->rb_right){
			temp_node = curr_node->rb_right;
			Enqueue(temp_node);
		}
	}
	return 0;
}
*/

void init_sample_tree(void)
{
	int i;
	for(i = 0; i < num_nodes; ++i)
		nodes[i].key = i;
}

/*
int sample_usage(void)
{
	struct test_node *curr_node = NULL;
	struct rb_node *rb_temp;
	int j;
	
	//alloc memory space (in form of array)
	nodes = kmalloc_array(num_nodes, sizeof(*nodes), GFP_KERNEL);
	if(!nodes)
		return -ENOMEM;
	
	printk(KERN_ALERT "Red Black Tree testing");

	//insert key into memory space
	init_sample_tree();

	for(j = 0; j < num_nodes; ++j){
		insert(nodes + j, &root);
		curr_node = (nodes + j);
		rb_temp = &(curr_node->rb);
		printk(KERN_INFO "test_node key: %d \n", rb_entry(rb_temp , struct test_node, rb)->key );
	}

	//curr_node = kmalloc(sizeof(*curr_node), GFP_KERNEL);
	//curr_node->key = 15;
	//insert(curr_node, &root);

	//level_order_print(&root);

	curr_node = search(12, &root);
	printk(KERN_INFO "search result: %d", curr_node->key);

	for(j = 0; j < num_nodes; ++j){
		erase(nodes + j, &root);
	}
	
	//curr_node = search(15, &root);
	//erase(curr_node, &root);
	
	return 0;
}
*/

//muki sample struct
struct task_struct *insert_thread, *delete_thread, *search_thread;

void struct_example(void)
{
	//int* data = (int*)kmalloc(sizeof(int), GFP_KERNEL);
	int i=0;
	for(;i<NUM_OF_THREAD;i++){
		insert_thread = kthread_run(insert_function, &i, "insert_function");
		msleep(500);
		search_thread = kthread_run(search_function, &i, "search_function");
		msleep(500);
		delete_thread = kthread_run(delete_function, &i, "delete_function");
		msleep(500);
	}
}


int __init rbt_test_module_init(void){
	printk(KERN_INFO "rbt_module_init");
	//sample_usage();
		printk("--------------------------Red Black Tree improvement--------------------------\n");
	
	initiate_rbtree();
	//init_sample_tree();

	struct_example();
	
	msleep(5000);

	printk("rb_tree insert time : %llu | count : %llu\n",insertTime,insertCount);
	printk("rb_tree search time : %llu | count : %llu\n",searchTime,searchCount);
	printk("rb_tree delete time : %llu | count : %llu\n",deleteTime,deleteCount);
	return 0;

	kfree(nodes);
}

void __exit rbt_test_module_cleanup(void){
	printk(KERN_INFO "rbt_module_cleanup");
}

module_init(rbt_test_module_init);
module_exit(rbt_test_module_cleanup);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("SangJin Lee");
MODULE_DESCRIPTION("rbtree test module of 2020-2 lect. CAUniv CSE");
