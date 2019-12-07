/*
 *
 * Authors: Aaron Asner & Steffan Ogato
 *    Date: December 2, 2019
 *
 * Description: Header File for Sudoku class
 *				Declares all attributes, functions, and structures for implementing 
 *				Sudoku solving via backtracking and DLX
 *
 */


#pragma once
#ifndef Sudoku_H
#define Sudoku_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Sudoku {


private:

	// forward declaration of dancing link(DL) struct
	struct Node;	

	// sudoku board representation
	int board[9][9];

	// DLX attributes
	bool** cover_mat;
	struct Node* header;
	std::vector<struct Node*> solution;
	struct Node** link_mat;
	bool set = false;

	// methods for backtracking
	bool find_empty(int& row, int& col);
	bool in_row(int row, int entry);
	bool in_col(int col, int entry);
	bool in_box(int start_row, int start_col, int entry);
	bool valid_move(int row, int col, int entry);
	
	// methods and structure for DLX
	struct Node;
	int get_index(int row, int col, int digit);
	void box_constraint(int& b);
	void col_constraint(int& b);
	void row_constraint(int& b);
	void cell_constraint(int& b);
	void create_link_mat();
	void cover(struct Node* targ);
	void uncover(struct Node* targ);
	Node* get_min_col();
	void set_DLX_sol();
	bool search();
	

public:

	// constructor and destructor
	Sudoku(const std::string& input_file);
	~Sudoku();

	// public method to solve sudoku via backtracking
	bool backtracking();

	// public method to solve sudoku via DLX
	bool dancing_links();

	// public method to print this objects sudoku board
	void print_board();


};

#endif // !Sudoku_H
