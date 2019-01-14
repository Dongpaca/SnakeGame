#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>//sleep함수를 위해 추가
#include "snake.h"
#include "board.h"

#define STAGE_NUM 4
using namespace std;

// 작성자: 배정민(20133079) - 기본 ncurses 동작 및 그래픽 요소, stage, mission, score board 구현, gate 출현조건 추가
// 기여자: 서동하
int main()
{
	srand(time(NULL));
	initscr();      // ncurses 시작
	curs_set(0);    // 커서 숨기기
	init_color();   // 게임에 쓰일 color pair 설정

	keypad(stdscr, TRUE);//키보드 입력 가능 설정
	noecho();//문자입력 안보이게 설정

	WINDOW* win = newwin(23, 46, 0, 0); // row col y x
	WINDOW* score_win = newwin(8, 30, 0, 48);
	WINDOW* mission_win = newwin(7, 30, 10, 48);
	refresh(); // 다른 window에 그리더라도 stdscr를 refresh 해야 한다

	//시작 화면 표시
	box(win, 0, 0);
	wbkgd(win, COLOR_PAIR(BG));         // 배경색 설정
	wrefresh(win);

	int stage = 1;
	// !!game start!!
	draw_startboard(win);
	while (stage <= STAGE_NUM) {
		Board board(score_win, mission_win, stage);
		load_map(stage, board); // 맵 로드

		Snake snake(win); //snake 생성자
		snake.set_pos(start_x, start_y);
		grow_timecount = 0;
		poison_timecount = 0;
		gate_timecount = 0;
		gate_cnt = 0;
		bool IS_CLEAR = false;

		while (!snake.FLAG_GAME_END) {
			snake.snake_move();
			board.set_body_len(snake.score.body_len);
			board.set_max_body_len(snake.score.max_body_len);
			board.set_num_eat_growth(snake.score.num_eat_growth);
			board.set_num_eat_poison(snake.score.num_eat_poison);
			board.set_num_use_gate(snake.score.num_use_gate);
			board.set_game_time(snake.score.game_time);

			board.printScoreBoard();
			board.printMissionBoard();

			while (kbhit()) getch();
			usleep(snake.get_delay());

			grow_timecount += snake.get_delay() / 1e6;
			poison_timecount += snake.get_delay() / 1e6;
			if (snake.score.game_time == 10 * 4) snake.gate_item_setting();  // gate 출현조건: 10초 이후

			if (snake.score.game_time >= 10 * 4) {
				if (!snake.FLAG_GATE_USE)
					gate_timecount += snake.get_delay() / 1e6;
				else
					gate_cnt++;
			}

			if (grow_timecount == 7) {
				set_map(snake.grow_x, snake.grow_y, BG);
				snake.grow_item_setting();
				grow_timecount = 0;
			}
			if (poison_timecount == 7) {
				set_map(snake.poison_x, snake.poison_y, BG);
				snake.poison_item_setting();
				poison_timecount = 0;
			}
			if (gate_cnt == snake.length) {
				snake.FLAG_GATE_USE = false;
				set_map(snake.gate_a.x, snake.gate_a.y, WALL);
				set_map(snake.gate_b.x, snake.gate_b.y, WALL);
				snake.gate_item_setting();
				gate_timecount = 0;
				gate_cnt = 0;
			}
			if (gate_timecount == 7) {
				set_map(snake.gate_a.x, snake.gate_a.y, WALL);
				set_map(snake.gate_b.x, snake.gate_b.y, WALL);
				snake.gate_item_setting();
				gate_timecount = 0;
			}

			if (board.CLEAR_LEN && board.CLEAR_GROWTH && board.CLEAR_POISON && board.CLEAR_GATE) {
				stage++;
				board.CLEAR_LEN = false;
				board.CLEAR_GROWTH = false;
				board.CLEAR_POISON = false;
				board.CLEAR_GATE = false;
				snake.FLAG_GAME_END = true;
				IS_CLEAR = true;
				while (kbhit()) getch();
			}
		}
		board.clearBoard();
		draw_endboard(win, IS_CLEAR);
	}

	delwin(win);    // window 삭제
	endwin();       // ncurses 종료

	return 0;
}
