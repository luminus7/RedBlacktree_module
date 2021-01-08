#include "sj_queue.h"

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