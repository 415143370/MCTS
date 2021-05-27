# MCTS
### Description
This repository contains different variations of the Monte Carlo Tree Search algorithm (MCTS) in C++17. Currently it is only implemented for the game Omega
but can be used for any other 2 player board games. The graphical interface is implemented in QT.

![](gameplay.gif)

### Game rules
Omega was born as an experiment on complexity and intuitive arithmetic, making a cross between Hex and Go. The game is played by
2 or more players, each trying to create groups of their colors by placing stones in a hexagonal grid, in order to score the most points, but 
each player places stones of all colors in each turn. The scores for each player is calculated by multiplying the sizes of disconnected groups 
of their respective colors. 

### Requirements
Download QT Creator https://www.qt.io/download-open-source. You need C++17 option enabled in your project file to complile.

### Implementation details
* Heavy use of C++ templates over virtual functions to maximize speed.
* UCT-2 [2] and RAVE [3] for exploration startegies.
* transposition table
* Node recycling [4] and transposition table replacement scheme. This implementation of node recycling is tailored for transpositions by storing the leaf nodes in the fifo as well.
* Move-Average Sampling Technique (MAST) simulation policy.
* Dynamic (parabolic) time allocation with early termination (when the best action can not change within the remaining time). The parabolic profile enables uneven time distribution (E.g. giving more budget on middle-game actions)
* There is no game specific knowledge incorporated.
* Did not compare the different variations but RAVE with OneDepthVNew replacement scheme seems to be the best. It is difficult to beat on board size smaller than 6.

### Acknowledgements
[1] https://www.redblobgames.com/grids/hexagons/

[2] Childs, B. E., Brodeur, J. H., & Kocsis, L. (2008, December). Transpositions and move groups in Monte Carlo tree search. In 2008 IEEE Symposium On Computational Intelligence and Games (pp. 389-395). IEEE.

[3] Gelly, S., & Silver, D. (2011). Monte-Carlo tree search and rapid action value estimation in computer Go. Artificial Intelligence, 175(11), 1856-1875.

[4] Powley, E., Cowling, P., & Whitehouse, D. (2017, September). Memory bounded monte carlo tree search. In Proceedings of the AAAI Conference on Artificial Intelligence and Interactive Digital Entertainment (Vol. 13, No. 1).
