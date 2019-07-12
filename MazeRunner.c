/* MazeRunner
 *
 * Takes a file that shows you a maze, and print the same maze, but with its
 * width increased (STAGE 1), its reachable squares labelled (STAGE 2), the 
 * minimum number of steps it takes to reach each square (STAGE 3), and the 
 * fastest path out of the maze (STAGE 4).
 *
 * PROGRAMMING IS FUN! :D
 *
 * Created by Benjamin Metha for the subject COMP20005 Engineering Computation
 * Last edit: May 15, 2016
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_ROWS    100
#define MAX_COLUMNS 100

#define DIV "=======\n"

#define FALSE 0
#define TRUE  1

#define FIRST 0

#define STOP     0
#define CONTINUE 1

#define NO_SOLUTION -1

typedef struct {
    int iswall, isused, istruepath;
    // Three flags that tell us whether the cell i) is a wall, ii) has been
    // stepped in, and iii) is part of the most efficient path through the maze.
    int numsteps;
    // The minimum number of steps take to reach this cell.
} cell_t;

typedef struct {
    int rows, cols;
    // The number of rows and columns in this maze.
    cell_t maze[MAX_ROWS][MAX_COLUMNS];
    // An array of all the cells in the maze
    int solution;
    // The number of steps it takes to exit the maze. This is NO_SOLUTION if the
    // maze has no solution.
} maze_t;

void read_data(maze_t *M);

void calc_num_steps(maze_t *M);
void trace(maze_t *M, int row, int col);
void trace_one_step(maze_t *M, int row, int col, int new_row, int new_col);

int  is_reachable(maze_t *M, int row, int col);

void find_true_path(maze_t *M);
void retrace(maze_t *M, int row, int col);
int  retrace_one_step(maze_t *M, int row, int col, int new_row, int new_col);

void print_output1(maze_t *M);
void print_output2(maze_t *M);
void print_output3(maze_t *M);
void print_output4(maze_t *M);

void print_last_2_digits(int n);

int
main(int argc, char *argv[]) {
    maze_t M;
    read_data(&M);
    print_output1(&M);
    calc_num_steps(&M);
    find_true_path(&M);
    print_output2(&M);
    print_output3(&M);
    if(M.solution != NO_SOLUTION) {
        // The maze has a solution
        print_output4(&M);
    }
    return 0;
}

// Read data from user input, and store in the structure pointed to by M.
void
read_data(maze_t *M) {
    int row_index=FIRST, col_index=FIRST, c;
    // Read the first row
    while((c=getchar()) != '\n') {
        if(c=='#') {
            // This cell is a wall.
            M->maze[row_index][col_index].iswall = TRUE;
        } else if(c=='.') {
            // This cell is not a wall.
            M->maze[row_index][col_index].iswall = FALSE;
            M->maze[row_index][col_index].isused = FALSE;
            M->maze[row_index][col_index].istruepath = FALSE;
            M->maze[row_index][col_index].numsteps = 0;
        }
        col_index++;
    }
    // We now know how many columns the maze has
    M->cols = col_index;
    
    // Read all other rows
    while(c != EOF) {
        row_index++;
        for(col_index=0; col_index < M->cols; col_index++) {
            // Read and silently discard all newline characters
            while( (c=getchar()) == '\n');
            // c is now either '#', '.' or EOF.
            
            if(c=='#') {
                // This cell is a wall.
                M->maze[row_index][col_index].iswall = TRUE;
            } else if(c=='.') {
                // This cell is not a wall.
                M->maze[row_index][col_index].iswall = FALSE;
                M->maze[row_index][col_index].isused = FALSE;
                M->maze[row_index][col_index].istruepath = FALSE;
                M->maze[row_index][col_index].numsteps = 0;
            }
        }
    }
    // We now know the number of rows this maze has, too!
    M->rows = row_index;
}

/* Calculate the minimum number of steps it takes to reach each cell in the maze
 * and store these in the structure pointed to by M.
 */
void
calc_num_steps(maze_t *M) {
    int i;
    // Initialise all non-wal cells in the top row
    for(i=FIRST; i<M->cols; i++) {
        if(!M->maze[FIRST][i].iswall) {
            M->maze[FIRST][i].isused = TRUE;
        }
    }
    
    // Trace all paths beginning from each of the cells we just initialised.
    for(i=FIRST; i<M->cols; i++) {
        if(!M->maze[FIRST][i].iswall) {
            trace(M, FIRST, i);
        }
    }
}

/* This function traces all possible paths beginning from the cell at the
 * specified position. If it is applied to all cells in the first row, it will
 * eventually assign values for the minimum number of steps it takes to reach
 * all cells in the maze.
 */
void
trace(maze_t *M, int row, int col) {
    // Trace all paths from the cell one row below the current cell
    trace_one_step(M, row, col, row+1, col);
    // Repeat for the cells one column to the right and left, and one row up.
    trace_one_step(M, row, col, row, col+1);
    trace_one_step(M, row, col, row, col-1);
    trace_one_step(M, row, col, row-1, col);
}

/* First, check if the new cell can be reached - ie. it is inside the maze and
 * not a wall.
 *
 * Then, if the cell has not yet been reached or it can be reached in less steps
 * than was previously possible, set its tag to USED, set its number of steps to
 * be one more than the current cells, and trace all possible paths from that 
 * cell.
 */
void
trace_one_step(maze_t *M, int row, int col, int new_row, int new_col) {
    // If the cell is not reacable, exit the program now.
    if(!is_reachable(M, new_row, new_col)) {
        return;
    }
    if(((M->maze[row][col].numsteps +1) < M->maze[new_row][new_col].numsteps) ||
    !(M->maze[new_row][new_col].isused)) {
        M->maze[new_row][new_col].numsteps = M->maze[row][col].numsteps + 1;
        if(!M->maze[new_row][new_col].isused) {
            M->maze[new_row][new_col].isused = TRUE;
        }
        trace(M, new_row, new_col);
    }
}

// Returns TRUE if a cell is (i) inside our maze and (ii) not a wall.
int
is_reachable(maze_t *M, int row, int col) {
    // Return FALSE if the cell is outside the maze.
    if(row < FIRST || row > M->rows-1 || col < FIRST || col > M->cols-1) {
        return FALSE;
    }
    // Return FALSE if the cell is a wall.
    if(M->maze[row][col].iswall) {
        return FALSE;
    }
    // If we haven't yet returned FALSE, then the cell is reachable.
    return TRUE;
}

/* Tag the cells that are part of the most efficient path through the maze.
 * If two paths are equally short, it takes the one beginning from the left most
 * cell in the bottom row.
 * Set the "solution" field of the maze structure to be the minimum number of
 * steps it takes to solve this maze, or NO_SOLUTION if it cannot be solved.
 */
void
find_true_path(maze_t *M) {
    /* Find the leftmost cell in the bottom row that can be reached in the
     * smallest number of steps.
     */
    int min_num_steps;
    int last_cell_col; // The column index of the last cell in the fastest path
    int col_index=FIRST;
    
    /* First, find the first cell in the bottom row that has been reached. If 
     * this cannot be done, the maze may not be solved.
     */
    while(!(M->maze[M->rows-1][col_index].isused) ||
          M->maze[M->rows-1][col_index].iswall) {
        if(col_index == M->cols) {
            /* Then we have reached the last cell in the bottom row and none of 
             * the cells we have seen can be reached.
             */
            M->solution = NO_SOLUTION;
            // Exit the program
            return;
        }
        col_index++;
    }
    
    /* If we have not exited by this point, then we are looking at the first
     * reachable cell in the bottom row.
     */
    min_num_steps = M->maze[M->rows-1][col_index].numsteps;
    last_cell_col = col_index;
    
    /* Now look at all reamining cells and if any of them can be reached in less
     * steps, update min_num_steps and last_cell_col to be the column index and
     * number of steps taken to reach the final cell.
     */
    for(; col_index<M->cols; col_index++) {
        if(M->maze[M->rows-1][col_index].isused &&
           !(M->maze[M->rows-1][col_index].iswall) &&
           (M->maze[M->rows-1][col_index].numsteps < min_num_steps)) {
            min_num_steps = M->maze[M->rows-1][col_index].numsteps;
            last_cell_col = col_index;
        }
    }
    M->solution = min_num_steps;
    
    // Label this cell as part of the "true path"
    M->maze[M->rows-1][last_cell_col].istruepath = TRUE;
    
    // Flag all cells that are used to get to this cell
    retrace(M, M->rows-1, last_cell_col);
}

/* Finds a path to this cell by looking for an adjacent cell that can be reached
 * in one less step than the current one, labelling it as part of the path, and 
 * then repeating the process for that cell.
 *
 * Note that, unlike trace, this function only calls itself for one cell that is
 * adjacent to the current cell, and not all cells.
 */
void
retrace(maze_t *M, int row, int col) {
    /* Check to see if the cell one row above the current cell can be reached in
     * one less step than the current cell. If it can be, retrace_one_step
     * will call retrace for that cell, and return STOP.
     */
    if(retrace_one_step(M, row, col, row-1, col) == STOP);
    
    /* If STOP was not returned, repeat for the cell one column to the left.
     * Otherwise, this function will end.
     */
    else if(retrace_one_step(M, row, col, row, col-1) == STOP);
    
    // Do the same for the cells one column to the right, and one row down.
    else if(retrace_one_step(M, row, col, row, col+1) == STOP);
    else if(retrace_one_step(M, row, col, row+1, col) == STOP);
}

/* First, check if the new cell can be reached - ie. it is inside the maze and
 * not a wall.
 *
 * Then, if the new cell can be reached in one less step than the current cell, 
 * label the new cell as part of the true path, call retrace for the new cell,
 * and return a flag that terminates retrace for the previous cell.
 */
int
retrace_one_step(maze_t *M, int row, int col, int new_row, int new_col) {
    if(is_reachable(M, new_row, new_col)) {
        if(M->maze[new_row][new_col].numsteps == M->maze[row][col].numsteps-1) {
            M->maze[new_row][new_col].istruepath = TRUE;
            retrace(M, new_row, new_col);
            return STOP;
        }
    }
    return CONTINUE;
}

/* Print the output for Stage 1 - ie. print ## for every wall, and .. for every
 * other cell.
 */
void
print_output1(maze_t *M) {
    int row_index, col_index;
    // First, print a header:
    printf("Stage 1\n");
    printf(DIV);
    // Next, display the number of rows and columns in this maze
    printf("maze has %d rows and %d columns\n", M->rows, M->cols);
    // For each cell in the array
    for(row_index=FIRST; row_index< M->rows; row_index++) {
        for(col_index=FIRST; col_index< M->cols; col_index++) {
            if(M->maze[row_index][col_index].iswall) { // Is the cell a wall?
                printf("##");
            } else {
                printf("..");
            }
        }
        printf("\n");
    }
    printf("\n");
}

/* Print the output for Stage 2 - ie. print ## for every wall, ++ for every
 * reachable cell and -- for every other cell.
 */
void
print_output2(maze_t *M) {
    int row_index, col_index;
    // First, print a header:
    printf("Stage 2\n");
    printf(DIV);
    if(M->solution == NO_SOLUTION) {
        printf("maze does not have a solution\n");
    } else {
        printf("maze has a solution\n");
    }
    // For each cell in the array
    for(row_index=FIRST; row_index< M->rows; row_index++) {
        for(col_index=FIRST; col_index< M->cols; col_index++) {
            if(M->maze[row_index][col_index].iswall) { // Is the cell a wall?
                printf("##");
            } else if(M->maze[row_index][col_index].isused) {
                // If a cell HAS been stepped in, it CAN be stepped in.
                printf("++");
            } else {
                printf("--");
            }
        }
        printf("\n");
    }
    printf("\n");
}

/* Print the output for Stage 3 - ie. print ## for every wall, -- for every
 * unreachable cell, the last two digits of the number of steps it takes to get
 * to all cells with an even number of steps, and ++ for all other cells.
 */
void
print_output3(maze_t *M) {
    int row_index, col_index;
    //First, print a header:
    printf("Stage 3\n");
    printf(DIV);
    if(M->solution == NO_SOLUTION) {
        printf("maze does not have a solution\n");
    } else {
        printf("maze has a solution with cost %d\n", M->solution);
    }
    // For each cell in the array
    for(row_index=FIRST; row_index< M->rows; row_index++) {
        for(col_index=FIRST; col_index< M->cols; col_index++) {
            if(M->maze[row_index][col_index].iswall) { // Is the cell a wall?
                printf("##");
            } else if(!(M->maze[row_index][col_index].isused)) {
                // If a cell has not been used, it cannot be reached!
                printf("--");
            } else if((M->maze[row_index][col_index].numsteps)%2){
                // The cell has an odd number of steps.
                printf("++");
            } else {
                // The cell has an even number of steps.
                print_last_2_digits(M->maze[row_index][col_index].numsteps);
            }
        }
    printf("\n");
    }
    printf("\n");
}

/* Print the output for Stage 4 - ie. print ## for every wall, -- for every
 * unreachable cell, the last two digits of the number of steps it takes to get
 * to all cells in the most efficient path with an even number of steps, .. for 
 * all cells in the most efficient path with an odd number of steps, and "  "
 * for all other cells.
 */
void
print_output4(maze_t *M) {
    int row_index, col_index;
    //First, print a header:
    printf("Stage 4\n");
    printf(DIV);
    printf("maze solution\n");
    //For each cell in the array
    for(row_index=FIRST; row_index< M->rows; row_index++) {
        for(col_index=FIRST; col_index< M->cols; col_index++) {
            if(M->maze[row_index][col_index].iswall) { // Is the cell a wall?
                printf("##");
            } else if(!(M->maze[row_index][col_index].isused)) {
                // If a cell has not been stepped in, it cannot be reached!
                printf("--");
            } else if(!(M->maze[row_index][col_index].istruepath)) {
                // Do not print the number of steps
                printf("  ");
            } else if((M->maze[row_index][col_index].numsteps)%2){
                // The cell is in the true path and has an odd number of steps.
                printf("..");
            } else {
                // The cell has an even number of steps.
                print_last_2_digits(M->maze[row_index][col_index].numsteps);
            }
        }
        printf("\n");
    }
    printf("\n");
}

// Prints the last 2 digits of a number, n. If n has less than 2 digits,
// it is printed as 0n.
void
print_last_2_digits(int n) {
    if(n%100 < 10) {
        // The second to last digit is zero
        printf("0%d",n%10);
        // mod 10 gets the last digit
    } else {
        printf("%2d",n%100);
        // mod 100 gets the last 2 digits if the second last digit is not zero.
    }
}
