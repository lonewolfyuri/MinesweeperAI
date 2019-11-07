// ======================================================================
// FILE:        MyAI.cpp
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

#include "MyAI.hpp"

MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================

	rowDimension = _rowDimension;
	colDimension = _colDimension;
	totalMines = _totalMines;
	agentX = _agentX;
	agentY = _agentY;

	// std::cout << "X: " << agentX + 1 << " | Y: " << agentY + 1 << std::endl;

	// 
	space origin(agentX, agentY);
	origin.total = 0;
	moves.push_back(origin);
	// unresolved.push_back(origin);
	
	// Make Board
	Board = new space*[rowDimension];
	for (int rowNdx = 0; rowNdx < rowDimension; rowNdx++) {
		Board[rowNdx] = new space[colDimension];
		space *curSpace;
		for (int colNdx = 0; colNdx < colDimension; colNdx++) {
			curSpace = &Board[rowNdx][colNdx];
			if (rowNdx == origin.y && colNdx == origin.x) {
				Board[rowNdx][colNdx] = origin;
			} else {
				curSpace->x = colNdx;
				curSpace->y = rowNdx;
			}
		}
	}
	
	// std::cout << "Made" << std::endl;

    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================
};

Agent::Action MyAI::getAction( int number )
{
    // ======================================================================
    // YOUR CODE BEGINS
    // ======================================================================
		

	// std::cout << " Get Action - Num: " << number << std::endl;
	// std::cerr << "----------------------" << std::endl;

	if (number >= 0) {
		Board[agentY][agentX].total = number;
		Board[agentY][agentX].isKnown = true;
		unresolved.push_back(&Board[agentY][agentX]);
	}

	int res = 0;
	bool skip_res = false;

	while(true) {

		/*
		std::cout << "Safe Size: " << safeSpaces.size() << std::endl;
		std::cout << "Flag Size: " << flags.size() << std::endl;
		std::cout << "Unresolved Size: " << unresolved.size() << std::endl;
		*/

		if (!safeSpaces.empty()) {
			bool oldMove = false;
			space newMove = safeSpaces.front();
			for (space curSpace : moves) {
				if (curSpace.x == newMove.x && curSpace.y == newMove.y) {
					oldMove = true;
					break;
				}
			}

			// std::cout << "New Move: (" << newMove.x + 1 << "," << newMove.y + 1;
			// std::cout << ") - oldMove: " << oldMove << std::endl;

			safeSpaces.pop_front();
		
			// std::cout << "Old Moves: " << std::endl;
			// for (space curSpace : moves) {
			//         std::cout<<curSpace.x+1<<" | "<<curSpace.y+1<<std::endl;
			// }

			if (!oldMove) {
				moves.push_back(newMove);
				agentX = newMove.x;
				agentY = newMove.y;
				return {UNCOVER, newMove.x, newMove.y};
			}

		} else if (!flags.empty()) {
			space newFlag = flags.top();
			flags.pop();

			bool oldFlag = false;
			for (space curSpace : flagged) {
				if (curSpace.x == newFlag.x && curSpace.y == newFlag.y) {
					oldFlag = true;
					break;
				}
			}
				
			if (!oldFlag) {
				flagged.push_back(newFlag);
				agentX = newFlag.x;
				agentY = newFlag.y;
				return {FLAG, newFlag.x, newFlag.y};
			}
		} else if (!unresolved.empty() && !skip_res) {
			if (res > unresolved.size()) {
				skip_res = true;	
			}

			space *curPtr = unresolved.front();
			space curSpace = *curPtr;
			unresolved.pop_front();

			/*
			std::cout << "Unresolved Space - x: " << curSpace.x + 1 << " | y: ";
			std::cout << curSpace.y + 1 << " | total: " << curSpace.total;
			std::cout << " | count: " << curSpace.count << std::endl;
			*/

			bool mineFnd = false, isResolved = false;

			// Scope tile's surroundings and update tile
			if (curSpace.count == curSpace.total) {
				// if count == total tile is resolved
				curSpace.isSafe = true;
				isResolved = true;
				
				// std::cout << "resolved" << std::endl;

			} else {
				// std::cout << "not resolved" << std::endl;
				
				std::vector<space> unknown;
				bool resolve = checkUnknown(curSpace, &unknown);
			
				// std::cout << "resolve: " << resolve << std::endl;
	
				if (resolve) {
					// if # non-mine unknown tiles == total - count
					// unknown tiles are mines and curSpace is resolved
					if (!unknown.empty()) {
						// std::cout << "unknown" << std::endl;
						mineFnd = true;
					}

					for (space unkn : unknown) {
						mineFound(unkn);
						mines.push(unkn);
					}

					isResolved = true;
					curSpace.isSafe = true;
				}
			}

			if (mineFnd) {
				
				// std::cout << "Mine Size: " << mines.size() << std::endl;

				// if mine found, flag as mine
				while (!mines.empty()) {
					space curMine = mines.top();
					mines.pop();				

					Board[curMine.y][curMine.x].isMine = true;
					flags.push(curMine);

					/*
					std::cout << "Mine - x: " << curMine.x + 1 << " | y: ";
					std::cout << curMine.y + 1 << std::endl;

					std::cout << "Mine Size: " << mines.size() << std::endl;
					*/

				} 
			} 
			
			if (isResolved) {
				// if resolved, call safeTile(curSpace)
				safeTile(curSpace);
				res = 0;
			} else {
				// else call unresolved.push_back() to recycle mine
				unresolved.push_back(curPtr);
				res++;
			}
			
			continue;
		} else {
			if (skip_res) {
				skip_res = false;
			}

			space newMove = findMine();

			if (!safeSpaces.empty() || !flags.empty()) {
				continue;
			}

			int unknBoard = 0;
                        for (int rowNdx = 0; rowNdx < rowDimension; rowNdx++) {
                                for (int colNdx = 0; colNdx < colDimension; colNdx++) {
                                        if (!Board[rowNdx][colNdx].isKnown && !Board[rowNdx][colNdx].isMine) {
                                                unknBoard++;
                                        }

                                }
                        }

			/*
			if (newMove.prob > 0 && newMove.prob < ((float)(totalMines - flagged.size())) / unknBoard) {
				safeSpaces.push_back(newMove);
				continue;
			} 
			*/

			if (newMove.y >= 0 && newMove.x >= 0) {
				if (newMove.prob == 0) {
					Board[newMove.y][newMove.x].isSafe = true;
					safeSpaces.push_back(newMove);
				} else { 
					mineFound(newMove);
					Board[newMove.y][newMove.x].isMine = true;
					flags.push(newMove);
				}
				continue;
			} else if (!flags.empty()) {
				continue;
			}

			// std::cout << "Guessing..." << std::endl;
	
			// Pick Random Tiles Here

			bool valid; 

			/*
			if ((float)(totalMines - flagged.size()) / unknBoard > 0.2) {
				valid = safeGuess(&newMove);
			} else {
				valid = randomGuess(&newMove);
			}
			*/

			valid = edgeGuess(&newMove);

			if (valid) {
				moves.push_back(newMove);
				agentX = newMove.x;
				agentY = newMove.y;
				return {UNCOVER, newMove.x, newMove.y};
			} else {
				valid = safeGuess(&newMove);
			}

			if (valid) {
				moves.push_back(newMove);
				agentX = newMove.x;
				agentY = newMove.y;
				return {UNCOVER, newMove.x, newMove.y};
			} else {
				valid = randomGuess(&newMove);
			}
			
			if (valid) {
				moves.push_back(newMove);
				agentX = newMove.x;
				agentY = newMove.y;
				return {UNCOVER, newMove.x, newMove.y};
			} else {
				return {LEAVE, -1, -1};
			}
		}
	}
    // ======================================================================
    // YOUR CODE ENDS
    // ======================================================================

}


// ======================================================================
// YOUR CODE BEGINS
// ======================================================================

void MyAI::safeTile(space safe) {
	std::vector<space> neighbors = safe.getNeighbors(rowDimension, colDimension, Board);

	for (space cur : neighbors) {
		if (!cur.isMine) {
			safeSpaces.push_back(Board[cur.y][cur.x]);
		}
	}

}

bool MyAI::checkUnknown(space tile, std::vector<space> *unknown) {
        int unkn = tile.total - tile.count;

	std::vector<space> neighbors = tile.getNeighbors(rowDimension, colDimension, Board);

	for (space cur : neighbors) {
		if (!Board[cur.y][cur.x].isMine && !Board[cur.y][cur.x].isKnown) {
			unkn--;
			unknown->push_back(Board[cur.y][cur.x]);
		}
	}

	return unkn == 0;
}

void MyAI::mineFound(space mine) {
	std::vector<space> neighbors = mine.getNeighbors(rowDimension, colDimension, Board);

	for (space cur : neighbors) {
		Board[cur.y][cur.x].count++;
	}
}

std::vector<MyAI::space> MyAI::findOverlap(space tile1, space tile2) {
	std::vector<space> result, overlap;

	if (tile1.isMine || tile2.isMine || !tile1.isKnown || !tile2.isKnown) {
		return result;
	}

	if (tile1.x != tile2.x - 1 || tile1.x != tile2.x + 1 && tile1.y != tile2.y) {
		return result;
	}

	if (tile1.y != tile2.y - 1 || tile1.y != tile2.y + 1 && tile1.x != tile2.x) {
		return result;
	}

	std::vector<space> neighbors = tile1.getNeighbors(rowDimension, colDimension, Board);
	std::vector<space> nbrs2 = tile2.getNeighbors(rowDimension, colDimension, Board);	
	int unkn1 = 0, unkn2 = 0;

	for (space nbr2 : nbrs2) {
		if (!nbr2.isKnown && !nbr2.isMine) {
			unkn2++;
		}
	}

	for (space nbr : neighbors) {
		if (!nbr.isKnown && !nbr.isMine) {
			unkn1++;
			for (space nbr2 : nbrs2) {
				if (nbr == nbr2) {
					overlap.push_back(nbr);
				}
			}
		}
	}

	int sz = overlap.size(), remUnkn;
	bool notIn;

	if (unkn1 - sz > 0 && unkn1 - sz <= tile1.total - tile1.count && unkn2 - sz <= 0) {
		for (space nbr : neighbors) {
			notIn = true;
			for (space lap : overlap) {
				if (nbr == lap) {
					notIn = false;
				}
			}
			if (notIn && !nbr.isKnown && !nbr.isMine) {
				result.push_back(nbr);
			}
		}
	} else if (unkn2 - sz > 0 && unkn2 - sz <= tile2.total - tile2.count && unkn1 - sz <= 0) {
		for (space nbr : nbrs2) {
			notIn = true;
			for (space lap : overlap) {
				if (nbr == lap) {
					notIn = false;
				}
			}
			if (notIn && !nbr.isKnown && !nbr.isMine) {
				result.push_back(nbr);
			}
		}
	}

	/*
	std::cout << "Returning Result - sz: " << result.size();
	std::cout << " | t1 - x: " << tile1.x + 1<< " y: " << tile1.y + 1;
	std::cout << " | t2 - x: " << tile2.x + 1 << " y: " << tile2.y + 1 << endl;
	*/

	return result;
}

std::vector<MyAI::space> MyAI::checkOneOne(space tile1, space tile2) {
	std::vector<space> result, overlap;

        if (tile1.isMine || tile2.isMine || !tile1.isKnown || !tile2.isKnown) {
		// std::cout << "1-1 terminate - 1" << std::endl;
                return result;
        }

        if (tile1.x != tile2.x - 1 && tile1.x != tile2.x + 1 && tile1.y == tile2.y) {
		// std::cout << "1-1 terminate - 2" << std::endl;
                return result;
        } else if (tile1.y != tile2.y - 1 && tile1.y != tile2.y + 1 && tile1.x == tile2.x) {
                // std::cout << "1-1 terminate - 3" << std::endl;
		return result;
        } 

	if (tile1.total - tile1.count != tile2.total - tile2.count) {
		// std::cout << "1-1 terminate - 4" << std::endl;
		return result;
	}

        std::vector<space> neighbors = tile1.getNeighbors(rowDimension, colDimension, Board);
        std::vector<space> nbrs2 = tile2.getNeighbors(rowDimension, colDimension, Board);
        int unkn1 = 0, unkn2 = 0;

        for (space nbr2 : nbrs2) {
                if (!nbr2.isKnown && !nbr2.isMine) {
                        unkn2++;
                }
        }

        for (space nbr : neighbors) {
                if (!nbr.isKnown && !nbr.isMine) {
                        unkn1++;
                        for (space nbr2 : nbrs2) {
                                if (nbr == nbr2) {
                                        overlap.push_back(nbr);
                                }
                        }
                }
        }

	int sz = overlap.size(), remUnkn;
        bool notIn;

        if (unkn1 - sz > 0 && unkn1 - sz <= tile1.total - tile1.count && unkn2 - sz <= 0) {
                for (space nbr : neighbors) {
                        notIn = true;
                        for (space lap : overlap) {
                                if (nbr == lap) {
                                        notIn = false;
                                }
                        }
                        if (notIn && !nbr.isKnown && !nbr.isMine) {
                                result.push_back(nbr);
                        }
                }
        } else if (unkn2 - sz > 0 && unkn2 - sz <= tile2.total - tile2.count && unkn1 - sz <= 0) {
                for (space nbr : nbrs2) {
                        notIn = true;
                        for (space lap : overlap) {
                                if (nbr == lap) {
                                        notIn = false;
                                }
                        }
                        if (notIn && !nbr.isKnown && !nbr.isMine) {
                                result.push_back(nbr);
                        }
                }
        }

	/*
        std::cout << "Returning Safe - sz: " << result.size();
        std::cout << " | t1 - x: " << tile1.x + 1<< " y: " << tile1.y + 1;
        std::cout << " | t2 - x: " << tile2.x + 1 << " y: " << tile2.y + 1 << endl;
	*/
	
        return result;


}

std::vector<MyAI::space> MyAI::checkOneTwo(space tile1, space tile2) {
	std::vector<space> result, overlap;
	int unkn1 = 0, unkn2 = 0, t1 = tile1.total - tile1.count, t2 = tile2.total - tile2.count;

        if (tile1.isMine || tile2.isMine || !tile1.isKnown || !tile2.isKnown) {
		// std::cout << "1-2 terminate - 1" << std::endl;
                return result;
        }

        if (tile1.y == tile2.y && tile1.x != tile2.x - 1 && tile1.x != tile2.x + 1) {
                // std::cout << "1-2 terminate - 2" << std::endl;
		return result;
        } else if (tile1.x == tile2.x && tile1.y != tile2.y - 1 && tile1.y != tile2.y + 1) {
            	// std::cout << "1-2 terminate - 3" << std::endl;
		return result;
        }

	if (t1 != t2 + 1 && t1 != t2 - 1) {
		// std::cout << "1-2 terminate - 4" << std::endl;
		return result;
	}

        std::vector<space> neighbors = tile1.getNeighbors(rowDimension, colDimension, Board);
        std::vector<space> nbrs2 = tile2.getNeighbors(rowDimension, colDimension, Board);

        for (space nbr2 : nbrs2) {
                if (!nbr2.isKnown && !nbr2.isMine) {
                        unkn2++;
                }
        }

        for (space nbr : neighbors) {
                if (!nbr.isKnown && !nbr.isMine) {
                        unkn1++;
                        for (space nbr2 : nbrs2) {
                                if (nbr == nbr2) {
                                        overlap.push_back(nbr);
                                }
                        }
                }
        }

	int sz = overlap.size(), remUnkn;
        bool notIn;

	//if (unkn1 - sz > 0 && unkn1 - sz <= t1 && t1 > t2) {
        if (unkn1 - sz > 0 && unkn1 - sz <= t1 && unkn2 - sz <= 0) {
                for (space nbr : neighbors) {
                        notIn = true;
                        for (space lap : overlap) {
                                if (nbr == lap) {
                                        notIn = false;
                                }
                        }
                        if (notIn && !nbr.isKnown && !nbr.isMine) {
                                result.push_back(nbr);
                        }
                }
	// } else if (unkn2 - sz > 0 && unkn2 - sz <= t2 && t1 > t2) {
        } else if (unkn2 - sz > 0 && unkn2 - sz <= t2 && unkn1 - sz <= 0) {
                for (space nbr : nbrs2) {
                        notIn = true;
                        for (space lap : overlap) {
                                if (nbr == lap) {
                                        notIn = false;
                                }
                        }
                        if (notIn && !nbr.isKnown && !nbr.isMine) {
                                result.push_back(nbr);
                        }
                }
        }

	/*
        std::cout << "Returning Mine - sz: " << result.size();
        std::cout << " | t1 - x: " << tile1.x + 1<< " y: " << tile1.y + 1;
        std::cout << " | t2 - x: " << tile2.x + 1 << " y: " << tile2.y + 1 << endl;
	*/

        return result;

}

MyAI::space MyAI::findMine() {
	int sz = unresolved.size();
	// std::priority_queue<space, vector, compareNum> pq;
	std::vector<space *> targets;
	float pMax;
	space *result = nullptr;

	for (int ndx = 0; ndx < sz; ndx++) {
		space *cur = unresolved.front();
		unresolved.pop_front();

		int mineLeft = cur->total - cur->count; 
		std::vector<space> unkn;
		std::vector<space> neighbors = cur->getNeighbors(rowDimension, colDimension, Board);
		
		for (space nbr : neighbors) {
			
			// std::cout << "1-1: t1 - " << cur->x + 1 << " " << cur->y + 1;
			// std::cout << " | t2 - " << nbr.x + 1 << " " << nbr.y + 1 << std::endl;
	
			std::vector<space> newSafe = checkOneOne(*cur, nbr);
			if (newSafe.size() > 0) {
				for (space safe : newSafe) {
					safeSpaces.push_back(safe);
				}
		
				return space();
			}

			// std::cout << "1-2:" << std::endl;

			std::vector<space> newMines = checkOneTwo(*cur, nbr);
			if (newMines.size() > 0) {
				for (space mine : newMines) {
					Board[mine.y][mine.x].isMine = true;
					mineFound(mine);
					flags.push(mine);
				}

				return space();
			}

			/*
			result = findOverlap(*cur, nbr);
			if (result) {
				return *result;
			}
			*/

			if (!nbr.isMine && !nbr.isSafe && nbr.isKnown) {
				std::vector<space> nbrs = nbr.getNeighbors(rowDimension, colDimension, Board);
				for(space nbrNbr : nbrs) {
					if (!nbrNbr.isMine && !nbrNbr.isSafe && !nbrNbr.isKnown) {
						unkn.push_back(nbrNbr);
					}
				}
			} else if (!nbr.isKnown) {
				unkn.push_back(nbr);
			}
		}
		
		float unknSz = unkn.size();
		for (space un : unkn) {
			bool fnd = false;
			for (space *tar : targets) {
				if (*tar == un) {
					fnd = true;
					if ((mineLeft / unknSz) > tar->prob) {
						tar->prob = (mineLeft / unknSz);
					}
				}
			}
			if (!fnd) {
				un.prob = mineLeft / unknSz;
				targets.push_back(&un);
			}
		}

		pMax = -1;
		for (space *tar : targets) {
			if (tar->prob > pMax) {
				pMax = tar->prob;
				result = tar;
			}
		}

		unresolved.push_back(cur);
		
		if (pMax >= 0.8) {
			return *result;
		}

		result = nullptr;
	}
	
	return space();

	/*
	pMax = -1;
	for (space *tar : targets) { 
		if (tar->prob > pMax) {
			pMax = tar->prob;
			result = tar;
		}
	}

	return *result;
	*/
}

bool MyAI::edgeGuess(space *result) {
	std::vector<space> pool;

	for (int rowNdx = 0; rowNdx < rowDimension; rowNdx++) {
                for (int colNdx = 0; colNdx < colDimension; colNdx++) {
			
			/*
			if (rowNdx == colNdx == 0) {
				continue;
			}
			*/

                        if (!Board[rowNdx][colNdx].isMine && !Board[rowNdx][colNdx].isKnown) {
                               	if (rowNdx == 0 || rowNdx == rowDimension - 1 || colNdx == 0 || colNdx == colDimension - 1) { 
					if (isGuess(Board[rowNdx][colNdx])) {
                                        	pool.push_back(Board[rowNdx][colNdx]);
					}
				}
                        }
                }
        }

	int sz = pool.size();
        if (sz > 0) {
                // Random select from tiles.
                int ndx = rand() % sz;
                *result = pool[ndx];
                return true;
        } else {
                return false;
        }
}

bool MyAI::safeGuess(space *result) {
	std::vector<space> pool;

        for (int rowNdx = 0; rowNdx < rowDimension; rowNdx++) {
                for (int colNdx = 0; colNdx < colDimension; colNdx++) {
                        
			/*
			if (rowNdx == colNdx == 0) {
				continue;
			}
			*/

			if (!Board[rowNdx][colNdx].isMine && !Board[rowNdx][colNdx].isKnown) {
                                if (isGuess(Board[rowNdx][colNdx])) {
					pool.push_back(Board[rowNdx][colNdx]);
                        	}
			}
                }
        }

        int sz = pool.size();
        if (sz > 0) {
                // Random select from tiles.
                int ndx = rand() % sz;
                *result = pool[ndx];
                return true;
        } else {
                return false;
        }
}

bool MyAI::isGuess(space tile) {
	std::vector<space> neighbors = tile.getNeighbors(rowDimension, colDimension, Board);

	for (space cur : neighbors) {
		if (Board[cur.y][cur.x].isMine || Board[cur.y][cur.x].isKnown) {
			return false;
		}
	}

	return true;
}

bool MyAI::randomGuess(space *result) {
	std::vector<space> pool;

	for (int rowNdx = 0; rowNdx < rowDimension; rowNdx++) {
		for (int colNdx = 0; colNdx < colDimension; colNdx++) {
			if (!Board[rowNdx][colNdx].isMine && !Board[rowNdx][colNdx].isKnown) {
				pool.push_back(Board[rowNdx][colNdx]);
			}
		}
	}

	int sz = pool.size();
	if (sz > 0) {
		// Random select from tiles.
		int ndx = rand() % sz;
		*result = pool[ndx];
		return true;
	} else {
		return false;
	}
}

// ======================================================================
// YOUR CODE ENDS
// ======================================================================
