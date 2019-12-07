/*
 *
 * Authors: Aaron Asner & Steffan Ogato
 *    Date: December 2, 2019
 *
 * Description: main method demonstrating usage of the Sudoku class and containing driver code used to generate results in the accompanying report and presentation
 *
 * Compilation: g++ Source.cpp Sudoku.cpp -o Source
 *
 * Notes: some code commented out described later, also please ensure if uncommmenting code that the provided sudoku puzzles are inside the pwd so the relative pathnames used resolve themselves
 *
 */

#include "Sudoku.h"

#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <vector>


using namespace std;


int main(int argc, const char* argv[]) {


	// Code snippet below was used to generate the near 50k 17 clue sudoku puzzles contained in folder 17clue
	// all_17_clue_sudokus.txt obtained from: https://codegolf.stackexchange.com/questions/190727/the-fastest-sudoku-solver
	//
	/*
	ifstream f_in("all_17_clue_sudokus.txt");
	
	int i, row = 0, col = 0;
	char num;
	f_in >> i;//total puzzles

	for (int j = 1; j <= i; j++) {

		string puzname("17clue/puzzle");
		puzname += to_string(j);
		puzname += ".txt";


		ofstream f_out;
		f_out.open(puzname, ofstream::out);

		while (f_in >> num) {
			if (row == 8 && col == 8) { 
				
				f_out << (int)(num - '0');

				row = 0, col = 0;

				break; 
			}
			else if (row < 8 && col == 8) { 

				f_out << (int)(num - '0');
				f_out << '\n';
				
				row++; col = 0; 
			}
			else { 
				f_out << (int)(num - '0');
				f_out << " ";
				
				col++; 
			}
		}
		f_out.close();
	}
	f_in.close();

	*/
	
	
	// Comparing the time to solve puz3 using backtracking and DLX implementations
	Sudoku s1("Hard/puz3.dat");
	cout << "Solving:";
	s1.print_board();
	auto start_t = std::chrono::high_resolution_clock::now();
	if (s1.backtracking()) {
		auto stop_t = std::chrono::high_resolution_clock::now();
		auto t = std::chrono::duration_cast<std::chrono::milliseconds>(stop_t - start_t);
		cout << "Following solution obtained by backtracking in " << t.count() << " milliseconds";
		s1.print_board();
	}
	
	Sudoku s2("Hard/puz3.dat");
	auto start_t1 = std::chrono::high_resolution_clock::now();
	if (s2.dancing_links()) {
		auto stop_t1 = std::chrono::high_resolution_clock::now();
		auto t1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop_t1 - start_t1);
		cout << "Following solution obtained by DLX in " << t1.count() << " milliseconds";
		s2.print_board();
	}
	

	// Code snippet below times both implementations on solving the 49151 sudoku puzzles generated previous
	// backtracking far superior because all the puzzles are 17-clues which tend to be easier and thus take longer for DLX, as shown
	//
	/*
	string puzname;
	//49151
	auto start_t = std::chrono::high_resolution_clock::now();
	for (int i = 1; i <= 49151; i++) {
		puzname = "17clue/puzzle" + to_string(i);
		Sudoku* s;
		s = new Sudoku(puzname);
		s->dancing_links();
		//s->print_board();
		if (i % 5000 == 4999) {
			cout << (i * 100) / 49151 << "%" << '\n';
		}
		delete s;
	}
	auto stop_t = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::duration_cast<std::chrono::seconds>(stop_t - start_t);
	cout << "49151 puzzles solved by DLX in " << t.count() << " seconds" << endl;

	
	string puzname1;
	auto start_t1 = std::chrono::high_resolution_clock::now();
	//49151

	for (int i = 1; i <= 49151; i++) {
		puzname1 = "17clue/puzzle" + to_string(i);
		Sudoku* s;
		s = new Sudoku(puzname1);
		s->backtracking();
		if (i % 5000 == 4999) {
			cout << (i * 100) / 49151 << "%" << '\n';
		}
		delete s;
	}
	auto stop_t1 = std::chrono::high_resolution_clock::now();
	auto t1 = std::chrono::duration_cast<std::chrono::seconds>(stop_t1 - start_t1);
	cout << "49151 puzzles solved by backtracking in " << t1.count() << " seconds";
	*/

	return 0;
}