/*
  Red Black Tree test code
  2020-2 LINUX kernel lect. CAUCSE
  test module for Linux kernel 5.4.0 version.
*/

#include <linux/rbtree_augmented.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/slab.h>

#define COMPARE(x,y) ( ((x)<(y))? -1 : ((x)==(y))? 0 : 1)
#define __param(type, name, init, msg)		\
	static type name = init;		\
	module_param(name, type, 0444);		\
	MODULE_PARM_DESC(name, msg);

__param(int, num_nodes, 15, "Number of nodes in the rb-tree");

struct test_node{
	int key;
	char name;
	struct rb_node rb;	//containing rbt color property and pointers
};

struct rb_root root = RB_ROOT; 
struct test_node *nodes = NULL;

/*function prototypes*/
void insert(struct test_node *, struct rb_root *);
void erase(struct test_node *, struct rb_root *);
struct test_node *search(int , struct rb_root *);

void Enqueue(struct rb_node *);
struct rb_node *Dequeue(void);
bool queue_full(void);
bool queue_empty(void);
int level_order_print(struct rb_root *);


//self
//(kernel source only provides you tools to implement rbtree insert)
//1. bst insert
//2. operations for maintaining rbtree properties
void insert(struct test_node *node, struct rb_root *tree_root)
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

//kernel provided
void erase(struct test_node *node, struct rb_root *tree_root)
{
	rb_erase(&node->rb, tree_root);
	printk(KERN_NOTICE "      :: node DELETE finished :: key = %d", node->key);
}

//self
struct test_node *search(int find_key, struct rb_root *tree_root)
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
//statically allocated circular queue (one space empty)

#define MAX_QUEUE_SIZE 10
#define TRUE 1
#define FALSE 0

struct rb_node *queue[MAX_QUEUE_SIZE];

int front = 0;
int rear = 0;

//handle case of full queue
bool queue_full()
{
	if( (rear + 1) % MAX_QUEUE_SIZE == front )
		return TRUE;
	else
		return FALSE;
}

//handle case of empty queue
bool queue_empty()
{
	if( front == rear )
		return TRUE;
	else
		return FALSE;
}

void Enqueue(struct rb_node *rb_ptr)
{
	if( queue_full() ){
		printk(KERN_ALERT "**Queue is FULL**");
		return;	
	}
	rear = (rear + 1) % MAX_QUEUE_SIZE;
	queue[rear] = rb_ptr;
}

struct rb_node *Dequeue()
{
	struct rb_node *dummy = NULL;
	if( queue_empty() ){
		printk(KERN_ALERT "**Queue is EMPTY**");
		return dummy;
	}
	front = (front + 1) % MAX_QUEUE_SIZE;
	return queue[front];
}

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

void init_sample_tree(void)
{
	int i;
	for(i = 0; i < num_nodes; ++i)
		nodes[i].key = i;
}

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
//		printk(KERN_INFO "test_node key: %d \n", rb_entry(rb_temp , struct test_node, rb)->key );
	}

	curr_node = kmalloc(sizeof(*curr_node), GFP_KERNEL);
	curr_node->key = 15;
	insert(curr_node, &root);

	level_order_print(&root);

	curr_node = search(12, &root);
	printk(KERN_INFO "search result: %d", curr_node->key);

	for(j = 0; j < num_nodes; ++j){
		erase(nodes + j, &root);
	}
	curr_node = search(15, &root);
	erase(curr_node, &root);

	return 0;
}

int __init rbt_test_module_init(void){
	printk(KERN_INFO "rbt_module_init");
	sample_usage();
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
