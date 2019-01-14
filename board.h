#ifndef _BOARD_H
#define _BOARD_H

#include <iostream>
#include <cstring>
#include <ncurses.h>

// 작성자: 배정민(20133079) - 게임, 미션 진행 상황 기록 및 표시
class Board {
    // private
    WINDOW *score_win;
    WINDOW *mission_win;
    int body_len = 3;
    int max_body_len = 3;
    int num_eat_growth = 0;
    int num_eat_poison = 0;
    int num_use_gate = 0;
    int game_time = 0;
    int stage;

    int goal_len;
    int goal_growth;
    int goal_poison;
    int goal_gate;

public:
    bool CLEAR_LEN = false;
    bool CLEAR_GROWTH = false;
    bool CLEAR_POISON = false;
    bool CLEAR_GATE = false;

    Board(WINDOW *score_w, WINDOW *mission_w, int num);
    void printScoreBoard();
    void printMissionBoard();
    void checkSubMissionClear();
    void clearBoard();

    void set_goal_len(int goal) { goal_len=goal; }
    void set_goal_growth(int goal) { goal_growth=goal; }
    void set_goal_poison(int goal) { goal_poison=goal; }
    void set_goal_gate(int goal) { goal_gate=goal; }

    void set_body_len(int val) { body_len = val; }
    void set_max_body_len(int val) { max_body_len = val; }
    void set_num_eat_growth(int val) { num_eat_growth = val; }
    void set_num_eat_poison(int val) { num_eat_poison = val; }
    void set_num_use_gate(int val) { num_use_gate = val; }
    void set_game_time(int val) { game_time = val; }
};
#endif
