#ifndef ASTAR_H_
#define ASTAR_H_

typedef struct coord {
	int x;
	int y;
} coord_t;

/* Run A* pathfinding over uniform-cost 2d grids using jump point search.

   grid: 0 if obstructed, non-0 if non-obstructed (the value is ignored beyond that).
   solLength: out-parameter, will contain the solution length
   boundX: width of the grid
   boundY: height of the grid
   start: index of the starting node in the grid
   end: index of the ending node in the grid

   return value: Array of node indexes making up the solution, of length solLength, in reverse order
 */

int *astar_compute (const char *grid, 
		    int *solLength, 
		    int boundX, 
		    int boundY, 
		    int start, 
		    int end);

int *astar_unopt_compute (const char *grid, 
		    int *solLength, 
		    int boundX, 
		    int boundY, 
		    int start, 
		    int end);


/* Compute cell indexes from cell coordinates and the grid width */
int astar_getIndexByWidth (int width, int x, int y);

/* Compute coordinates from a cell index and the grid width */
void astar_getCoordByWidth (int width, int node, int *x, int *y);
#endif
