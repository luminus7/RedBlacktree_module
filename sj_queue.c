// statically allocated circular queue (one space empty)
// Made for level_order traversal(BFS) of RBtree.

#include <linux/kernel.h>
#include <linux/rbtree_augmented.h>

#define MAX_QUEUE_SIZE 10
#define TRUE 1
#define FALSE 0

struct rb_node *queue[MAX_QUEUE_SIZE];

static int front = 0;
static int rear = 0;

bool queue_full(void);          // handle case of full queue
bool queue_empty(void);         // handle case of empty queue
void Enqueue(struct rb_node *);
struct rb_node *Dequeue(void);

bool queue_full(void)
{
	if( (rear + 1) % MAX_QUEUE_SIZE == front )
		return TRUE;
	else
		return FALSE;
}

bool queue_empty(void)
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