# include <iostream>
# include <unistd.h>
# include <cstdio>
# include "Strategy.h"

using namespace std;

extern "C" Point* getPoint(const int M, const int N, const int* top, const int* _board, 
	const int lastX, const int lastY, const int noX, const int noY) {

	// 不要更改这段代码
	int x = -1, y = -1;			//最终将你的落子点存到x,y中
	int** board = new int*[M];	// 二维数组
	for(int i = 0; i < M; i++) {
		board[i] = new int[N];
		for(int j = 0; j < N; j++)
			board[i][j] = _board[i * N + j];
	}
	
	UcTree uct(M, N, Point(lastX, lastY), USER, board, top);
	TreeNode::no_X = noX;
	TreeNode::no_Y = noY;
	Point best_point = uct.uctSearch();
	x = best_point.x; y = best_point.y;
	
	// 不要更改这段代码
	clearArray(M, N, board);
	return new Point(x, y);
}


/*
 *	getPoint函数返回的Point指针是在本dll模块中声明的，为避免产生堆错误，应在外部调用本dll中的
 *	函数来释放空间，而不应该在外部直接delete
 */
extern "C" void clearPoint(Point* p) {
	delete p;
	return;
}


// 清除top和board数组
void clearArray(int M, int N, int** board){
	for(int i = 0; i < M; i++) {
		delete[] board[i];
	}
	delete[] board;
}