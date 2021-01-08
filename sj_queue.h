// statically allocated circular queue (one space empty)
// Made for level_order traversal(BFS) of RBtree.

#include <linux/kernel.h>
#include <linux/rbtree_augmented.h>

#define MAX_QUEUE_SIZE 10
#define TRUE 1
#define FALSE 0

struct rb_node *queue[MAX_QUEUE_SIZE];

int front = 0;
int rear = 0;

bool queue_full(void);      // handle case of full queue
bool queue_empty(void);     // handle case of empty queue
void Enqueue(struct rb_node *);
struct rb_node *Dequeue(void);