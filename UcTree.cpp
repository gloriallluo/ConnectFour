# include "UcTree.h"


// -------------------- TreeNode -------------------- //

int TreeNode::no_X = -2;
int TreeNode::no_Y = -2;
int TreeNode::len_M = 0;
int TreeNode::len_N = 0;

TreeNode::TreeNode(): Q(0), N(1), children_num(0), 
    parent(nullptr), top_index(0) {}

TreeNode::TreeNode(Point p, int _player, int **_board, const int *_top)
    : pos(p), player(_player), Q(0), N(1), children_num(0), 
      parent(nullptr), top_index(0), status(STATUS_NOT_CAL)
    /* 树结点的构造函数，所含信息：棋盘状态，落点，下一回合的动作合集 */ {
    for (int i = 0; i < len_N; i++) top[i] = _top[i];
    top[pos.y] = pos.x;
    if (pos.x - 1 == no_X && pos.y == no_Y) top[pos.y] = pos.x - 1 > 0? pos.x - 1 : 0;
    for (int i = 0; i < len_M; i++) {
        for (int j = 0; j < len_N; j++)
            state[i][j] = _board[i][j];
    }
    if (pos.x >= 0 && pos.x < len_M && pos.y >= 0 && pos.y < len_N)
        state[pos.x][pos.y] = player;
}

TreeNode::TreeNode(Point p, int _player, int _board[][K], const int *_top)
    : pos(p), player(_player), Q(0), N(1), children_num(0), 
      parent(nullptr), top_index(0), status(STATUS_NOT_CAL)
    /* 树结点的构造函数，所含信息：棋盘状态，落点，下一回合的动作合集 */ {
    for (int i = 0; i < len_N; i++) top[i] = _top[i];
    top[pos.y] = pos.x;
    if (pos.x - 1 == no_X && pos.y == no_Y) top[pos.y] = pos.x - 1 > 0? pos.x - 1 : 0;
    for (int i = 0; i < len_M; i++) {
        for (int j = 0; j < len_N; j++)
            state[i][j] = _board[i][j];
    }
    if (pos.x >= 0 && pos.x < len_M && pos.y >= 0 && pos.y < len_N)
        state[pos.x][pos.y] = player;
}

TreeNode::~TreeNode() {
    for (int i = 0; i < children_num; i++) {
        if (children[i]) delete children[i];
        children[i] = nullptr;
    }
}

bool TreeNode::isTerminal() {
    if (status != STATUS_NOT_CAL) return status;
    if (pos.x < 0 || pos.x >= len_M) return false;
    if (pos.y < 0 || pos.y >= len_N) return false;
    if (player == USER && 
        userWin(pos.x, pos.y, len_M, len_N, state))
        status = STATUS_WIN;
    else if (player == MACHINE && 
        machineWin(pos.x, pos.y, len_M, len_N, state))
        status = STATUS_WIN;
    else if (isTie(len_N, top))
        status = STATUS_TIE;
    else status = STATUS_NOT_TERMINAL;
    // printf("exit isEnd() -> return false\n");
    return status & 1;
}

TreeNode* TreeNode::bestChild(bool is_root = false)
/* 综合胜率和尝试次数，选出表现最佳的孩子 */ {
    TreeNode *best_child = nullptr;
    double max_val = (double)(0 - 0xfffffff);

    for (int i = 0; i < children_num; i++)
        if (children[i]->detectGoldenChance())
            return children[i];

    for (int i = 0; i < children_num; i++) {
        double a = (double)children[i]->Q / (double)children[i]->N;
        double b = C * sqrt(2 * log(this->N + 1.001) / children[i]->N);
        int er = children[i]->detectDeathTrap();
        if (er == -1) return children[i];
        double c = (double)er / (double)log(N + 1.001) * 0.1;
        double val = a + b + c;
        if (val > max_val) {
            best_child = children[i];
            max_val = val;
        } else if (val == max_val && random() % 2) {
            best_child = children[i];
        }
    }
    return best_child;
}

void TreeNode::backup(int delta)
/* 将所有祖先的收益Q和尝试次数N进行修正 */ {
    TreeNode *p = this;
    do {
        p->Q += delta;
        p->N += 1;
        delta = 0 - delta;
        if (abs(delta) > 1) {
            int abs_delta = abs(delta) / 10;
            if (abs_delta < 1) abs_delta = 1;
            delta = delta > 0? abs_delta: 0 - abs_delta;
        }
        p = p->parent;
    } while (p);
}

TreeNode* TreeNode::expand()
/* 从该结点扩展出一个未访问过的状态，返回子结点
 * 返回nullptr表示已经不可扩展 */ {
    TreeNode *new_child = nullptr;
    while (top[top_index] == 0 && top_index < len_N)
        top_index++;    // top[index]是一个合法的后继
    if (top_index >= len_N) return new_child;
    Point new_pos = Point(top[top_index] - 1, top_index); top_index++;
    new_child = new TreeNode(new_pos, 3 - player, state, top);
    children[children_num++] = new_child;
    new_child->parent = this;
    return new_child;
}

int TreeNode::randomExpand()
/* 从该点开始，随机尝试直到决出胜负 */ {
    if (isTerminal()) {
        if (status == STATUS_WIN) return D;
        if (status == STATUS_LOSE) return 0 - D;
        else return 0;
    }
    int s[K][K];    // 拷贝的state数组
    for (int i = 0; i < len_M; i++)
        for (int j = 0; j < len_N; j++)
            s[i][j] = state[i][j];
    int t[K];       // 拷贝的top数组
    for (int i = 0; i < len_N; i++)
        t[i] = top[i];
    int x = pos.x; int y = pos.y;
    int identity = player; int delta = 0;
    
    while (true) {
        int i;
        do { i = random() % len_N; } while (t[i] == 0);
        x = t[i] - 1; y = i; identity = 3 - identity;
        s[x][y] = identity;
        t[y] = x;
        if (x > 0 && x - 1 == no_X && y == no_Y) t[y]--;
        if (identity == USER && userWin(x, y, len_M, len_N, s)) {
            delta = player == USER? 1: -1;
            break;
        } else if (identity == MACHINE && machineWin(x, y, len_M, len_N, s)) {
            delta = player == MACHINE? 1: -1;
            break;
        } else if (isTie(len_N, t)) {
            delta = 0; break;
        }
    }
    return delta;
}

// --------------------- UcTree --------------------- //

UcTree::UcTree() {}

UcTree::UcTree(const int M, const int N, Point p, int player, int **board, const int *top)
    : M(M), N(N) /* 构造UCT的根结点 */ {
    TreeNode::len_M = M;
    TreeNode::len_N = N;
    root = new TreeNode(p, player, board, top);
}

UcTree::~UcTree() {
    if (root) delete root;
}

TreeNode* UcTree::treePolicy() {
    TreeNode *p = root;
    while(p && !p->isTerminal()) {
        TreeNode *q = p->expand();
        if (q) {
            return q;
        } else {
            p = p->bestChild();
        }
    }
    return p;
}

Point UcTree::uctSearch() {
    clock_t start_time = clock();
    int cnt = 0;
    while ((double)(clock() - start_time) / (double)CLOCKS_PER_SEC < TIME_LIMIT) {
        cnt++;
        TreeNode *v = treePolicy();
        int delta = v->randomExpand();
        v->backup(delta);
    }
    TreeNode *best_child = root->bestChild(true);
    return best_child->pos;
}

// -------------------- TreeNode -------------------- //

void TreeNode::printState() {
    for (int i = 0; i < len_M; i++) {
        for (int j = 0; j < len_N; j++)
            cerr << state[i][j] << " ";
        cerr << "\n";
    }
}

int TreeNode::detectDeathTrap() {
    if (pos.x < 0 || pos.x >= len_M || 
        pos.y < 0 || pos.y >= len_N) return false;
    int count_all = 0;
    // 竖向
    int count = 0;
    for (int i = pos.x - 1; i >= 0; i--) {
        if (state[i][pos.y] == 3 - player) count++;
        else break;
    }
    for (int i = pos.x + 1; i < len_M; i++) {
        if (state[i][pos.y] == 3 - player) count++;
        else break;
    }
    count_all += count;
    if (count >= 3) return -1;
    
    // 横向
    count = 0;
    for (int i = pos.y - 1; i >= 0; i--) {
        if (state[pos.x][i] == 3 - player) count++;
        else break;
    }
    for (int i = pos.y + 1; i < len_N; i++) {
        if (state[pos.x][i] == 3 - player) count++;
        else break;
    }
    count_all += count;
    if (count >= 3) return -1;

    int i, j;
    // 斜向 左上-右下
    count = 0;
    for (i = pos.x - 1, j = pos.y - 1; i >= 0 && j >= 0; i--, j--) {
        if (state[i][j] == 3 - player) count++;
        else break;
    }
    for (i = pos.x + 1, j = pos.y + 1; i < len_M && j < len_N; i++, j++) {
        if (state[i][j] == 3 - player) count++;
        else break;
    }
    count_all += count;
    if (count >= 3) return -1;

    // 斜向 右上-左下
    count = 0;
    for (i = pos.x + 1, j = pos.y - 1; i < len_M && j >= 0; i++, j--) {
        if (state[i][j] == 3 - player) count++;
        else break;
    }
    for (i = pos.x - 1, j = pos.y + 1; i >= 0 && j < len_N; i--, j++) {
        if (state[i][j] == 3 - player) count++;
        else break;
    }
    count_all += count;
    if (count >= 3) return -1;

    return count_all;
}

int TreeNode::detectGoldenChance() {
    if (pos.x < 0 || pos.x >= len_M || 
        pos.y < 0 || pos.y >= len_N) return 0;
    // 竖向
    int count = 0;
    for (int i = pos.x - 1; i >= 0; i--) {
        if (state[i][pos.y] == player) count++;
        else break;
    }
    for (int i = pos.x + 1; i < len_M; i++) {
        if (state[i][pos.y] == player) count++;
        else break;
    }
    if (count >= 3) return 1;
    
    // 横向
    count = 0;
    for (int i = pos.y - 1; i >= 0; i--) {
        if (state[pos.x][i] == player) count++;
        else break;
    }
    for (int i = pos.y + 1; i < len_N; i++) {
        if (state[pos.x][i] == player) count++;
        else break;
    }
    if (count >= 3) return 1;

    int i, j;
    // 斜向 左上-右下
    count = 0;
    for (i = pos.x - 1, j = pos.y - 1; i >= 0 && j >= 0; i--, j--) {
        if (state[i][j] == player) count++;
        else break;
    }
    for (i = pos.x + 1, j = pos.y + 1; i < len_M && j < len_N; i++, j++) {
        if (state[i][j] == player) count++;
        else break;
    }
    if (count >= 3) return 1;

    // 斜向 右上-左下
    count = 0;
    for (i = pos.x + 1, j = pos.y - 1; i < len_M && j >= 0; i++, j--) {
        if (state[i][j] == player) count++;
        else break;
    }
    for (i = pos.x - 1, j = pos.y + 1; i >= 0 && j < len_N; i--, j++) {
        if (state[i][j] == player) count++;
        else break;
    }
    if (count >= 3) return 1;

    return 0;
}