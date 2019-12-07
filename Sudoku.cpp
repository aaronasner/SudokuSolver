/*
 *
 * Authors: Aaron Asner & Steffan Ogato
 *    Date: December 2, 2019
 *
 * Description: Implementation file for Sudoku class described in the header of the same name
 *				Two methods for solving sudoku puzzles are implemented here, namely backtracking and DLX
 *
 * Notes: Backtracking implementation is not "completely" brute force, as it only guesses moves that are valid, rather than placing digits 1-9
 *		  DLX selects the next column deterministically as the one with the least amount of nodes in the sparse matrix as reccomended by Knuth,
 *		      however non-deterministic column-choosing is also valid
 *
 */


#include "Sudoku.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>

using namespace std;


// constructor taking input filename with encoded sudoku puzzle
Sudoku::Sudoku(const string& input_file) {
	
	// open infile 
	ifstream f_in(input_file);
	int num;
	int row = 0, col = 0;

	// read infile and fill sudoku board
	while (f_in >> num) {
		board[row][col] = num;
		if (row == 8 && col == 8) { break; }
		else if (row < 8 && col == 8) { row++; col = 0; }
		else { col++; }
	}

	// close file
	f_in.close();

}

// destructor, does not need to deallocate anything as all dynamic memory is managed by methods
Sudoku::~Sudoku() {}



////// methods for backtracking

// finds first empty cell in the board, seaching as you would read(left to right, top to bottom) *at least in english
bool Sudoku::find_empty(int& row, int& col) {
	for (row = 0; row < 9; row++) {		
		for (col = 0; col < 9; col++) {
			if (board[row][col] == 0) { return true; }
		}
	}
	return false;
}

// checks to see if an entry(1-9) is in a given row
bool Sudoku::in_row(int row, int entry) {
	for (int col = 0; col < 9; col++) {
		if (board[row][col] == entry) { return true; }
	}
	return false;
}

// checks to see if an entry(1-9) is in a given col
bool Sudoku::in_col(int col, int entry) {
	for (int row = 0; row < 9; row++) {
		if (board[row][col] == entry) { return true; }
	}
	return false;
}

// checks to see if an entry(1-9) is in a given 3x3 box, (start_row, start_col) is the top-left indexing of the 3x3 box which has been pre-calculated
bool Sudoku::in_box(int start_row, int start_col, int entry) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (board[start_row + i][start_col + j] == entry) { return true; }
		}
	}
	return false;
}
 
// checks to see if placing an entry(1-9) in cell(row, col) is a valid move i.e., row, col, and box conditions are not violated by placing entry in cell(row, col)
bool Sudoku::valid_move(int row, int col, int entry) { return !in_row(row, entry) && !in_col(col, entry) && !in_box(row - (row % 3), col - (col % 3), entry) && (board[row][col] == 0); }



////// methods and node struct for DLX

// node struct represents a link
struct Sudoku::Node {
	struct Node* l, * r, * u, * d, * c;	// links to adjacent nodes
	int rowID, colID, count;			// attributes, count is only used for header nodes
};

// method to determine row index of (row, col, digit) tuple in sparse link matrix (+1 because row 0 contains column headers)
int Sudoku::get_index(int row, int col, int digit) { return (row * 81) + (col * 9) + (digit - 1) + 1; }

// method populating cover_mat to reflect 3x3 box contraints of the puzzle
void Sudoku::box_constraint(int& b) {
	for (int row = 0; row < 9; row += 3) {
		for (int col = 0; col < 9; col += 3) {
			for (int digit = 1; digit < 10; digit++, b++) {
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						cover_mat[get_index(row + i, col + j, digit)][b] = true;
					}
				}
			}
		}
	}
}

// method populating cover_mat to reflect column contraints of the puzzle
void Sudoku::col_constraint(int& b) {
	for (int col = 0; col < 9; col++) {
		for (int digit = 1; digit < 10; digit++, b++) {
			for (int row = 0; row < 9; row++) {
				cover_mat[get_index(row, col, digit)][b] = true;
			}
		}
	}
}

// method populating cover_mat to reflect row contraints of the puzzle
void Sudoku::row_constraint(int& b) {
	for (int row = 0; row < 9; row++) {
		for (int digit = 1; digit < 10; digit++, b++) {
			for (int col = 0; col < 9; col++) {
				cover_mat[get_index(row, col, digit)][b] = true;
			}
		}
	}
}

// method populating cover_mat to reflect cell contraints of the puzzle
void Sudoku::cell_constraint(int& b) {
	for (int row = 0; row < 9; row++) {
		for (int col = 0; col < 9; col++, b++) {
			for (int digit = 1; digit < 10; digit++) {
				cover_mat[get_index(row, col, digit)][b] = true;
			}
		}
	}
}

// method to create link matrix, called when appropriate
void Sudoku::create_link_mat() {
	
	// build sparse link matrix
	for (int row = 0; row < 730; row++) {
		for (int col = 0; col < 324; col++) {
			if (cover_mat[row][col]) {
				int x, y;
				if (row > 0) { link_mat[0][col].count++; }
				link_mat[row][col].c = &link_mat[0][col];
				link_mat[row][col].rowID = row;
				link_mat[row][col].colID = col;

				//// link with neighbours

				// left pointer
				x = row, y = col;
				do { y = (y + 323) % 324; } while (!cover_mat[x][y] && (y != col));
				link_mat[row][col].l = &link_mat[row][y];

				// right pointer
				x = row, y = col;
				do { y = (y + 1) % 324; } while (!cover_mat[x][y] && (y != col));
				link_mat[row][col].r = &link_mat[row][y];

				// up pointer
				x = row, y = col;
				do { x = (x + 729) % 730; } while (!cover_mat[x][y] && (x != row));
				link_mat[row][col].u = &link_mat[x][col];

				// down pointer
				x = row, y = col;
				do { x = (x + 1) % 730; } while (!cover_mat[x][y] && (x != row));
				link_mat[row][col].d = &link_mat[x][col];
			}
		}
	}
	
	// link header
	header->r = &link_mat[0][0];
	header->l = &link_mat[0][323];
	link_mat[0][0].l = header;
	link_mat[0][323].r = header;
	
	
}

// method to cover a link(node) of the link matrix
void Sudoku::cover(struct Node* targ) {
	struct Node * row, * rnode;
	struct Node* cnode = targ->c;	// header of column

	// unlink column header from neighbours
	cnode->l->r = cnode->r;
	cnode->r->l = cnode->l;

	// cover
	for (row = cnode->d; row != cnode; row = row->d) {
		for (rnode = row->r; rnode != row; rnode = rnode->r) {
			rnode->u->d = rnode->d;
			rnode->d->u = rnode->u;

			link_mat[0][rnode->colID].count--;
		}
	}

}

// method to uncover a link(node) of the link matrix
void Sudoku::uncover(struct Node* targ) {
	struct Node * rownode, * lnode;
	struct Node* colnode = targ->c; // header of column

	// uncover
	for (rownode = colnode->u; rownode != colnode; rownode = rownode->u) {
		for (lnode = rownode->l; lnode != rownode; lnode = lnode->l) {
			lnode->u->d = lnode;
			lnode->d->u = lnode;

			link_mat[0][lnode->colID].count++;
		}
	}

	// link from neighbours
	colnode->l->r = colnode;
	colnode->r->l = colnode;

}

// method to deterministically find and return a pointer to the header node of the column with the least amount of links
Sudoku::Node* Sudoku::get_min_col() {
	struct Node* h = header;
	struct Node* min_col = h->r;
	h = h->r->r;

	// loop through all column headers
	do {
		if (h->count < min_col->count) { min_col = h; }
		h = h->r;
	} while (h != header);

	return min_col;
}

// method to update sudoku board internally with a solution, called when DLX finds the first solution to a puzzle
void Sudoku::set_DLX_sol() {

	vector<struct Node*>::iterator i;
	int ind, row, col, digit;
	for (i = solution.begin(); i != solution.end(); ++i) {
		ind = (*i)->rowID;
		row = (ind - 1) / 81;
		col = (ind - 1) / 9;
		col = col % 9;
		digit = ind - (row * 81) - (col * 9);

		board[row][col] = digit;
		
	}
}

// recursive method containing the core implementation of Donald Knuth's Algorithm X
bool Sudoku::search() {
	
	struct Node * row_n, * r_n, * l_n, * col_n;

	// check for completion
	if (header->r == header) {
		if (set) { return true; }
		set_DLX_sol();
		if (solution.size() == 81) { set = true; }
		return true;
	}

	// get next column and cover it
	col_n = get_min_col();
	cover(col_n);

	// loop through column links
	for (row_n = col_n->d; row_n != col_n; row_n = row_n->d) {
		solution.push_back(row_n);
		for (r_n = row_n->r; r_n != row_n; r_n = r_n->r) {
			cover(r_n);
		}

		// recursive call
		search();

		// return from call if board is updated with a solution
		if (set) { return true; }

		// backtrack
		solution.pop_back();

		// loop through row links
		col_n = row_n->c;
		for (l_n = row_n->l; l_n != row_n; l_n = l_n->l) {
			uncover(l_n);
		}
	}
	uncover(col_n);
}



////// public methods

// method call to Backtracking algorithm
bool Sudoku::backtracking() {
	int row, col;

	// if no empty cells in the board, then the puzzle is solved
	if (!find_empty(row, col)) { return true; }

	// loop through digits 1-9
	for (int digit = 1; digit < 10; digit++) {

		// valid move (potential correct move)
		if (valid_move(row, col, digit)) {
			board[row][col] = digit; // temp assignment
			if (backtracking()) { return true; } // recursive call
			board[row][col] = 0; // unassign as potential move proves incorrect
		}
	}
	return false; // backtrack
}

// method call to Algorithm X 
bool Sudoku::dancing_links() {
	
	// create and allocate exact cover matrix (boolean)
	cover_mat = new bool* [730];
	for (int i = 0; i < 730; i++) {
		cover_mat[i] = new bool [324];
	}
	for (int i = 0; i < 730; i++) {
		for (int j = 0; j < 324; j++) {
			if (i == 0) { cover_mat[i][j] = true; }
			else { cover_mat[i][j] = false; }
		}
	}

	// update exact cover matrix with column constraints of the puzzle
	int x = 0;
	cell_constraint(x);
	row_constraint(x);
	col_constraint(x);
	box_constraint(x);

	// update cover_mat with final information from sudoku puzzle
	for (int row = 0; row < 9; row++) {
		for (int col = 0; col < 9; col++) {
			int entry = board[row][col];
			if (entry != 0) {
				for (int digit = 1; digit < 10; digit++) {
					if (entry != digit) {
						int ind = get_index(row, col, digit);
						for (int i = 0; i < 324; i++) {
							cover_mat[ind][i] = false;
						}
					}
				}
			}
		}
	}

	// build linked structure
	header = new Node();
	// allocate link matrix
	link_mat = new struct Node* [730];
	for (int i = 0; i < 730; i++) {
		link_mat[i] = new struct Node[324];
	}
	// build link mat
	create_link_mat();
	// search for solution and store if successful in result
	bool result;
	result = search();

	// deallocate heap memory
	for (int i = 0; i < 730; i++) {
		delete[] link_mat[i];
		delete[] cover_mat[i];
	}
	delete[] link_mat;
	delete[] cover_mat;
	delete header;

	return result;
}

// method to print sudoku board to the console
void Sudoku::print_board() {
	cout << endl << "-----------------" << endl;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << board[i][j] << " ";
		}
		std::cout << endl;
	}
	cout << "-----------------" << endl;
}
