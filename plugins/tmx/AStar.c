#include "AStar.h"
#include "IndexPriorityQueue.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Distance metrics, you might want to change these to match your game mechanics

// Chebyshev distance metric for distance estimation by default
static double estimateDistance (coord_t start, coord_t end)
{
	return fmax (abs (start.x - end.x), abs (start.y - end.y));
}

// Since we only work on uniform-cost maps, this function only needs
// to see the coordinates, not the map itself.
// Euclidean geometry by default.
// Note that since we jump over points, we actually have to compute 
// the entire distance - despite the uniform cost we can't just collapse
// all costs to 1
static double preciseDistance (coord_t start, coord_t end)
{
	if (start.x - end.x != 0 && start.y - end.y != 0)
		return sqrt (pow (start.x - end.x, 2) + 
			     pow (start.y - end.y, 2)) ;
	else
		return abs (start.x - end.x) + abs (start.y - end.y);
}

// Below this point, not a lot that there should be much need to change!

typedef int node;

typedef struct astar {
	const char *grid;
	coord_t bounds;
	node start;
	node goal;
	queue *open;
	char *closed;
	double *gScores;
	node *cameFrom;
	int *solutionLength;
} astar_t;

// The order of directions is: 
// N, NE, E, SE, S, SW, W, NW 
typedef unsigned char direction;
#define NO_DIRECTION 8
typedef unsigned char directionset;

// return and remove a direction from the set
// returns NO_DIRECTION if the set was empty
static direction nextDirectionInSet (directionset *dirs)
{
	for (int i = 0; i < 8; i++) {
		char bit = 1 << i;
		if (*dirs & bit) {
			*dirs ^= bit;
			return i;
		}
	}
	return NO_DIRECTION;
}

static directionset addDirectionToSet (directionset dirs, direction dir)
{
	return dirs | 1 << dir;
}

/* Coordinates are represented either as pairs of an x-coordinate and
   y-coordinate, or map indexes, as appropriate. getIndex and getCoord
   convert between the representations. */
static int getIndex (coord_t bounds, coord_t c)
{
	return c.x + c.y * bounds.x;
}

int astar_getIndexByWidth (int width, int x, int y)
{
	return x + y * width;
}

static coord_t getCoord (coord_t bounds, int c)
{
	coord_t rv = { c % bounds.x, c / bounds.x };
	return rv;
}

void astar_getCoordByWidth (int width, int node, int *x, int *y)
{
	*x = node % width;
	*y = node / width;
}


// is this coordinate contained within the map bounds?
static int contained (coord_t bounds, coord_t c)
{
	return c.x >= 0 && c.y >= 0 && c.x < bounds.x && c.y < bounds.y;
}

// is this coordinate within the map bounds, and also walkable?
static int isEnterable (astar_t *astar, coord_t coord)
{
	node node = getIndex (astar->bounds, coord);
	return contained (astar->bounds, coord) && 
		astar->grid[node];
}

static int directionIsDiagonal (direction dir)
{
	return (dir % 2) != 0;
}

// the coordinate one tile in the given direction
static coord_t adjustInDirection (coord_t c, int dir)
{
	// we want to implement "rotation" - that is, for instance, we can
	// subtract 2 from the direction "north" and get "east"
	// C's modulo operator doesn't quite behave the right way to do this,
	// but for our purposes this kluge should be good enough
	switch ((dir + 65536) % 8) {
	case 0: return (coord_t) {c.x, c.y - 1};
	case 1: return (coord_t) {c.x + 1, c.y - 1};
	case 2: return (coord_t) {c.x + 1, c.y };
	case 3: return (coord_t) {c.x + 1, c.y + 1};
	case 4: return (coord_t) {c.x, c.y + 1};
	case 5: return (coord_t) {c.x - 1, c.y + 1};
	case 6: return (coord_t) {c.x - 1, c.y};
	case 7: return (coord_t) {c.x - 1, c.y - 1};
	}
	return (coord_t) { -1, -1 };
}

// logical implication operator
static int implies (int a, int b)
{
	return a ? b : 1;	
}

/* Harabor's explanation of exactly how to determine when a cell has forced
   neighbours is a bit unclear IMO, but this is the best explanation I could
   figure out. I won't go through everything in the paper, just the extra
   insights above what I thought was immediately understandable that it took
   to actually implement this function.

   First, to introduce the problem, we're looking at the immedate neighbours
   of a cell on the grid, considering what tile we arrived from.

   ...  This is the basic situation we're looking at. Supposing the top left
   -X.  period is cell (0,0), we're coming in to cell (1, 1) from (0, 1).
   ...  

   ...  The other basic case, the diagonal case. All other cases are obviously
   .X.  derivable from these two cases by symmetry.
   /..

   The question is: Given that some tiles might have walls, *how many tiles
   are there that we can reach better by going through the center tile than
   any other way?* (for the horizontal case it's ok to only be able to reach
   them as well some other as through the center tile too)

   In case there are no obstructions, the answers are simple: In the horizontal
   or vertical case, the cell directly ahead; in the diagonal case, the three
   cells ahead.

   The paper is pretty abstract about what happens when there *are* 
   obstructions, but fortunately the abstraction seems to collapse into some
   fairly simple practical cases:

   123  Position 4 is a natural neighbour (according to the paper's terminology)
   -X4  so we don't need to count it. Positions 1, 2, 5 and 6 are accessible
   567  without going through the center tile. This leaves positions 3 and 7
   to be looked at.

   Considering position 3 (everything here also follows for 7 by symmetry):
   If 3 is obstructed, then it doesn't matter what's in position in 2.
   If 3 is free and 2 is obstructed, 3 is a forced neighbour.
   If 3 is free and 2 is free, 3 is pruned (not a forced neighbour)

   i.e. logically, 
   3 is not a forced neighbour iff (3 is obstructed) implies (2 is obstructed).

   Similar reasoning applies for the diagonal case, except with bigger angles.
   
 */
/*
static int hasForcedNeighbours (astar_t *astar, coord_t coord, int dir)
{
#define ENTERABLE(n) isEnterable (astar, \
	                          adjustInDirection (coord, dir + (n)))
	if (directionIsDiagonal (dir))
		return !implies (ENTERABLE (-2), ENTERABLE (-3)) ||
		       !implies (ENTERABLE (2), ENTERABLE (3));
	else 
		return !implies (ENTERABLE (-1), ENTERABLE (-2)) ||
		       !implies (ENTERABLE (1), ENTERABLE (2));
#undef ENTERABLE
}
*/
static directionset forcedNeighbours (astar_t *astar, 
				      coord_t coord, 
				      direction dir)
{
	if (dir == NO_DIRECTION)
		return 0;

	directionset dirs = 0;
#define ENTERABLE(n) isEnterable (astar, \
				  adjustInDirection (coord, (dir + (n)) % 8))
	if (directionIsDiagonal (dir)) {
		if (!implies (ENTERABLE (6), ENTERABLE (5)))
			dirs = addDirectionToSet (dirs, (dir + 6) % 8);
		if (!implies (ENTERABLE (2), ENTERABLE (3)))
			dirs = addDirectionToSet (dirs, (dir + 2) % 8);
	}
	else {
		if (!implies (ENTERABLE (7), ENTERABLE (6)))
			dirs = addDirectionToSet (dirs, (dir + 7) % 8);
		if (!implies (ENTERABLE (1), ENTERABLE (2)))
			dirs = addDirectionToSet (dirs, (dir + 1) % 8);
	}	
#undef ENTERABLE	
	return dirs;
}

static directionset naturalNeighbours (direction dir)
{
	if (dir == NO_DIRECTION)
		return 255;

	directionset dirs = 0;
	dirs = addDirectionToSet (dirs, dir);
	if (directionIsDiagonal (dir)) {
		dirs = addDirectionToSet (dirs, (dir + 1) % 8);
		dirs = addDirectionToSet (dirs, (dir + 7) % 8);
	}
	return dirs;
}

static void addToOpenSet (astar_t *astar,
			  int node, 
			  int nodeFrom)
{
	coord_t nodeCoord = getCoord (astar->bounds, node);
	coord_t nodeFromCoord = getCoord (astar->bounds, nodeFrom);

	if (!exists (astar->open, node)) {
		astar->cameFrom[node] = nodeFrom;
		astar->gScores[node] = astar->gScores[nodeFrom] + 
			preciseDistance (nodeFromCoord, nodeCoord);
		insert (astar->open, node, astar->gScores[node] + 
			estimateDistance (nodeCoord, 
					  getCoord (astar->bounds, astar->goal)));
	}
	else if (astar->gScores[node] > 
		 astar->gScores[nodeFrom] + 
		 preciseDistance (nodeFromCoord, nodeCoord)) {
		astar->cameFrom[node] = nodeFrom;
		int oldGScore = astar->gScores[node];
		astar->gScores[node] = astar->gScores[nodeFrom] + 
			preciseDistance (nodeFromCoord, nodeCoord);
		double newPri = priorityOf (astar->open, node)
			- oldGScore
			+ astar->gScores[node];
		changePriority (astar->open, node, newPri);
	}	
}


// directly translated from "algorithm 2" in the paper
static int jump (astar_t *astar, direction dir, int start)
{
	coord_t coord = adjustInDirection (getCoord (astar->bounds, start), dir);
	int node = getIndex (astar->bounds, coord);
	if (!isEnterable (astar, coord))
		return -1;

	if (node == astar->goal || 
	    forcedNeighbours (astar, coord, dir)) {
		return node;
	}

	if (directionIsDiagonal (dir)) {
		int next = jump (astar, (dir + 7) % 8, node);
		if (next >= 0)
			return node;
		next = jump (astar, (dir + 1) % 8, node);
		if (next >= 0)
			return node;
	}
	return jump (astar, dir, node);
}

// path interpolation between jump points in here
static int nextNodeInSolution (astar_t *astar,
			       int *target,
			       int node)
{
	coord_t c = getCoord (astar->bounds, node);
	coord_t cTarget = getCoord (astar->bounds, *target);

	if (c.x < cTarget.x) 
		c.x++;
	else if (c.x > cTarget.x)
		c.x--;

	if (c.y < cTarget.y) 
		c.y++;
	else if (c.y > cTarget.y)
		c.y--;

	node = getIndex (astar->bounds, c);

	if (node == *target)
		*target = astar->cameFrom[*target];

	return node;
}

// a bit more complex than the usual A* solution-recording method,
// due to the need to interpolate path chunks
static int *recordSolution (astar_t *astar)
{
	int rvLen = 1;
	*astar->solutionLength = 0;
	int target = astar->goal;
	int *rv = malloc (rvLen * sizeof (int));
	int i = astar->goal;

	for (;;) {
		i = nextNodeInSolution (astar, &target, i);
		rv[*astar->solutionLength] = i;
		(*astar->solutionLength)++;
		if (*astar->solutionLength >= rvLen) {
			rvLen *= 2;
			rv = realloc (rv, rvLen * sizeof (int));
			if (!rv)
				return NULL;
		}
		if (i == astar->start)
			break;
	}

	(*astar->solutionLength)--; // don't include the starting tile
	return rv;
}


static direction directionOfMove (coord_t to, coord_t from)
{
	if (from.x == to.x) {
		if (from.y == to.y)
			return -1;
		else if (from.y < to.y)
			return 4;
		else // from.y > to.y
			return 0;
	}
	else if (from.x < to.x) {
		if (from.y == to.y)
			return 2;
		else if (from.y < to.y)
			return 3;
		else // from.y > to.y
			return 1;
	}
	else { // from.x > to.x
		if (from.y == to.y)
			return 6;
		else if (from.y < to.y)
			return 5;
		else // from.y > to.y
			return 7;
	}

}

static direction directionWeCameFrom (astar_t *astar, int node, int nodeFrom)
{
	if (nodeFrom == -1)
		return NO_DIRECTION;

	return directionOfMove (getCoord (astar->bounds, node), 
				getCoord (astar->bounds, nodeFrom));
}

static int init_astar_object (astar_t* astar, const char *grid, int *solLength, int boundX, int boundY, int start, int end)
{
	*solLength = -1;
	coord_t bounds = {boundX, boundY};

	int size = bounds.x * bounds.y;

	if (start >= size || start < 0 || end >= size || end < 0)
		return 0;

	coord_t startCoord = getCoord (bounds, start);
	coord_t endCoord = getCoord (bounds, end);

	if (!contained (bounds, startCoord) || !contained (bounds, endCoord))
		return 0;

	astar->solutionLength = solLength;
	astar->bounds = bounds;
	astar->start = start;
	astar->goal = end;
	astar->grid = grid;

	astar->open = createQueue();
	if (!astar->open)
		return 0;

	astar->closed = malloc (size);
	if (!astar->closed) {
		freeQueue (astar->open);
		return 0;
	}

	astar->gScores = malloc (size * sizeof (double));
	if (!astar->gScores) {
		freeQueue (astar->open);
		free (astar->closed);
		return 0;
	}

	astar->cameFrom = malloc (size * sizeof (int));
	if (!astar->cameFrom) {
		freeQueue (astar->open);
		free (astar->closed);
		free (astar->gScores);
		return 0;
	}

	memset (astar->closed, 0, size);

	astar->gScores[start] = 0;
	astar->cameFrom[start] = -1;

	insert (astar->open, astar->start, estimateDistance (startCoord, endCoord));

	return 1;
}


int *astar_compute (const char *grid, 
		    int *solLength, 
		    int boundX, 
		    int boundY, 
		    int start, 
		    int end)
{
	astar_t astar;
	if (!init_astar_object (&astar, grid, solLength, boundX, boundY, start, end))
		return NULL;

	coord_t bounds = {boundX, boundY};
	coord_t endCoord = getCoord (bounds, end);

	while (astar.open->size) {
		int node = findMin (astar.open)->value; 
		coord_t nodeCoord = getCoord (bounds, node);
		if (nodeCoord.x == endCoord.x && nodeCoord.y == endCoord.y) {
			freeQueue (astar.open);
			free (astar.closed);
			free (astar.gScores);

			int *rv = recordSolution (&astar);

			free (astar.cameFrom);

			return rv;
		}

		deleteMin (astar.open);
		astar.closed[node] = 1;

		direction from = directionWeCameFrom (&astar, 
						      node,
						      astar.cameFrom[node]);

		directionset dirs = 
			forcedNeighbours (&astar, nodeCoord, from) 
		      | naturalNeighbours (from);

		for (int dir = nextDirectionInSet (&dirs); dir != NO_DIRECTION; dir = nextDirectionInSet (&dirs))
		{
			int newNode = jump (&astar, dir, node);
			coord_t newCoord = getCoord (bounds, newNode);

			// this'll also bail out if jump() returned -1
			if (!contained (bounds, newCoord))
				continue;

			if (astar.closed[newNode])
				continue;
			
			addToOpenSet (&astar, newNode, node);

		}
	}
	freeQueue (astar.open);
	free (astar.closed);
	free (astar.gScores);
	free (astar.cameFrom);

	return NULL;
}



int *astar_unopt_compute (const char *grid, 
		    int *solLength, 
		    int boundX, 
		    int boundY, 
		    int start, 
		    int end)
{
	astar_t astar;

	if (!init_astar_object (&astar, grid, solLength, boundX, boundY, start, end))
		return NULL;

	coord_t bounds = {boundX, boundY};
	coord_t endCoord = getCoord (bounds, end);

	while (astar.open->size) {
		int node = findMin (astar.open)->value; 
		coord_t nodeCoord = getCoord (bounds, node);
		if (nodeCoord.x == endCoord.x && nodeCoord.y == endCoord.y) {
			freeQueue (astar.open);
			free (astar.closed);
			free (astar.gScores);

			int *rv = recordSolution (&astar);

			free (astar.cameFrom);

			return rv;
		}

		deleteMin (astar.open);
		astar.closed[node] = 1;

		for (int dir = 0; dir < 8; dir++)
		{
			coord_t newCoord = adjustInDirection (nodeCoord, dir);
			int newNode = getIndex (bounds, newCoord);

			if (!contained (bounds, newCoord) || !grid[newNode])
				continue;

			if (astar.closed[newNode])
				continue;
			
			addToOpenSet (&astar, newNode, node);

		}
	}
	freeQueue (astar.open);
	free (astar.closed);
	free (astar.gScores);
	free (astar.cameFrom);
	return NULL;
}

