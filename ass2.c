/*Assignment 2 written by James Hicks #833966 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_TREES 52
#define HEAD_LINES 1
#define MEGA_LITRE_CONVERSION 0.000001
#define ROWS 60
#define COLUMNS 70
#define EMPTY -1
#define X_DISPLACEMENT 0.5
#define Y_DISPLACEMENT 1.0
#define Y_SQUASH 2
#define STRESS_LIMIT 1
#define TREE_DIES 1
#define NO_TREES_DIE 0
#define CELL_SIZE 2

typedef struct {
		char 	label;
		int 	square_meters;
		double 	xloc;
		double 	yloc;
		double 	litres;
		double 	rootrad;
		double 	water_received;
		double stress_factor;
	} trees_t;
	

int 	input_trees(trees_t trees[]);
int		mygetchar();
double 	conflict_distance(trees_t trees[], int count, int conflict_count);
double 	cell_distance(trees_t trees[], double cell_xloc, double cell_yloc,
			int count);
double 	total_water(trees_t trees[], int tree_number);
char	cell_check(int row, int column, int tree_number, trees_t trees[]);
void 	stage_one(trees_t trees[], int tree_number);	
void 	stage_two(trees_t trees[], int tree_number);
void 	stage_three(trees_t trees[], int tree_number,
			char cells[(ROWS/Y_SQUASH)][COLUMNS]);
void 	stage_four(trees_t trees[], int tree_number,
			char cells[(ROWS/Y_SQUASH)][COLUMNS], double rain_fall);
void 	print_stage3(trees_t trees[], int tree_number,
				char cells[(ROWS/Y_SQUASH)][COLUMNS]);
void 	print_stage4(trees_t trees[], int tree_number,
				char cells[(ROWS/Y_SQUASH)][COLUMNS]);
void	cell_fill(int row, int column, int tree_number, trees_t trees[], 
								char cells[(ROWS/Y_SQUASH)][COLUMNS]);
void	land_finder(trees_t trees[], int tree_number);
void	water_finder(trees_t trees[], int tree_number, double rain_fall);
void 	stress_finder(trees_t trees[], int tree_number);
int	 	death_finder(trees_t trees[], int tree_number, 
					char cells[(ROWS/Y_SQUASH)][COLUMNS], double rain_fall);


/*Computing is fun*/

int 
main(int argc, char *argv[]){
	
	/*declare struct that holds information of the trees*/
	trees_t trees[MAX_TREES];
	/*declare 2D array that represents the grid in which the trees lie*/
	char	cells[(ROWS/Y_SQUASH)][COLUMNS];
	/*finds the number of trees and inputs data to the trees struct*/
	int 	tree_number=input_trees(trees);
	/* the second input in the comand line will be the rain*/
	double rain_fall=atoi(argv[1]);
	
	
	stage_one(trees, tree_number);
	stage_two(trees, tree_number);
	stage_three(trees, tree_number, cells);
	stage_four(trees, tree_number, cells, rain_fall);
	
return 0;
}
/* taken from assignment specifacation */
int
mygetchar() {
		int c;
		while ((c=getchar())=='\r') {
		}
		return c;
}

/*find the number of trees and inputs data to the trees struct adapted from 	
 	assignment one*/
int
input_trees(trees_t trees[]){
	
	int tree_number=0;
	while( mygetchar()!= '\n');
	
	while(scanf("%c%lf%lf%lf%lf\n", &trees[tree_number].label, 
		&trees[tree_number].xloc, &trees[tree_number].yloc, 
		&trees[tree_number].litres, &trees[tree_number].rootrad) == 5
		&& tree_number < MAX_TREES-1) {
		trees[tree_number].square_meters=0;
		trees[tree_number].water_received=0;
		trees[tree_number].stress_factor=0;
		tree_number++;
	}
	return tree_number;
}

/* find the total water needed by all trees */
double
total_water(trees_t trees[], int tree_number) {
	
	double water_needed=0;
	int count;
	
	for (count=0; count<=tree_number; count++) {
		water_needed+= trees[count].litres;
	}
	
	water_needed*=MEGA_LITRE_CONVERSION;
	return water_needed;
}


void 
stage_one(trees_t trees[], int tree_number){
	
	double water_needed;
	
	water_needed=total_water(trees, tree_number);
	
	printf("S1: total data lines   = %5d trees\n", tree_number);
	printf("S1: total water needed = %.3lf megalitres per year\n\n", 
		water_needed);
}

/* find the distance between two trees */
double
conflict_distance(trees_t trees[], int count, int conflict_count){
	double distance;
	
	distance=sqrt((trees[count].xloc-trees[conflict_count].xloc) *
		(trees[count].xloc-trees[conflict_count].xloc) +
		(trees[count].yloc-trees[conflict_count].yloc) *
		(trees[count].yloc-trees[conflict_count].yloc));
	
	return distance;
	}

void 
stage_two(trees_t trees[], int tree_number){
	
	int count, conflict_count;
	/* Cycle through the trees to find which are in conflict*/
	for (count=0; count <tree_number; count++){
		printf("S2: tree %c is in conflcit with ",trees[count].label);
		
		for (conflict_count=0; conflict_count <tree_number; 
				conflict_count++) {
			/*if the tree we are checking is the same as the tree we are 
				checking it against skip to the next tree*/
			if(conflict_count==count){
				continue;
				
			}
			/*if the trees distance apart is less then the combined distance 
				of their radii then the trees are in conflict*/
			if( conflict_distance(trees, count, conflict_count)
				<= trees[count].rootrad +trees[conflict_count].rootrad){
			
			printf("%c ", trees[conflict_count].label);
			}
			
		}
		printf("\n\n");
	}
}


/*finds the distance between the midpoint of a cell and a tree*/
double 
cell_distance(trees_t trees[], double cell_xloc, double cell_yloc,
			int count){
	
	double distance;
	
	distance=sqrt((trees[count].xloc-cell_xloc) *
		(trees[count].xloc-cell_xloc) +
		(trees[count].yloc-cell_yloc) *
		(trees[count].yloc-cell_yloc));
		
	return distance;
	}

/*check whether and what tree claims a cell*/
char
cell_check(int row, int column, int tree_number, trees_t trees[]){
	
	int dom_tree=EMPTY, count;
	double cell_xloc, cell_yloc;
	char cell_label;
	cell_xloc = column+X_DISPLACEMENT;
	cell_yloc = (double) (ROWS-(Y_SQUASH*row))-Y_DISPLACEMENT;
	
	/* Cycles through the trees checking if a cell is claimed*/
	for (count=0; count<tree_number;  count++){
		/* checks if  the distance between the midpoint of the cell and the 
			tree is less then the root radius of the tree in question*/
		if (cell_distance(trees, cell_xloc, cell_yloc, count)<=
			trees[count].rootrad){
			/*if no tree has already claimed this cell and the current tree
				is in range the current tree claimes the cell*/
			if (dom_tree==EMPTY){ 
				dom_tree=count;
				cell_label=trees[dom_tree].label;
			/* if the cell has already been claimed find whether the current 
				tree is closer to the cell then the dominating tree, if so 
				current tree claims it, if not the dominating tree retains it
				*/
			} if (cell_distance(trees, cell_xloc, cell_yloc, count)<
				cell_distance(trees, cell_xloc, cell_yloc, dom_tree)){
					dom_tree=count;
					cell_label=trees[dom_tree].label;
					}
			/*if no tree claims the cell it is left 'empty'*/
			} else if (dom_tree==EMPTY){ 
				  cell_label=' ';
		}
	}	
	return cell_label;
}

/*Fills the 2D array with the labels of dominating trees*/
void
cell_fill(int row, int column, int tree_number, trees_t trees[], 
								char cells[(ROWS/Y_SQUASH)][COLUMNS]){
	
	cells[row][column]=cell_check(row, column, tree_number, trees);
}

/*Prints the graph for stage three*/
void
print_stage3(trees_t trees[], int tree_number,
				char cells[(ROWS/Y_SQUASH)][COLUMNS]) {

	int row, column;
	printf("S3: %2.0d  +\n", ROWS);
	for(row=0; row<(ROWS/Y_SQUASH); row++) {
		printf("S3: ");
		if ((row+1)%5==0){
			printf("%2.0lf  +", (double)ROWS-(Y_SQUASH*(row+1)));
		} else {
			printf("\t|");
		}
		for(column=0; column<COLUMNS; column++) {
			cell_fill(row, column, tree_number, trees, cells);
			printf("%c",  cells[row][column]);
		}
		printf("\n");
	}

	printf("S3:\t ");
	for(column=0; column<=COLUMNS; column++) {
		if(column%10==0){
			printf("+");
		} else {
			printf("-");
		}
	}
	printf("\nS3:\t ");
	for(column=0; column<=COLUMNS; column++) {
		if (column%10==0){
			printf("0");
		} else if ((column+1)%10==0) {
			printf("%d",(column+1)/10); 
		} else {
		printf(" ");
		}
	}
	printf("\n\n\n");
}
	

void
stage_three(trees_t trees[], int tree_number,
				char cells[(ROWS/Y_SQUASH)][COLUMNS]) {
	
	print_stage3(trees, tree_number, cells);
	
	
	
}
	
/*Find the land in square meters claimed by each tree*/
void
land_finder(trees_t trees[], int tree_number){
	
	int row, column, count;
	/*cycle through the 2D array adding 2 meters squared to the land claimed
		by a tree every time is comes across a cell claimed by that tree*/
	for(row=0; row<(ROWS/Y_SQUASH); row++) {
		for(column=0; column<COLUMNS; column++) {
			for (count=0; count <tree_number; count++){
				if (cell_check(row, column, tree_number, trees)== 
					trees[count].label){
				
				trees[count].square_meters+=CELL_SIZE;
				}
			}
		}
			
	}
}

/*Find the water claimed by a tree*/

void
water_finder(trees_t trees[], int tree_number, double rain_fall){
	
	int count;
	/*water claimed is equal to the square meterage of land claimed by the
		tree multiplyed by the rainfall per square meter*/
	for (count=0; count<tree_number; count++){
		trees[count].water_received=(double) trees[count].square_meters * 
		rain_fall;
		
	}
}

/*Find the stress factor of a tree*/
void
stress_finder(trees_t trees[], int tree_number){
	
	int count;
	/* the stress factor is the water claimed by a tree divide by the water 
		it requires to live*/
	for (count=0; count<tree_number; count++){
	trees[count].stress_factor = trees[count].litres/
										trees[count].water_received;
	}
}


/*finds if a tree dies due to stress factor being greater then 1*/
int 
death_finder(trees_t trees[], int tree_number, 
			char cells[(ROWS/Y_SQUASH)][COLUMNS], double rain_fall){
	
	int row, column, count, dead_tree=EMPTY;
	
	/*fills the cells with current living trees*/
	for(row=0; row<(ROWS/Y_SQUASH); row++) {
			for(column=0; column<COLUMNS; column++) {
				cell_fill(row, column, tree_number, trees, cells);
			}		
	}
	/*calculates the strees each tree is under*/
	land_finder(trees, tree_number);
	water_finder(trees, tree_number, rain_fall);
	stress_finder(trees, tree_number);
		/*cycle through the trees to find the one with the greatest stress
		factor over 1*/
		for (count=0; count<tree_number; count++){
			if(dead_tree==EMPTY){
				if(trees[count].stress_factor>STRESS_LIMIT){
					dead_tree=count;
				} 
			}else if(trees[count].stress_factor>
					trees[dead_tree].stress_factor){
						dead_tree=count;
			}
					
		}		

		/*if a tree died reduce its rootradius and water required to 0*/
		if(dead_tree!=EMPTY){
			printf("S4: tree %c has a stress factor %.2lf and dies next\n",
				trees[dead_tree].label, trees[dead_tree].stress_factor);
			trees[dead_tree].rootrad=0;
			trees[dead_tree].litres=0;
			for (count=0; count<tree_number; count++){
				trees[count].square_meters=0;
				trees[count].water_received=0;
				
			}
			return TREE_DIES;
		} else {
			return NO_TREES_DIE;
		}
}
/* same as stage three but S3 change to S4	*/		
void
print_stage4(trees_t trees[], int tree_number,
				char cells[(ROWS/Y_SQUASH)][COLUMNS]) {

	int row, column;
	printf("S4: %2.0d  +\n", ROWS);
	for(row=0; row<(ROWS/Y_SQUASH); row++) {
		printf("S4: ");
		if ((row+1)%5==0){
			printf("%2.0lf  +", (double)ROWS-(Y_SQUASH*(row+1)));
		} else {
			printf("\t|");
		}
		for(column=0; column<COLUMNS; column++) {
			cell_fill(row, column, tree_number, trees, cells);
			printf("%c",  cells[row][column]);
		}
		printf("\n");
	}

	printf("S4:\t ");
	for(column=0; column<=COLUMNS; column++) {
		if(column%10==0){
			printf("+");
		} else {
			printf("-");
		}
	}
	printf("\nS4:\t ");
	for(column=0; column<=COLUMNS; column++) {
		if (column%10==0){
			printf("0");
		} else if ((column+1)%10==0) {
			printf("%d",(column+1)/10); 
		} else {
		printf(" ");
		}
	}
	printf("\n\n\n");
}


void 	
stage_four(trees_t trees[], int tree_number,
			char cells[(ROWS/Y_SQUASH)][COLUMNS], double rain_fall) {
	printf("S4: rainfall amount = %.1lf\n", rain_fall);
	/*check whether a tree dies until one doesn't then prints new map*/
	while(death_finder(trees, tree_number, cells, rain_fall)==TREE_DIES){
			death_finder(trees, tree_number, cells, rain_fall);	
	} 
	print_stage4(trees, tree_number, cells);
	
}



