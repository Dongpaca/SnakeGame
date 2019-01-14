#include <iostream>
#include <ncurses.h>
#include <cstring>
#include "board.h"
#define TEXT 11
#define CHECK 12

// 작성자: 배정민(20133079) - 생성자
Board::Board(WINDOW *score_w, WINDOW *mission_w, int num): stage(num){
    score_win = score_w;
    mission_win = mission_w;
    init_pair(TEXT, COLOR_WHITE, COLOR_BLACK);
    init_pair(CHECK, COLOR_YELLOW, COLOR_BLACK);  // 완료된 미션 체크용
}

// 작성자: 배정민(20133079) - 스코어 보드 출력
void Board::printScoreBoard(){
    werase(score_win);

    wattron(score_win, COLOR_PAIR(TEXT));
    box(score_win, 0, 0);
    wattroff(score_win, COLOR_PAIR(TEXT));

    mvwprintw(score_win, 1, 10, "<Stage   >");
    mvwaddch(score_win, 1, 17, stage + '0');  // int to ascii char
    mvwprintw(score_win, 2, 1, "Score Board    Playtime:");
    mvwprintw(score_win, 2, 25, std::to_string(game_time/4).c_str());

    mvwprintw(score_win, 3, 1, "Body  :");
    mvwprintw(score_win, 3, 9, (std::to_string(body_len) + " / " + std::to_string(max_body_len)).c_str());
    mvwprintw(score_win, 4, 1, "Growth:");
    mvwprintw(score_win, 4, 9, std::to_string(num_eat_growth).c_str());
    mvwprintw(score_win, 5, 1, "Poison:");
    mvwprintw(score_win, 5, 9, std::to_string(num_eat_poison).c_str());
    mvwprintw(score_win, 6, 1, "Gate  :");
    mvwprintw(score_win, 6, 9, std::to_string(num_use_gate).c_str());

    wrefresh(score_win);
}

// 작성자: 배정민(20133079) - 세부 미션 클리어 여부 판정 및 미션 보드에 표시
void Board::checkSubMissionClear(){
    if ((!CLEAR_LEN) && max_body_len >= goal_len) CLEAR_LEN = true;
    if ((!CLEAR_GROWTH) && num_eat_growth>= goal_growth) CLEAR_GROWTH = true;
    if ((!CLEAR_POISON) && num_eat_poison >= goal_poison) CLEAR_POISON = true;
    if ((!CLEAR_GATE) && num_use_gate >= goal_gate) CLEAR_GATE = true;

    char ch;
    wattron(mission_win, COLOR_PAIR(CHECK));
    ch = (CLEAR_LEN)? 'v':' ';
    mvwaddch(mission_win, 2, 13, ch);
    ch = (CLEAR_GROWTH)? 'v':' ';
    mvwaddch(mission_win, 3, 13, ch);
    ch = (CLEAR_POISON)? 'v':' ';
    mvwaddch(mission_win, 4, 13, ch);
    ch = (CLEAR_GATE)? 'v':' ';
    mvwaddch(mission_win, 5, 13, ch);
    wattroff(mission_win, COLOR_PAIR(CHECK));
}

// 작성자: 배정민(20133079) - 미션 보드 출력
void Board::printMissionBoard(){
    werase(mission_win);

    wattron(mission_win, COLOR_PAIR(TEXT));
    box(mission_win, 0, 0);
    wattroff(mission_win, COLOR_PAIR(TEXT));

    mvwprintw(mission_win, 1, 1, "Mission");
    mvwprintw(mission_win, 2, 1, "Body  :    ( )");
    mvwprintw(mission_win, 2, 9, std::to_string(goal_len).c_str());
    mvwprintw(mission_win, 3, 1, "Growth:    ( )");
    mvwprintw(mission_win, 3, 9, std::to_string(goal_growth).c_str());
    mvwprintw(mission_win, 4, 1, "Poison:    ( )");
    mvwprintw(mission_win, 4, 9, std::to_string(goal_poison).c_str());
    mvwprintw(mission_win, 5, 1, "Gate  :    ( )");
    mvwprintw(mission_win, 5, 9, std::to_string(goal_gate).c_str());

    checkSubMissionClear();
    wrefresh(mission_win);
}

// 작성자: 배정민(20133079) - 보드 지우기
void Board::clearBoard(){
    werase(score_win);
    werase(mission_win);
    wrefresh(score_win);
    wrefresh(mission_win);
}
