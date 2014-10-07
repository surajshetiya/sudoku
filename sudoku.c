#include<stdio.h>
#include <stdlib.h>

int total_respawns,success_level;

void read_sudoku(int arr[9][9]) {
	printf("\nEnter the sudoko(with 0s for empty spaces):\n");
	int i=0,j;
	for(;i<9;i++) {
		for(j=0;j<9;j++) {
			scanf("%d",&arr[i][j]);
		}
	}
	return;
}

void print_sudoku(int arr[9][9]) {
	int i=0,j;
	printf("\n\n");
        for(;i<9;i++) {
		printf("\n");
		for(j=0;j<9;j++) {
                        printf("%d ",arr[i][j]);
		}
	}
	printf("\n\n");
	return;
}

int to_box(int i,int j) {
	// i - ranges between 0-8
	// j - ranges between 0-8
	// return value ranges between 0-8
	return (3*((i/3))+((j/3)));
}

int preprocess(int arr[9][9],int rows[9][2],int cols[9][2],int boxes[9][2]) {
	// rows - [i][0] - contains the number of elements filled in the 'i'th row
	// rows - [i][1] - contains the binary(1s or 0s) representation of the values in the 'i'th row
	// For eg: (row 0)4 0 0 0 0 3 0 0 1
	// rows[0][0] = 3 rows[0][1] = 13 = 000001101(last 9 digits)
	// cols - [i][0] - contains the number of elements filled in the 'i'th column
	// cols - [i][1] - contains the binary(1s or 0s) representation of the values in the 'i'th column
	// boxes - [i][0] - contains the number of elements filled in the 'i'th box
	// boxes - [i][1] - contains the binary(1s or 0s) representation of the values in the 'i'th box
	// returns 0 or non 0 depending on success or failure
        int i=0,j;
	for(;i<9;i++) {
		// Setting initial values to 0
		rows[i][0]=0;
		rows[i][1]=0;
		cols[i][0]=0;
		cols[i][1]=0;
		boxes[i][0]=0;
		boxes[i][1]=0;
	}

        for(i=0;i<9;i++) {
		for(j=0;j<9;j++) {
			if(arr[i][j]) {
				if(rows[i][1]&(1<<(arr[i][j]-1))) {
					//Number already exists in row
					//Invalid case. Return non 0 value.
					return 1;
				}
				if(cols[j][1]&(1<<(arr[i][j]-1))) {
                                        //Number already exists in column
					//Invalid case. Return non 0 value.
					return 1;
				}
				if(boxes[to_box(i,j)][1]&(1<<(arr[i][j]-1))) {
                                        //Number already exists in box
					//Invalid case. Return non 0 value.
                                        return 1;
                                }
				rows[i][1] = rows[i][1] | (1<<(arr[i][j]-1));
				rows[i][0]++;
				cols[j][1] = cols[j][1] | (1<<(arr[i][j]-1));
				cols[j][0]++;
				boxes[to_box(i,j)][1] = boxes[to_box(i,j)][1] | (1<<(arr[i][j]-1));
				boxes[to_box(i,j)][0]++;
			}
		}
	}
	return 0;
}

int _log(int j) {
	// j - meant to be a value like 16 = 0010000
	// log of which yields 4
	int i = 0;
	while((1<<i) != j) {
		i++;
	}
	return i;
}

void print_sudoku_processed(int arr[9][9],int rows[9][2],int cols[9][2],int boxes[9][2]) {
	// print the actual sudoku and also the value set in the rows,cols and boxes arrays.
	print_sudoku(arr);
	int i,j;
	printf("\nRows computation\nR stands for row number\nT stands for total set\nR T 123456789\n");
	for(i=0;i<9;i++) {
		printf("\n%d %d ",i+1,rows[i][0]);
		for(j=0;j<9;j++) {
			if(rows[i][1]&(1<<j)) {
				printf("1");
			} else {
				printf("0");
			}
		}
	}
	printf("\nColumns computation\nC stands for column number\nT stands for total set\nR T 123456789\n");
	for(i=0;i<9;i++) {
		printf("\n%d %d ",i+1,cols[i][0]);
		for(j=0;j<9;j++) {
			if(cols[i][1]&(1<<j)) {
				printf("1");
			} else {
				printf("0");
			}
		}
	}
	printf("\nBoxes computation\nB stands for box number\nT stands for total set\nR T 123456789\n");
	for(i=0;i<9;i++) {
		printf("\n%d %d ",i+1,boxes[i][0]);
		for(j=0;j<9;j++) {
			if(boxes[i][1]&(1<<j)) {
				printf("1");
			} else {
				printf("0");
			}
		}
	}
	return ;
}

void construct_prob(int arr[9][9],int rows[9][2],int cols[9][2],int boxes[9][2],int mat[9][9]) {
	// Set the probability matrix
	// Say that the sudoku trying to be solved is
	/*
		4 0 0 0 0 3 0 0 1 
		0 8 0 5 0 4 6 3 0 
		0 0 3 0 1 0 0 0 0 
		8 1 5 0 0 6 7 9 3 
		2 4 7 0 3 0 1 6 5 
		3 6 9 1 0 0 8 2 4 
		0 0 0 0 9 0 3 0 0 
		0 3 4 7 0 2 0 1 0 
		5 0 0 3 0 0 0 0 2 
		Then mat[0][0] will be 00..001000 <-- 1 is set in the 4th bit from the end
		mat[0][1] will be 00..0(101010010) (last 9 bits bracketed) <--- the values that can arrive there are 2,5,7 and 9
			1,3,4 are eliminated by the row comparisions. 1,3,4,6 by column comparisions. 3,8 by box comparisions.
	*/
	int i=0,j;
	for(;i<9;i++) {
		for(j=0;j<9;j++) {
			if(arr[i][j]) {
				//Case where value is already present in a specific location
				mat[i][j] = 1 << (arr[i][j] - 1);
			} else {
				// Case where value is not present in location
				// Explanation of formula: 
				// For a location (i,j) rows[i][1] indicates all the values in binary present in the 'i'th row,
				// cols[j][1] represents all the values in binary present in the 'j'th column and
				// boxes[box(i,j)][1] represents all the values im binary in the (i,j)th box.
				// Applying a bitwise or operator('|') on these values(rows[i][1], cols[j][1] and boxes[box(i,j)][1] )
				// gives us all the values that are present in the row, column and box for the location (i,j).
				// Hence, as these have already appeared in one/more of these location(s)(row or/and column or/and box),
				// thse cannot appear at location (i,j) and can be eliminated from the probability value.
				// 511 represents number 1 to 9. 511 = 00..00(111111111) (last 9 digits bracketed).
				// Hence 511 - ((rows[i][1] | cols[j][1]) | boxes[to_box(i,j)][1]) will give us which all numbers can appear
				// in the position (i,j).
				mat[i][j] = 511 - ((rows[i][1] | cols[j][1]) | boxes[to_box(i,j)][1]);
			}
		}
	}
	return ;
}

int probability_calc_row(int mat[9][9],int i, int j){
	// Computes the probability of the remaining elements in the row
	int k=0,prob = 0;
	for(;k<9;k++) {
		if(j!=k) {
			prob |= mat[i][k];
		}
	}
	return prob;
}

int probability_calc_col(int mat[9][9],int i, int j){
	// Computes the probability of the remaining elements in the column
        int k=0,prob = 0;
        for(;k<9;k++) {
                if(i!=k) {
                        prob |= mat[k][j];
                }
        }
        return prob;
}

int probability_calc_box(int mat[9][9],int i, int j){
	// Computes the probability of the remaining elements in the box
        int k=0,l,prob=0;
	int x_base = i/3, y_base = j/3;
	for(;k<3;k++) {
		int pos_x = 3*x_base + k;
		for(l=0;l<3;l++) {
			int pos_y = 3*y_base + l;
			if(!(i==pos_x && j==pos_y)) {
				prob |= mat[pos_x][pos_y];
			}
		}
	}
	return prob;
}

void update(int rows[9][2],int cols[9][2],int boxes[9][2],int mat[9][9],int i,int j,int value) {
	// Whenever the algorithm finds a value for a specific location,function "update" is called to update
	// the rows,cols,boxes and probability matrix(mat) with the new change.
	int update_value = 1<<(value - 1);
	rows[i][0]++;
	cols[j][0]++;
	rows[i][1] |= update_value; // Set the value in rows matrix for row (i)
	cols[j][1] |= update_value; // Set the value in cols matrix for column (j)
	boxes[to_box(i,j)][1] = update_value; // Set the value in boxes matrix (for box(i,j))
	mat[i][j] = update_value; // Set the value in probability matrix at (i,j)
	// unset value is used so that we unset element "value" from all the other probability values
	// other than (i,j) in row i, column j and box (i,j)
	// For eg:
	/*	1 2 3 4 5 6 7 8 9

	1	4 0 0 0 0 3 0 0 1 
	2	0 8 0 5 0 4 6 3 0 
	3	0 0 3 0 1 0 0 0 0 
	4	8 1 5 0 0 6 7 9 3 
	5	2 4 7 0 3 0 1 6 5 
	6	3 6 9 1 0 0 8 2 4 
	7	0 0 0 0 9 0 3 0 0 
	8	0 3 4 7 0 2 0 1 0 
	9	5 0 0 0 0 0 0 0 2  <-- 3 is being set to the 4th position and
		row will turn to
		5 0 0 3 0 0 0 0 2
		then to indicate that 3 cannot occur in other places in the 9th row, 4th column and
		8th box we unset 4 (1<<(3-1)) from all the above locations.
		511 - 4 = (507)
		And hence we do a bitwise and('&') to all mat positions.

	*/
	int unset_value = 511 - update_value;
	int k=0,l;
	for(;k<9;k++) {
		if(k != j) {
			mat[i][k] &= unset_value;
		}
		if(k != i) {
			mat[k][j] &= unset_value;
		}
	}
	int x_base = i/3, y_base = j/3;
	for(k=0;k<3;k++) {
		int pos_x = 3*x_base + k;
                for(l=0;l<3;l++) {
			int pos_y = 3*y_base + l;
			if(!(i==pos_x && j==pos_y)) {
				mat[pos_x][pos_y] &= unset_value;
			}
		}
	}
	return ;
}

int prob_mat_eliminations(int arr[9][9],int rows[9][2],int cols[9][2],int boxes[9][2],int mat[9][9]) {
	// Computes the probability of the remaining elements in the row,column and box.
	// If it finds that only 1 element is possible in at a location, this function fills up the value in the location
	// and updates row,cols,boxes and mat(probability matrix) with the findings.
	int total=0;
	int i=0,j;
        for(;i<9;i++) {
                for(j=0;j<9;j++) {
                        if(arr[i][j]==0) {
				int prob_row = 511 - probability_calc_row(mat,i,j);
				// prob_row should be either 0 or should point to 1 value
				if(prob_row==0) {
					// Valid case. Can't eliminate anymore as of now.
				} else if(prob_row & (prob_row-1)) {
					return -1;
				} else {
					int value = _log(prob_row) + 1;
					arr[i][j] = value;
					update(rows,cols,boxes,mat,i,j,value);
					printf("\nMade an row insertion at (%d,%d) with value %d",i,j,value);
					total++;
					break;
				}
				int prob_col = 511 - probability_calc_col(mat,i,j);
                                // prob_col should be either 0 or should point to 1 value
                                if(prob_col==0) {
                                        // Valid case. Can't eliminate anymore as of now.
                                } else if(prob_col & (prob_col-1)) {
                                        return -1;
                                } else {
                                        int value = _log(prob_col) + 1;
                                        arr[i][j] = value;
                                        update(rows,cols,boxes,mat,i,j,value);
					printf("\nMade an col insertion at (%d,%d) with value %d",i,j,value);
					total++;
                                        break;
                                }
				int prob_box = 511 - probability_calc_box(mat,i,j);
                                // prob_box should be either 0 or should point to 1 value
                                if(prob_box==0) {
                                        // Valid case. Can't eliminate anymore as of now.
                                } else if(prob_box & (prob_box-1)) {
                                        return -1;
                                } else {
                                        int value = _log(prob_box) + 1;
                                        arr[i][j] = value;
                                        update(rows,cols,boxes,mat,i,j,value);
					printf("\nMade an box insertion at (%d,%d) with value %d",i,j,value);
					total++;
                                        break;
                                }
			}
		}
	}
        for(i=0;i<9;i++) {
                for(j=0;j<9;j++) {
			if(mat[i][j] ==0) {
				return -1;
			}
			if((arr[i][j]==0) && ((mat[i][j]&(mat[i][j]-1))==0)){
				int value = _log(mat[i][j]) + 1;
				arr[i][j] = value;
				update(rows,cols,boxes,mat,i,j,value);
				total++;
			}
		}
	}
	return total;
}

int total_probables(int x) {
	// Computes the number of 1s in the binary representation of a number.
	// Hence, if the value of (1,1) in probability matrix is (binary)0..01011
	// then this function returns 3 as three 1s are set at positions 1,2 and 4.
	int total = 0;
	while(x!=0) {
		total++;
		x = x & (x-1);
	}
	return total;
}

int restart_operation(int **orig_sud) {
	// If the solution to the sudoku is not found in eliminations, then we can fill up
	// a specific location with one of the probable values and try to find a solution.
	// If the fails then we do a rollback to the actual scenario and fill in a different
	// value in that location.
	// This process is done recursively.
	
	total_respawns++;
	int sud[9][9];
	int cop_x,cop_y;
	for(cop_x=0;cop_x<9;cop_x++) {
		for(cop_y=0;cop_y<9;cop_y++) {
			sud[cop_x][cop_y] = *(*(orig_sud+cop_x)+cop_y);
		}
	}
	print_sudoku(sud);
	int rows[9][2];
        int cols[9][2];
        int boxes[9][2];
        if(preprocess(sud,rows,cols,boxes)) {
                return 1;
        }
        int prob[9][9];
        construct_prob(sud,rows,cols,boxes,prob);
        int val = prob_mat_eliminations(sud,rows,cols,boxes,prob);
        while(val) {
                if(val == -1) {
			printf("Failure in elimination!");
			print_sudoku(sud);
                        return 3;
                }
                printf("\nInserted %d values",val);
                val = prob_mat_eliminations(sud,rows,cols,boxes,prob);
        }
	int i = 0;
	while(i<9) {
		if(rows[i][0] != 9) {
			break;
		}
		i++;
	}
	int pos_x,pos_y;
	if(i!=9) {
		// Unable to find a solution as of now.
		// Will need to spawn restart_operation with a location filled with one of its probabilities.
		// Finding the location which has least probabilities will reduce the task of multiple spawns.
		// Finding the location here, will spawn later.
		int min = 10;
		int j,k;
		for(j=0;j<9;j++) {
			for(k=0;k<9;k++) {
				if(sud[j][k] == 0) {
					int val = total_probables(prob[j][k]);
					if(val < min) {
						min = val;
						pos_x = j;
						pos_y = k;
					}
				}
			}
		}
	} else {
		int j,k;
                // Copy the matrix from the stack to the callers matrix
                printf("\n\nSUCCESS!\nCopying the matrix from the stack..\n");
		success_level++;
                for(j=0;j<9;j++) {
                	for(k=0;k<9;k++) {
                        	*(*(orig_sud+j)+k) = sud[j][k];
                        }   
                }
		return 0;
	}
	int prob_pos = prob[pos_x][pos_y];
	int value = 1;
	while(prob_pos != 0) {
		int unit_set = prob_pos % 2;
		if(unit_set == 1) {
			// Spawn a new restart_operation with a value set at location (pos_x,pos_y)
			int **mat;
			mat = (int **)malloc(9*sizeof(int *));
			for(i=0;i<9;i++) {
				*(mat+i) = (int *)malloc(9*sizeof(int));
			}
			int x,y;
			for(x=0;x<9;x++) {
				for(y=0;y<9;y++) {
					*(*(mat+x)+y) = sud[x][y];
				}
			}
			*(*(mat+pos_x)+pos_y) = value;
			int result = restart_operation(mat);
			if(result == 0) {
				// Success
				int j,k;
				// Copy the matrix from the stack to the callers matrix
				printf("\n\nSUCCESS!\nCopying the matrix from the stack..\n");
				success_level++;
				for(j=0;j<9;j++) {
		                        for(k=0;k<9;k++) {
						*(*(orig_sud+j)+k) = *(*(mat+j)+k);
					}
				}
			}
			int new_m=0;
			for(;new_m<9;new_m++) {
				free(*(mat+new_m));
			}
			free(mat);
			if(result == 0) {
				return 0;
			}
		}
		prob_pos = prob_pos >> 1;
		value++;
	}
	return 4;
}

int main() {
	int sud[9][9];
	read_sudoku(sud);
	printf("\nEntered sudoku is");
	print_sudoku(sud);
	int rows[9][2];
	int cols[9][2];
	int boxes[9][2];
	if(preprocess(sud,rows,cols,boxes)) {
		printf("\nThere is some problem with the sudoku!");
		return 1;
	}
	print_sudoku_processed(sud,rows,cols,boxes);
	int prob[9][9];
	construct_prob(sud,rows,cols,boxes,prob);
	int val = prob_mat_eliminations(sud,rows,cols,boxes,prob);
	while(val) {
                if(val == -1) {
			print_sudoku(sud);
                        printf("\nThere is some problem with the sudoku!");
                        return 3;
                }
                printf("\nInserted %d values",val);
                val = prob_mat_eliminations(sud,rows,cols,boxes,prob);
        }
	int i = 0;
        while(i<9) {
                if(rows[i][0] != 9) {
                        break;
                }
                i++;
        }
        print_sudoku(sud);
	if(i==9) {
		printf("\nSuccess.. found the solution");
		return 0;
	}
	printf("\n\nCOPY MATRIX OPERATION\n\n");
	int **ptr_sud;
	ptr_sud = (int **)malloc(9*sizeof(int *));
	int j;
	for(i = 0;i<9;i++) {
		*(ptr_sud+i) = (int *)malloc(9*sizeof(int));
	}
	for(i = 0;i<9;i++) {
		for(j=0;j<9;j++) {
			*(*(ptr_sud+i)+j) = sud[i][j];
		}
	}	
	int failure = restart_operation(ptr_sud);
	if(!failure) {
		for(i = 0;i<9;i++) {
			for(j=0;j<9;j++) {
				sud[i][j] = *(*(ptr_sud+i)+j);
			}
		}
		print_sudoku(sud);
	}
	for(i = 0;i<9;i++) {
		free(*(ptr_sud+i));
	}
	free(ptr_sud);
	if(!failure) {
		printf("\nSuccess found at level %d\nTotal respawns = %d\n",success_level,total_respawns);
		return 0;
	}
	printf("\n\nFailure in finding a solution!\n");
	return 1;
}
