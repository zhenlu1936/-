#include <ctime>
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
//#include <windows.h>
#define X 32		//窗口横宽
#define Y 20		//窗口纵宽
#define FOD 1		//食物代码
#define HED 2		//身体代码

int board[Y][X], turn[Y + 1][X], forward[5][2] = { {0,0}, {-1,0},{0,-1},{1,0},{0,1} };
int p = 0, c = 0, tick = 0, flag = 0, prefwd = 0, length = 0, head[2], end[2], endfwd, food[2];

//随机产生新食物
void new_food() {
	do {
		food[0] = rand() % Y;
		food[1] = rand() % X;
	} while (board[food[0]][food[1]]);
	board[food[0]][food[1]] = FOD;
}

//出生
void birth() {
	end[0] = head[0] = rand() % Y;
	end[1] = head[1] = rand() % X;
	endfwd = p = rand() % 4 + 1;
	board[head[0]][head[1]] = HED;
	turn[head[0]][head[1]] = p;
	length = 1;
	new_food();
}

//移动头部前进，创造一格
int moh() {
	head[0] = (head[0] + forward[p][0] + Y) % Y;
	head[1] = (head[1] + forward[p][1] + X) % X;
	if (board[head[0]][head[1]] == HED) return 1;
	board[head[0]][head[1]] = HED;
	turn[head[0]][head[1]] = p;
	prefwd = p;
	return 0;
}

//移动尾部前进，消去一格
void moe() {
	if (length == 1) endfwd = p;
	board[end[0]][end[1]] = 0;
	end[0] = (end[0] + forward[endfwd][0] + Y) % Y;
	end[1] = (end[1] + forward[endfwd][1] + X) % X;
	endfwd = turn[end[0]][end[1]];
}

//判定吃食物
void eat() {
	if (head[0] == food[0] && head[1] == food[1]) {
		length++;
		end[0] = (end[0] - forward[endfwd][0] + Y) % Y;
		end[1] = (end[1] - forward[endfwd][1] + X) % X;
		board[end[0]][end[1]] = HED;
		new_food();
	}
}

//键盘输入
int turnw() {
	char c = getch();
	if (c == 'w' && prefwd != 3) p = 1;
	if (c == 'a' && prefwd != 4) p = 2;
	if (c == 's' && prefwd != 1) p = 3;
	if (c == 'd' && prefwd != 2) p = 4;
	if (c == 'q') return 1;
	turn[head[0]][head[1]] = p;
	return 0;
}

//时钟，随时间增加难度
int do_tick() {
	if (++tick > 30 - length) {
		flag = 0;
		tick = 0;
		if (moh()) return 1;
		moe();
		eat();
	}
	return 0;
}

//给游戏上色
void frame() {
	for (int i = 1; i <= Y; i++) {
		move(i, 1);
		for (int j = 0; j < X;j++) {
			board[i - 1][j] && attron(262176 | COLOR_PAIR(board[i - 1][j]));
			printw("  ");
			attroff(262176 | COLOR_PAIR(board[i - 1][j]));
		}
	}
	move(Y + 1, 1);
	printw("Score: %d", length - 1);
	refresh();
}

//游戏主循环
void runloop() {
	while (!do_tick()) {
		usleep(10000);
		//Sleep(10);
		if (turnw()) break;
		frame();
	}
}

/*void SetConsoleWindowSize(SHORT width, SHORT height)
{
	HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT wrt = { 0, 0, width - 1, height - 1 };
	SetConsoleWindowInfo(hStdOutput, TRUE, &wrt); // 设置窗体尺寸
	COORD coord = { width, height };
	SetConsoleScreenBufferSize(hStdOutput, coord); // 设置缓冲尺寸
}*/

int main() {
	srand(time(0));
	initscr();
	start_color();
	init_pair(1, COLOR_YELLOW, 0);
	init_pair(2, COLOR_BLUE, 0);
	birth();
	resizeterm(Y + 2, X * 2 + 2);
	//SetConsoleWindowSize(Y + 2, X * 2 + 2);
	noecho();
	timeout(0);
	curs_set(0);
	box(stdscr, 0, 0);
	runloop();
	endwin();
}