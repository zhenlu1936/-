#include <ctime>
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
//#include <windows.h>
#define X 32		//���ں��
#define Y 20		//�����ݿ�
#define FOD 1		//ʳ�����
#define HED 2		//�������

int board[Y][X], turn[Y + 1][X], forward[5][2] = { {0,0}, {-1,0},{0,-1},{1,0},{0,1} };
int p = 0, c = 0, tick = 0, flag = 0, prefwd = 0, length = 0, head[2], end[2], endfwd, food[2];

//���������ʳ��
void new_food() {
	do {
		food[0] = rand() % Y;
		food[1] = rand() % X;
	} while (board[food[0]][food[1]]);
	board[food[0]][food[1]] = FOD;
}

//����
void birth() {
	end[0] = head[0] = rand() % Y;
	end[1] = head[1] = rand() % X;
	endfwd = p = rand() % 4 + 1;
	board[head[0]][head[1]] = HED;
	turn[head[0]][head[1]] = p;
	length = 1;
	new_food();
}

//�ƶ�ͷ��ǰ��������һ��
int moh() {
	head[0] = (head[0] + forward[p][0] + Y) % Y;
	head[1] = (head[1] + forward[p][1] + X) % X;
	if (board[head[0]][head[1]] == HED) return 1;
	board[head[0]][head[1]] = HED;
	turn[head[0]][head[1]] = p;
	prefwd = p;
	return 0;
}

//�ƶ�β��ǰ������ȥһ��
void moe() {
	if (length == 1) endfwd = p;
	board[end[0]][end[1]] = 0;
	end[0] = (end[0] + forward[endfwd][0] + Y) % Y;
	end[1] = (end[1] + forward[endfwd][1] + X) % X;
	endfwd = turn[end[0]][end[1]];
}

//�ж���ʳ��
void eat() {
	if (head[0] == food[0] && head[1] == food[1]) {
		length++;
		end[0] = (end[0] - forward[endfwd][0] + Y) % Y;
		end[1] = (end[1] - forward[endfwd][1] + X) % X;
		board[end[0]][end[1]] = HED;
		new_food();
	}
}

//��������
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

//ʱ�ӣ���ʱ�������Ѷ�
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

//����Ϸ��ɫ
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

//��Ϸ��ѭ��
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
	SetConsoleWindowInfo(hStdOutput, TRUE, &wrt); // ���ô���ߴ�
	COORD coord = { width, height };
	SetConsoleScreenBufferSize(hStdOutput, coord); // ���û���ߴ�
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