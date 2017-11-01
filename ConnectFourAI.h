#include <vector>
#include <stdlib.h>
using namespace std;

class ConnectFourAI {
   public:
	 	int getChoice(vector<vector<char> > board, int ply) {
	 		int w = findWinningMove(board);
	 		if (w < board.size()) return w;
	 		vector<int> valid_moves = getValidMoves(board);
	 		int descision[valid_moves.size()];
	 		for (int i = 0; i < valid_moves.size(); ++i) {
	 			for (int j = 0; j </*=*/ board.size(); ++j) { //place piece in valid row
	 				if (board[valid_moves[i]][j] == ' ') {
						board[valid_moves[i]][j] = 'H';  //apply a valid move
						descision[i] = minimax(board, ply, -10000, 10000, true);//measure best outcome
						board[valid_moves[i]][j] = ' ';	 //revert move
						break; //break out of loop
					}
				}
	 		}
	 		int best_score = -10000;
	 		int final_descision;
	 		for (int i = 0; i < valid_moves.size(); ++i) {	//select the move that resulted in the best score
	 			if (descision[i] > best_score){
	 				best_score = descision[i];
	 				final_descision = valid_moves[i];
	 			}
	 		}
	    	return final_descision;	//return the move that resulted in the best score
		}
	private:
		//returns the larger of two integers as an integer
		int max (int a, int b) {
			if (a > b) return a;
			else return b;
		}
		//returns the smaller of two integers as an integer
		int min (int a, int b) {
			if (a < b) return a;
			else return b;
		}
		//traverses the minimax tree and performs alpha-beta pruning to return a strategic move 
		//based on the evaluateBoard function.
		int minimax(vector<vector<char> > node, int depth, int a, int b, bool maxing) {
			if (depth == 0) {
				return evaluateBoard(node);	//root node
			}
			if (maxing) {
				int v = -10000; //Arbitrarily low number
				vector<vector<char> > child;
				child.resize(node.size());
				for (int i = 0; i < node.size(); ++i) child[i].resize(node.size());
				vector<int> valid_moves = getValidMoves(node);
				for (int i = 0; i < valid_moves.size(); ++i) {	//for each child
					for (int x = 0; x < node.size(); ++x) {	//copy the board to child
						for (int y = 0; y < node.size(); ++y) {
							child[x][y] = node[x][y];
						}
					}
					for (int j = 0; j < child.size(); ++j) { //place piece in valid row
						if (child[valid_moves[i]][j] == ' ') {
							child[valid_moves[i]][j] = 'C';
							break;
						}
					}
					v = max(v, minimax(child, depth-1, a, b, false));
					a = max(a, v);
					if (b <= a) break;
				}
				return v;
			} else {
				int v = 10000;
				vector<vector<char> > child;
				child.resize(node.size());
				for (int i = 0; i < node.size(); ++i) child[i].resize(node.size());
				vector<int> valid_moves = getValidMoves(node);
				for (int i = 0; i < valid_moves.size(); ++i) {	//for each child
					for (int x = 0; x < node.size(); ++x) {	//copy the board to child
						for (int y = 0; y < node.size(); ++y) {
							child[x][y] = node[x][y];
						}
					}
					for (int j = 0; j < child.size(); ++j) { //place piece in valid row
						if (child[valid_moves[i]][j] == ' ') {
							child[valid_moves[i]][j] = 'H';
							break;
						}
					}
					v = min(v, minimax(child, depth-1, a, b, true));
					b = min(b, v);
					if (b <= a) break;
				}
				return v;
			}
		}
		//evaluates a board state for the human player using a heuristic function in order to minimize this
		int evaluateBoard(vector<vector<char> > board) {
			int total = 0;
			int left, right;
			//This stores the sum of the captured spaces in all directions of the space. [0] = 12 o'clock, [2] = 3 o'clock
			int dirState[8]; 
			for (int i = 0; i < board.size(); ++i) {
				for (int j = 0; j < board.size(); ++j) {
					if (board[i][j] == 'H'){
						//count the caputed and uncaptured directions
						for (int z = 1; z < 4; ++z) {
							left = i - z;
							if (left < 0) left = board.size()+(i-z);
							right = i + z;
							if (right >= board.size()) right = (i+z)-board.size();

							if (j+z < board.size()) {
								if (board[i][j + z] == 'H') dirState[0]+=2;
								if (board[i][j + z] == ' ') dirState[0]++;
							}
							if (j+z < board.size()) {
								if (board[right][j + z] == 'H') dirState[1]+=2;
								if (board[right][j + z] == ' ') dirState[1]++;
							}
							if (board[right][j] == 'H') dirState[2]+=2;
							if (board[right][j] == ' ') dirState[2]++;
							if (j-z >= 0) {
								if (board[right][j - z] == 'H') dirState[3]+=2;
								if (board[right][j - z] == ' ') dirState[3]++;
							}
							if (j-z >= 0) {
								if (board[i][j - z] == 'H') dirState[4]+=2;
								if (board[i][j - z] == ' ') dirState[4]++;
							}
							if (j-z >= 0) {
								if (board[left][j - z] == 'H') dirState[5]+=2;
								if (board[left][j - z] == ' ') dirState[5]++;
							}
							if (board[left][j] == 'H') dirState[6]+=2;
							if (board[left][j] == ' ') dirState[6]++;
							if (j+z < board.size()) {
								if (board[left][j + z] == 'H') dirState[7]+=2;
								if (board[left][j + z] == ' ') dirState[7]++;
							}
						}
						//add the values of the directions
						for (int d = 0; d < 8; ++d) {
							//depending on the sum of dirState[d], add to total
							switch(dirState[d]) {
								case 0:	//no connections to spaces or captures
									total += 0;
									break;
								case 1: //one space in the direction
									total ++;
									break;
								case 2: //two spaces or one captured spot in row
									total +=3;
									break;
								case 3: //three spaces or one capture and a space
									total +=4;
									break;
								case 4: //four spaces, two captures, or one capture two spaces
									total += 7;
									break;
								case 5: //three spaces and a capture, or two captures and a space
									total += 20;
									break;
								case 6: //three adjacent captures
									total += 100;
									break;
								default:
									total += 0;
									break;
							}
							dirState[d] = 0;
						}
					}
				}
			}
			
			return total;
		}
		//scans the board, checking for columns that have reached maxed height, and returns a vector
		//of valid columns to place a piece in
		vector<int> getValidMoves(vector<vector<char> > board) {
			vector<int> result;
			int count;
			for (int i = 0; i < board.size(); ++i) {
				count = 0;
				for (int j = 0; j < board.size(); ++j) {
					if (board[i][j] != ' ') count++;
				}
				if (count < board.size()) result.push_back(i);
			}
			return result;
		}
		//scans the board searching for a winning move, returning the winning move. If it does not find
		//a winning move, it returns board size + 1. This adds value based on the type of the surrounding
		//squares.
		int findWinningMove(vector<vector<char> > board) {
			int N, NE, E, SE, S, SW, W, NW;
			int left, right;
			for (int i = 0; i < board.size(); ++i) {
				for (int j = 0; j < board.size(); ++j) {
					if (board[i][j] == ' ' && surroundingC(i,j,board)){
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

							if (board[left][j] == 'C') W++;
							if (board[left][j] == 'H') W = 0;
							if (board[right][j] == 'C') E++;
							if (board[right][j] == 'H') E = 0;
							if (j-k >= 0 && board[i][j-k] == 'C') S++;
							if (j-k >= 0 && board[i][j-k] == 'H') S = 0;
							if (j+k < board.size() && board[i][j+k] == 'C') N++;
							if (j+k < board.size() && board[i][j+k] == 'H') N = 0;
							if (j-k >= 0 && board[left][j-k] == 'C') SW++;
							if (j-k >= 0 && board[left][j-k] == 'H') SW=0;
							if (j-k >= 0 && board[right][j-k] == 'C') SE++;
							if (j-k >= 0 && board[right][j-k] == 'H') SE=0;
							if (j+k < board.size() && board[right][j+k] == 'C') NE++;
							if (j+k < board.size() && board[right][j+k] == 'H') NE=0;
							if (j+k < board.size() && board[left][j+k] == 'C') NW++;
							if (j+k < board.size() && board[left][j+k] == 'H') NW = 0;
						}
						if (N == 3 || NE == 3 || E == 3 || SE == 3 || S == 3 || SW == 3 || W == 3 || NW == 3){
							return i;
						}
					}
				}
			}
			return board.size()+1;
		}
		//this checks to see if there is a surrounding 'C' around i,j
		bool surroundingC(int i, int j, vector<vector<char> > board) {
			for (int r = -1; r < 2; ++r) {
				for (int k = -1; k < 2; ++k) {
					if (i + r >= 0 && i + r < board.size() && j + k >= 0 && j + k < board.size()){
						if (board[i+r][j+k] == 'C') return true;
					}
				}
			}
			return false;
		}
};

