#include "IndexPriorityQueue.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int smallestPowerOfTwoAfter (int x)
{
        if (x < 0)
                return 0;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x+1;
}

int makeSpace (queue *q, size_t size)
{
        unsigned int newAllocated = smallestPowerOfTwoAfter (size * sizeof(item));

        if (newAllocated <= q->allocated)
                return 0;

        q->root = realloc (q->root, newAllocated);
        if (NULL == q->root)
                exit (1); // I'm a bloody Haskell programmer, I don't have any business trying to do anything fancy when realloc fails

        q->allocated = newAllocated;
        return newAllocated;
}

int placeAtEnd (queue *q, item item)
{
	makeSpace (q, q->size + 1);
	q->root[q->size] = item;
	return q->size++;
}

void siftUp (queue *q, int i)
{
	if (0 == i)
		return;

	int p = (i - 1) / 2;

	if (q->root[p].priority < q->root[i].priority)
		return;

	q->index[q->root[i].value] = p;
	q->index[q->root[p].value] = i;

	item swap = q->root[i];
	q->root[i] = q->root[p];
	q->root[p] = swap;

	return siftUp (q, p);
}

void insert (queue *q, int value, double pri)
{
	item i;
	i.value = value;
	i.priority = pri;

        int newAllocated = smallestPowerOfTwoAfter ((value + 1) * sizeof(int));

        if ((value + 1) * sizeof (int) > q->indexAllocated) {
		q->index = realloc (q->index, newAllocated);
		if (NULL == q->index)
			exit (1);
		for (unsigned int j = q->indexAllocated / sizeof (int); j < newAllocated / sizeof (int); j++)
			q->index[j] = -1;
		q->indexAllocated = newAllocated;
	}

	int p = placeAtEnd (q, i);

	q->index[q->root[p].value] = p;	

	siftUp (q, p);
}

void siftDown (queue *q, int i)
{
	int c = 1 + 2 * i;
	if (c >= q->size)
		return;

	if ((c + 1 < q->size) && (q->root[c].priority > q->root[(c+1)].priority))
		c++;

	if (q->root[i].priority < q->root[c].priority)
		return;

	q->index[q->root[c].value] = i;
	q->index[q->root[i].value] = c;

	item swap = q->root[i];
	q->root[i] = q->root[c];
	q->root[c] = swap;

	return siftDown (q, c);
}

void deleteMin (queue *q)
{
	if (0 == q->size)
		return;

	q->index[q->root[0].value] = -1;
	q->size--;

	if (0 == q->size)
		return;

	q->index[q->root[q->size].value] = 0;
	q->root[0] = q->root[q->size];

	siftDown (q, 0);
} 

item *findMin (const queue *q)
{
	return q->root;
}

void changePriority (queue *q, int ind, double newPriority)
{
	int oldPriority = q->root[q->index[ind]].priority;
	q->root[q->index[ind]].priority = newPriority;
	if (oldPriority < newPriority)
		siftDown (q, q->index[ind]);
	else if (oldPriority > newPriority)
		siftUp (q, q->index[ind]);
}

void delete (queue *q, int ind)
{
	changePriority (q, ind, INT_MIN);
	deleteMin (q);
}

int priorityOf (const queue *q, int ind)
{
	return q->root[q->index[ind]].priority;
}

int exists (const queue *q, int ind)
{
	return  (q->indexAllocated / sizeof (int) > ind) &&
		(-1 != q->index[ind]) && 
		(q->size > q->index[ind]);
}

queue* createQueue ()
{
	queue *rv = (queue*) malloc (sizeof (queue));
	if (NULL == rv)
		exit (1);
	rv->size = 0;
	rv->allocated = 0;
	rv->root = NULL;
	rv->index = NULL;
	rv->indexAllocated = 0;
	return rv;
}

void freeQueue (queue* q)
{
	free (q->root);
	free (q->index);
	free (q);
}

