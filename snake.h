#ifndef _SNAKE_H
#define _SNAKE_H

#include <iostream>
#include <cstdlib>
#include <ncurses.h>
#include "board.h"

// 기여자: 서동하(20153034) - 좌표 표시를 위한 구조체
// 기여자: 배정민(20133079) - 좌표 표시를 위한 구조체
struct Position {
    int x;
    int y;
};
// 기여자: 서동하(20153034) - snake 점수표시를 위한 구조체
// 기여자: 배정민(20133079) - snake 점수표시를 위한 구조체
struct Score {
    int body_len;
    int max_body_len;
    int num_eat_growth;
    int num_eat_poison;
    int num_use_gate;
    int game_time;
};
// 작성자: 서동하(20153034) - snake 기본 이동 및 아이템별 이동변화, gate 구현, snake 생성자, 현재 score 표시
// 기여자: 배정민(20133079) - 미션, 게임 종료 관련 FLAG 정의, 일부 멤버함수 구현 및 기여 (snake.cpp 참고)
class Snake {
    // private
    WINDOW* cur_win;
    Score cur_score;
    Position cur_pos;
    unsigned int direction;
    int max_body_len;
    int x, y;//head
    int DELAY;//���� �ӵ� ����
    bool start_draw_check;
    int body_x[200], body_y[200];//���� �迭 ũ�� �̸� ����
    unsigned int input_key;

    int current_length;//���� ���ھ�

public:
    Snake(WINDOW* win);
    bool FLAG_MISSION_CLEAR;
    bool FLAG_GAME_END;
    bool FLAG_GATE_USE;
    int length;
    Score score;
    Position gate_a, gate_b;
    int grow_x, grow_y;//grow_item
    int poison_x, poison_y;//grow_item
    void grow_item_setting();
    void poison_item_setting();
    int get_delay();
    void set_pos(int xpos, int ypos);
    void snake_move();
    void move_snake_arr();
    void die_check();
    void check_body_arr();
    void start_snake_arr(unsigned input_key);
    void gate_item_setting();//게이트 아이템 세팅
    void gate_move_arr(Position gate_out);//게이트 통과시 배열 이동함수
    void input_key_set(int n);//인풋값을 int로 치환
    void print_score_board();
};

extern Position offset[4];
typedef enum _DIRECTION { UP, DOWN, LEFT, RIGHT }DIRECTION;
typedef enum _COLORSET { BG = 1, WALL, IWALL, HEAD, BODY, GROWTH, POISON, GATE }COLORSET;
extern int start_x, start_y;
extern const int HEIGHT;
extern const int WIDTH;
extern int MAP[21][21];
extern float grow_timecount;
extern float poison_timecount;
extern float gate_timecount;
extern int gate_cnt;
void draw_endboard(WINDOW* win, bool IS_CLEAR);
void check_map_arr(WINDOW* win);
void init_color();
void load_map(int stage_num, Board& board);
void draw(WINDOW* win);
void draw_startboard(WINDOW* win);
void set_map(int x, int y, int num);
bool kbhit(void);
#endif
