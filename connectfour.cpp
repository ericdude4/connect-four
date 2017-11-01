#include <iostream>
#include <vector>
#include <pthread.h>
#include "ConnectFourAI.h"
using namespace std;

vector< vector<char> > board;	//' ' = empty space, 'C' = computer piece, 'H' = human piece
vector<int> col_height;	//stores the next available row in each column (bottom to top)
vector<int> computer_decisions;//stores the computers counter-plays for potential human moves
ConnectFourAI cpu;
int ply;	//the depth at which the computer searches

// printBoard prints the passed board
void printBoard (vector< vector<char> > printee) {
	cout << '#';
	for (int i = 0; i < printee.size(); ++i){
		cout << i;
	}
	cout << "#" << endl;
	for (int i = printee.size()-1; i >= 0 ; --i) {
		cout << "#";
		for (int j = 0; j < printee[i].size(); ++j) {
			cout << printee[j][i];
		} cout << "#" << endl;
	}
	cout << string(printee.size()+2, '#') << endl; 
}
//this method makes the bottom row disappear if it is full just like in tetris
vector<vector<char> > tetris(vector<vector<char> > board) {
	bool bottom_row_full = true;
	for (int i = 0; i < board.size(); ++i) {	//chack if the bottom row is full
		if (board[i][0] == ' ') {
			bottom_row_full = false;
			break;
		}
	}
	if (bottom_row_full) {		//if the bottom row is full, commence tetrification
		vector<char> temp_row;
		for (int i = 0; i < board.size(); ++i) {
			for (int j = 0; j < board.size()-1; ++j) {
				board[i][j] = board[i][j+1];
			}
		}
		for (int i = 0; i < col_height.size(); ++i){	//reduce the stored column heights
			col_height[i]--;
		}
	}
	return board;
}
//places the given move_choice into its accociated column on the game board
void updateBoard(int move_choice, char type) {
	board[move_choice][col_height[move_choice]] = type;
	col_height[move_choice]++;
}
//this one is used to update the temp in compy method. it doesnt update col_height
vector<vector<char> > updateBoard(vector<vector<char> > temp, int move_choice, char type) {
	temp[move_choice][col_height[move_choice]] = type;
	return board;
}

//prompts the user to input a valid integer
int userSelection() {
	int result;
	while (true) {
		cout << "Select Column to Place Disk: ";
		cin >> result;
		if (col_height[result] < board.size() && result < board.size()) return result;
		cout << "Invalid. ";
	}
}
//this is the method run by the thread. This finds all the computers next moves based on the possible
//moves that the human player can make, and stores the counter moves inside of the global computer_decisions
//vector.
void *compy(void *) {
	computer_decisions.clear();
	vector<vector<char> > temp;
	temp.resize(board.size());
	for (int i = 0; i < board.size(); ++i) temp[i].resize(board.size());	//resize temp vector
	for (int i = 0; i < board.size(); ++i) {
		for (int j = 0; j < board.size(); ++j) {
			temp[i][j] = board[i][j];
		}
	}
	for (int i = 0; i < board.size(); ++i) {	//find the counter move if the human player plays 'n' and
												//store the counter in computer_decisions[n]
		for (int j = 0; j <= board.size(); ++j) { //place piece in the rows
			if (temp[i][j] == ' ') {
				temp[i][j] = 'H';  //apply a valid move
				computer_decisions.push_back(cpu.getChoice(temp, ply));
				temp[i][j] = ' ';	 //revert move
				j = 10;
			}
		}
	}
}
//This checks whether there is a row of four of the given char, then returns true if one is found
bool gameFinished(vector<vector<char> > board, char type) {
	int N, NE, E, SE, S, SW, W, NW;
	int left, right;	//these are necessary for the cylindrical nature of the board
	for (int i = 0; i < board.size(); ++i) {
		for (int j = 0; j < board.size(); ++j) {
			if (board[i][j] == type){
				N = 0;
				NE = 0;
				E = 0;
				SE = 0;
				S = 0;
				SW = 0;
				W = 0;
				NW = 0;
				for (int k = 1; k < 4; ++k){
					left = i-k;
					if (left < 0) left = board.size()+(i-k);
					right = i+k;
					if (right >= board.size()) right = (i+k)-board.size();

					if (board[left][j] == type) W++;
					if (board[left][j] != type) W = 0;
					if (board[right][j] == type) E++;
					if (board[right][j] != type) E = 0;
					if (j-k >= 0 && board[i][j-k] == type) S++;
					if (j-k >= 0 && board[i][j-k] != type) S = 0;
					if (j+k < board.size() && board[i][j+k] == type) N++;
					if (j+k < board.size() && board[i][j+k] != type) N = 0;
					if (j-k >= 0 && board[left][j-k] == type) SW++;
					if (j-k >= 0 && board[left][j-k] != type) SW=0;
					if (j-k >= 0 && board[right][j-k] == type) SE++;
					if (j-k >= 0 && board[right][j-k] != type) SE=0;
					if (j+k < board.size() && board[right][j+k] == type) NE++;
					if (j+k < board.size() && board[right][j+k] != type) NE=0;
					if (j+k < board.size() && board[left][j+k] == type) NW++;
					if (j+k < board.size() && board[left][j+k] != type) NW = 0;
				}
				if (N == 3 || NE == 3 || E == 3 || SE == 3 || S == 3 || SW == 3 || W == 3 || NW == 3){
					return true;
				}
			}
		}
	}
	return false;
}

//main game running method
void connectFour() {
	int move_choice;
	while(true) {
		pthread_t computer_player;	//get list of moves from computer to be ready for user selection
		pthread_create(&computer_player, NULL, compy, NULL);	//start the cpu AI Thread
		move_choice = userSelection();	//input move from user
		updateBoard(move_choice, 'H');	//update board to include users move
		if (gameFinished(board, 'H')) {
			printBoard(board);
			cout << "HUMAN WINS" << endl;
			break;
		}
		pthread_join(computer_player, NULL);	//wait for cpu AI thread to finish if it hasnt already
		if (col_height[computer_decisions[move_choice]] < board.size()-1){
			updateBoard(computer_decisions[move_choice], 'C');//add appropriate counter-play based on users move
		} else {	//if the computer tries to place it out of bounds, do another single search;
			updateBoard(cpu.getChoice(board, ply), 'C');
		}
		printBoard(board);	//print out the resulting board
		if (gameFinished(board, 'C')) {
			cout << "COMPUTER WINS" << endl;
			break;
		}
		board = tetris(board);
	}
}

// initBoard: intializes the size of the board and sets all the spaces to 0 (open)
void initBoard () {
	int board_size;
	cout << "enter board size (n x n): ";
	cin >> board_size;
	cout << "enter a search ply (recommend 2-5): ";
	cin >> ply;
	col_height.resize(board_size);
	board.resize(board_size);
	for (int i = 0; i < board_size; ++i){
		board[i].resize(board_size);
		col_height[i] = 0;
		for (int j = 0; j < board_size; ++j) {
			board[i][j] = ' ';
		}
	} 
}

int main(int argc, char const *argv[])
{
	initBoard();
	printBoard(board);
	connectFour();
	return 0;
}