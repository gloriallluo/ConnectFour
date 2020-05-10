# ifndef UCTREE_H_
# define UCTREE_H_

# define USER 1
# define MACHINE 2
# define TIME_LIMIT 2.6
# define C 0.7
# define STATUS_WIN 1
# define STATUS_LOSE 2
# define STATUS_TIE 3
# define STATUS_NOT_TERMINAL 0
# define STATUS_NOT_CAL -1
# define D 10000

# include <math.h>
# include <unistd.h>
# include <random>
# include <iostream>
# include "Judge.h"
# include "Point.h"

using std::cerr;
using std::cout;

struct TreeNode {
public:
    int Q, N;
    int status;
    static int no_X, no_Y;
    static int len_M, len_N;
    Point pos;
    int state[K][K];
    int top[K];
    int player;
    int top_index;
    TreeNode *parent;
    TreeNode *children[K];
    int children_num;

    TreeNode();
    TreeNode(Point p, int _player, int **_board, const int *_top);
    TreeNode(Point p, int _player, int _board[][K], const int *_top);
    ~TreeNode();
    bool isTerminal();
    TreeNode *bestChild(bool is_root);
    TreeNode *expand();
    void backup(int delta);
    int randomExpand();
    int detectDeathTrap();
    int detectGoldenChance();
    void printState();
};

struct UcTree {
    int M, N;
public:
    TreeNode *root;
    TreeNode *treePolicy();
    UcTree();
    UcTree(const int M, const int N, Point p, int player, int **board, const int *top);
    ~UcTree();
    Point uctSearch();
};

# endif