// ======================================================================
// FILE:        MyAI.hpp
//
// AUTHOR:      Jian Li
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#ifndef MINE_SWEEPER_CPP_SHELL_MYAI_HPP
#define MINE_SWEEPER_CPP_SHELL_MYAI_HPP

#include "Agent.hpp"
#include <iostream> // temporary use
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <stack>

using namespace std;

class MyAI : public Agent
{
public:
    MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY );

    Action getAction ( int number ) override;


    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================
	struct space {
		int x, y, count, total;
		bool isMine, isKnown, isSafe;
		float prob;
		space() : x(-1), y(-1), count(0), total(-1), isMine(false), isKnown(false), isSafe(false), prob(-1) {}
		space(int x, int y) : x(x), y(y), count(0), total(-1), isMine(false), isKnown(false), isSafe(false), prob(-1) {}
	
		bool operator==(space target) {
			return x == target.x && y == target.y;
		}

		std::vector<space> getNeighbors(int rowDimension, int colDimension, space **Board) {
			std::vector<space> result;		

			for (int ndY = y - 1; ndY <= y + 1; ndY++) {
                		if (ndY < 0 || ndY >= rowDimension) {
                        		continue;
                		}

				for (int ndX = x - 1; ndX <= x + 1; ndX++) {
					if (ndX < 0 || ndX >= colDimension) {
						continue;
					} else if (ndX == x && ndY == y) {
						continue;
					} else {
						result.push_back(Board[ndY][ndX]);
					}
				}
			}

			return result;		
 		}
	};

	space **Board;

	std::deque<space> safeSpaces;
	std::vector<space> moves;
	std::stack<space> flags;
	std::vector<space> flagged;
	std::deque<space *> unresolved;
	std::stack<space> mines;

	struct compareNum {
		bool operator() (const space& l, const space& r) {
			return l.prob < r.prob;
		}
	};

	// pushes all non-mine tiles around safe tile into safeSpaces queue
	void safeTile(space safe);

	// looks for unknown tiles around tile and, if they add up to total - count, returns true
	bool checkUnknown(space tile, std::vector<space> *unknown);

	// increments counts of all tiles around mine
	void mineFound(space mine);

	// finds the overlapping mines between 2 tiles and returns vector of mines.
	std::vector<space> findOverlap(space tile1, space tile2);

	// checks using 1-1 and similar to determine if a safe tile can be discovered
	std::vector<space> checkOneOne(space tile1, space tile2);

	// checks using 1-2 and similar to determine if a mine can be flagged
	std::vector<space> checkOneTwo(space tile1, space tile2);

	// calculates probabilities of mines in a priority queue and returns max prob
	// if a mine is determined to be 100%, short circuit returns that mine.
	space findMine();

	// makes a safer guess using only edge tiles surrounded by unknown tiles.
	bool edgeGuess(space *result);

	// makes a safer guess out of all tiles that are only surrounded by unknown tiles.
	bool safeGuess(space *result);

	// checks if tile is a safer guess or not (only surrounded by unknown tiles.
	bool isGuess(space tile);

	// makes a random guess from all tiles, only used when there are very few tiles unknown.
	bool randomGuess(space *result);

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
