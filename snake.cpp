#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ncurses.h>
#include <fstream>
#include "snake.h"
using namespace std;
int start_x, start_y;//시작위치(stage.txt에서 읽어옴)
Position offset[4] = { {0,-1},{1,0},{0,1},{-1,0} };//gate를 나갈때 방향 위한 방향배열 선언
const int HEIGHT = 21;//맵크기(높이)
const int WIDTH = 21;//맵크기(폭)
int MAP[HEIGHT][WIDTH];
bool FLAG_GAME_END;//게임종료 flag
float grow_timecount;//grow item 출현시간 변수
float poison_timecount;//poison item 출현시간 변수
float gate_timecount;//gate item 출현시간 변수
int gate_cnt;//gate 사용 중 재배치 방지 변수

Snake::Snake(WINDOW* win) { // 생성자 외부 정의
	this->cur_win = win;
	DELAY = 250000;//게임 속도 0.5초
	start_draw_check = 0;
	memset(body_x, 0, sizeof(body_x));
	memset(body_y, 0, sizeof(body_y));
	memset(&score, 0, sizeof(Score));
	input_key = KEY_RIGHT;//처음 시작 입력 오른쪽
	direction = KEY_RIGHT;//처음 시작 오른쪽
	length = 3;//초기 몸통 길이
	current_length = 0;
	FLAG_GAME_END = false;
	FLAG_GATE_USE = false;
	grow_item_setting();
	poison_item_setting();
	//gate_item_setting();
}

// 작성자: 배정민(20133079) - snake 초기 위치 오류 제거를 위해 생성자에서 따로 분리시킴
void Snake::set_pos(int xpos, int ypos) {
	cur_pos.x = xpos;
	cur_pos.y = ypos;
}

// 작성자 : 서동하(20153034) - 처음 방향키 입력에 맞춰 snake 배치, 반대 방향 입력시 실패, 제한시간까지 입력 없을 시 이전 방향값 유지, 이후 게임 시간, 게임화면 출력
// 기여자: 배정민(20133079) - 방향키 외 입력 무시
void Snake::snake_move() {
	if (!(start_draw_check)) {
		while (!kbhit()) {}
		input_key = getch();
		if (input_key != KEY_UP && input_key != KEY_DOWN && input_key != KEY_LEFT && input_key != KEY_RIGHT)
			input_key = direction;
		direction = input_key;
		start_snake_arr(input_key);
		set_map(body_x[0], body_y[0], HEAD);
		for (int i = 1; i < length; i++) {
			set_map(body_x[i], body_y[i], BODY);
		}
		start_draw_check = 1;
		draw(cur_win);
		return;
	}
	if (kbhit()) {
		input_key = getch();
		if (input_key != KEY_UP && input_key != KEY_DOWN && input_key != KEY_LEFT && input_key != KEY_RIGHT)
			input_key = direction;

		if ((direction == KEY_UP && input_key == KEY_DOWN) || (direction == KEY_DOWN && input_key == KEY_UP) || (direction == KEY_LEFT && input_key == KEY_RIGHT) || (direction == KEY_RIGHT && input_key == KEY_LEFT)) {
			FLAG_GAME_END = 1;
			return;
		}
		else {
			switch (input_key) {//방향 이동 head의 좌표를 이동
			case KEY_UP:
				cur_pos.y--;
				break;
			case KEY_DOWN:
				cur_pos.y++;
				break;
			case KEY_RIGHT:
				cur_pos.x++;
				break;
			case KEY_LEFT:
				cur_pos.x--;
				break;
			default:
				input_key = direction;
				return;
				break;
			}
		}
	}
	else {
		switch (input_key) {//방향 이동 head의 좌표를 이동, 버퍼에 입력이 없을
		case KEY_UP:
			cur_pos.y--;
			break;
		case KEY_DOWN:
			cur_pos.y++;
			break;
		case KEY_RIGHT:
			cur_pos.x++;
			break;
		case KEY_LEFT:
			cur_pos.x--;
			break;
		default:
			break;
		}
	}

	move_snake_arr();//snake 이동
	die_check();//이동된 snake가 죽는 조건인가 확인
	set_map(body_x[0], body_y[0], HEAD);//snake 머리 출력(set_map은 map 배열을 수정하기때문에 이동된 head 먼저 검사 후 map에 추가)
	//check_map_arr(cur_win);
	//check_body_arr();
	//print_score_board();  // 5단계에 쓰일 임시 score 표시
	score.body_len = length;
	if (score.max_body_len < score.body_len)
		score.max_body_len = score.body_len;
	score.game_time++;
	draw(cur_win);
	direction = input_key;
}
// 작성자 : 서동하(20153034) - 아이템, 게이트 획득 시에 따른 snake 배열의 변화
void Snake::move_snake_arr() {
	if (cur_pos.x == grow_x && cur_pos.y == grow_y) {
		grow_timecount = 0;
		grow_item_setting();
		length++;
		score.num_eat_growth++;
		for (int i = length - 1; i > 0; i--) {//몸통좌표 하나씩 뒤로 전달
			body_x[i] = body_x[i - 1];
			body_y[i] = body_y[i - 1];
		}
		for (int i = 1; i < length; i++) {//머리를 제외한 몸통 그리기(꼬리까지)
			set_map(body_x[i], body_y[i], BODY);
		}
	}
	else if (cur_pos.x == poison_x && cur_pos.y == poison_y) {
		poison_timecount = 0;
		poison_item_setting();//item 재생성
		for (int i = length; i > 0; i--) {//몸통좌표 하나씩 뒤로 전달
			body_x[i] = body_x[i - 1];
			body_y[i] = body_y[i - 1];
		}
		score.num_eat_poison++;
		length--;//몸통 길이 줄이기
		//draw_score();//점수 표시
		for (int i = 1; i < length - 1; i++) {//머리를 제외한 몸통 그리기(꼬리까지)
			set_map(body_x[i], body_y[i], BODY);
		}
		set_map(body_x[length], body_y[length], BG);
		set_map(body_x[length + 1], body_y[length + 1], BG);
	}
	else if (cur_pos.x == gate_a.x && cur_pos.y == gate_a.y) {
		FLAG_GATE_USE = TRUE;
		gate_move_arr(gate_b);
		score.num_use_gate++;
		set_map(body_x[length - 1], body_y[length - 1], BG);//꼬리 지우기(앞으로 이동하기 때문에)
		for (int i = length - 1; 0 < i; i--) {//뒤에서 앞으로 값 전달
			body_x[i] = body_x[i - 1];
			body_y[i] = body_y[i - 1];
		}

		for (int i = 1; i < length; i++) {
			set_map(body_x[i], body_y[i], BODY);
		}
	}
	else if (cur_pos.x == gate_b.x && cur_pos.y == gate_b.y) {
		FLAG_GATE_USE = TRUE;
		gate_move_arr(gate_a);
		score.num_use_gate++;
		set_map(body_x[length - 1], body_y[length - 1], BG);//꼬리 지우기(앞으로 이동하기 때문에)
		for (int i = length - 1; 0 < i; i--) {//뒤에서 앞으로 값 전달
			body_x[i] = body_x[i - 1];
			body_y[i] = body_y[i - 1];
		}

		for (int i = 1; i < length; i++) {
			set_map(body_x[i], body_y[i], BODY);
		}
	}
	else {
		set_map(body_x[length - 1], body_y[length - 1], BG);//꼬리 지우기(앞으로 이동하기 때문에)
		for (int i = length - 1; 0 < i; i--) {//뒤에서 앞으로 값 전달
			body_x[i] = body_x[i - 1];
			body_y[i] = body_y[i - 1];
		}
		for (int i = 1; i < length; i++) {
			set_map(body_x[i], body_y[i], BODY);
		}
	}
	body_x[0] = cur_pos.x;
	body_y[0] = cur_pos.y;
}
// 작성자 : 서동하(20153034) - gate 획득시에 방향 우선순위를 고려한 HEAD 이동
void Snake::gate_move_arr(Position gate_out) {
	Position new_point;
	if (gate_out.x == 0) {
		cur_pos.x = 1;
		cur_pos.y = gate_out.y;
		input_key = KEY_RIGHT;
	}
	else if (gate_out.x == 20) {
		cur_pos.x = 19;
		cur_pos.y = gate_out.y;
		input_key = KEY_LEFT;
	}
	else if (gate_out.y == 0) {
		cur_pos.y = 1;
		cur_pos.x = gate_out.x;
		input_key = KEY_DOWN;
	}
	else if (gate_out.y == 20) {
		cur_pos.y = 19;
		cur_pos.x = gate_out.x;
		input_key = KEY_UP;
	}
	else {
		switch (direction) {//방향 이동 head의 좌표를 이동
		case KEY_UP: {
			for (int i = 0; i < 4; i++) {
				new_point.x = gate_out.x + offset[i].x;
				new_point.y = gate_out.y + offset[i].y;
				if (MAP[new_point.y][new_point.x] != 1) {
					cur_pos.y = new_point.y;
					cur_pos.x = new_point.x;
					input_key_set(i);
					return;
				}
			}
			break;
		}
		case KEY_RIGHT: {
			for (int i = 1; i < 4; i++) {
				new_point.x = gate_out.x + offset[i].x;
				new_point.y = gate_out.y + offset[i].y;
				if (MAP[new_point.y][new_point.x] != 1) {
					cur_pos.y = new_point.y;
					cur_pos.x = new_point.x;
					input_key_set(i);
					return;
				}
			}
			new_point.x = gate_out.x + offset[0].x;
			new_point.y = gate_out.y + offset[0].y;
			if (MAP[new_point.y][new_point.x] != 1) {
				cur_pos.y = new_point.y;
				cur_pos.x = new_point.x;
				input_key_set(0);
				return;
			}
			break;
		}

		case KEY_DOWN: {
			for (int i = 2; i < 4; i++) {
				new_point.x = gate_out.x + offset[i].x;
				new_point.y = gate_out.y + offset[i].y;
				if (MAP[new_point.y][new_point.x] != 1) {
					cur_pos.y = new_point.y;
					cur_pos.x = new_point.x;
					input_key_set(i);
					return;
				}
			}
			for (int i = 0; i < 2; i++) {
				new_point.x = gate_out.x + offset[i].x;
				new_point.y = gate_out.y + offset[i].y;
				if (MAP[new_point.y][new_point.x] != 1) {
					cur_pos.y = new_point.y;
					cur_pos.x = new_point.x;
					input_key_set(i);
					return;
				}
			}
		}

		case KEY_LEFT: {
			new_point.x = gate_out.x + offset[3].x;
			new_point.y = gate_out.y + offset[3].y;
			if (MAP[new_point.y][new_point.x] != 1) {
				cur_pos.y = new_point.y;
				cur_pos.x = new_point.x;
				input_key_set(3);
				return;
			}
			for (int i = 0; i < 3; i++) {
				new_point.x = gate_out.x + offset[i].x;
				new_point.y = gate_out.y + offset[i].y;
				if (MAP[new_point.y][new_point.x] != 1) {
					cur_pos.y = new_point.y;
					cur_pos.x = new_point.x;
					input_key_set(i);
					return;
				}
			}
			break;
		}

		default:
			break;
		}
	}
}
// 작성자 : 서동하(20153034) - 입력값을 정해진 정수값으로 변환
void Snake::input_key_set(int n) {
	switch (n) {
	case 0:
		input_key = KEY_UP;
		break;
	case 1:
		input_key = KEY_RIGHT;
		break;
	case 2:
		input_key = KEY_DOWN;
		break;
	case 3:
		input_key = KEY_LEFT;
		break;
	default:
		break;
	}
	return;
}
// 작성자 : 서동하(20153034) - poison item 세팅
void Snake::poison_item_setting() {
	poison_x = ((rand() % 19) + 1);//item 랜덤 탄생
	poison_y = ((rand() % 19) + 1);

	while (MAP[poison_y][poison_x] != 0) { //item 위치에 뭔가 있을 시
		poison_x = ((rand() % 19) + 1);
		poison_y = ((rand() % 19) + 1);
	}
	set_map(poison_x, poison_y, POISON);
}
// 작성자 : 서동하(20153034) - gate item 세팅
void Snake::gate_item_setting() {
	gate_a.x = (rand() % 21);
	gate_a.y = (rand() % 21);

	while (MAP[gate_a.y][gate_a.x] != 1) { //item 위치에 뭔가 있을 시
		gate_a.x = (rand() % 21);
		gate_a.y = (rand() % 21);
	}
	set_map(gate_a.x, gate_a.y, GATE);
	gate_b.x = (rand() % 21);
	gate_b.y = (rand() % 21);

	while (MAP[gate_b.y][gate_b.x] != 1) { //item 위치에 뭔가 있을 시
		gate_b.x = (rand() % 21);
		gate_b.y = (rand() % 21);
	}
	set_map(gate_b.x, gate_b.y, GATE);
}
// 작성자 : 서동하(20153034) - grow item 세팅
void Snake::grow_item_setting() {

	grow_x = ((rand() % 19) + 1);//item 랜덤 탄생
	grow_y = ((rand() % 19) + 1);
	while (MAP[grow_y][grow_x] != 0) { //item 위치에 뭔가 있을 시
		grow_x = ((rand() % 19) + 1);
		grow_y = ((rand() % 19) + 1);
	}
	set_map(grow_x, grow_y, GROWTH);
}

//입력 유무 확인함수(입력을 버퍼에 넣지 않는다) 외부 소스 출처: https://ko.coder.work/so/c%2B%2B/548510
bool kbhit(void) {
	int ch;
	bool ret;

	nodelay(stdscr, TRUE);

	ch = getch();
	if (ch == ERR) {
		ret = false;
	}
	else {
		ret = true;
		ungetch(ch); // 마지막에 받은 문자를 버퍼에 다시 넣어서 다음 getch()가 받을 수 있도록 합니다.
	}

	nodelay(stdscr, FALSE);
	return ret;
}
// 작성자 : 서동하(20153034) - private 변수인 DELAY의 값
int Snake::get_delay() {
	return this->DELAY;
}

// 작성자: 배정민(20133079) - MAP배열에 입력좌표에 NUM값 입력
void set_map(int x, int y, int num) {
	MAP[y][x] = num - 1;  // enum 값에 1을 뺀 결과가 실제 요소들 값
}

// 작성자: 배정민(20133079) - 맵데이터 불러오기
void load_map(int stage_num, Board& board) {
	int h = 0;
	string file_path = "./stages/stage" + to_string(stage_num) + ".txt";    // 특정 스테이지 로딩
	string cur_line;
	ifstream readMap(file_path);
	if (readMap.is_open()) {
		getline(readMap, cur_line);
		start_x = atoi(cur_line.c_str());
		getline(readMap, cur_line);
		start_y = atoi(cur_line.c_str());
		while (getline(readMap, cur_line)) {
			if (cur_line == "END") break;
			for (int w = 0; w < WIDTH; w++)
				MAP[h][w] = (int)cur_line[w] - '0';  // ascii to int
			h++;
		}
		// 각 stage파일에서 미션 목표값을 불러온다
		getline(readMap, cur_line);
		board.set_goal_len(atoi(cur_line.c_str()));
		getline(readMap, cur_line);
		board.set_goal_growth(atoi(cur_line.c_str()));
		getline(readMap, cur_line);
		board.set_goal_poison(atoi(cur_line.c_str()));
		getline(readMap, cur_line);
		board.set_goal_gate(atoi(cur_line.c_str()));
	}
	readMap.close();
}
// 작성자 : 서동하(20153034) - 게임 실패 조건 확인 함수 실패시 FLAG TRUE
void Snake::die_check() {
	for (int k = 1; k < length; k++) {
		if (body_x[0] == body_x[k] && body_y[0] == body_y[k]) {//head가 몸과 겹쳐졌는가?
			FLAG_GAME_END = 1;
		}
	}
	if (MAP[body_y[0]][body_x[0]] == 1 || MAP[body_y[0]][body_x[0]] == 2)
		FLAG_GAME_END = 1;
	if (length < 3)//길이가 3보다 작아졌는가?
		FLAG_GAME_END = 1;
}
// 작성자 : 서동하(20153034) -  현재 획득한 점수판 표시
void Snake::print_score_board() {
	char tmp[4];
	int len;
	mvprintw(2, 47, "CURRENT LENGTH=");
	sprintf(tmp, "%d", score.body_len);
	mvprintw(2, 65, tmp);
	mvprintw(3, 47, "MAX LENGTH=");
	sprintf(tmp, "%d", score.max_body_len);
	mvprintw(3, 65, tmp);
	mvprintw(4, 47, "GROW ITEM=");
	sprintf(tmp, "%d", score.num_eat_growth);
	mvprintw(4, 65, tmp);
	mvprintw(4, 47, "POISON ITEM=");
	sprintf(tmp, "%d", score.num_eat_poison);
	mvprintw(4, 65, tmp);
	mvprintw(5, 47, "GATE ITEM=");
	sprintf(tmp, "%d", score.num_use_gate);
	mvprintw(5, 65, tmp);
	mvprintw(6, 47, "GAME PLAYTIME=");
	sprintf(tmp, "%d", score.game_time / 4);
	mvprintw(6, 65, tmp);
	mvprintw(6, 67, "SECONDS");
}
// 작성자 : 서동하(20153034) - 현재 snake 배열 출력
void Snake::check_body_arr() {
	char tmp[4];
	int len;
	mvprintw(21, 47, "x=");
	int z = 0;
	for (int i = 0; i < length; i++) {
		len = sprintf(tmp, "%d", body_x[i]);
		mvprintw(21, 50 + i + z, tmp);
		mvprintw(21, 50 + i + len + z, " ");
		z += 2;
	}

	z = 0;
	mvprintw(22, 47, "y=");
	for (int i = 0; i < length; i++) {
		len = sprintf(tmp, "%d", body_y[i]);
		mvprintw(22, 50 + i + z, tmp);
		mvprintw(22, 50 + i + len + z, " ");
		z += 2;
	}
	sprintf(tmp, "%d", MAP[body_y[0]][body_x[0]]);
	mvprintw(22, 62, tmp);
}
// 작성자 : 서동하(20153034) - 현재 map 배열 출력
void check_map_arr(WINDOW* win) {
	char tmp[2];
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			sprintf(tmp, "%d", MAP[i][j]);
			mvprintw(i, j + 50, tmp);
		}
	}
}
// 작성자 : 서동하(20153034) - 입력 방향에 따른 초기 snake 배열 세팅
void Snake::start_snake_arr(unsigned input_key) {
	body_x[0] = cur_pos.x;
	body_y[0] = cur_pos.y;
	switch (input_key) {//방향 이동 head의 좌표를 이동
	case KEY_UP:
		for (int i = 1; i < length; i++) {
			body_x[i] = cur_pos.x;
			body_y[i] = cur_pos.y + i;
		}
		break;
	case KEY_DOWN:
		for (int i = 1; i < length; i++) {
			body_x[i] = cur_pos.x;
			body_y[i] = cur_pos.y - i;
		}
		break;
	case KEY_RIGHT:
		for (int i = 1; i < length; i++) {//초기 length는 4
			body_x[i] = cur_pos.x - i;
			body_y[i] = cur_pos.y;
		}
		break;
	case KEY_LEFT:
		for (int i = 1; i < length; i++) {//dir방향의 반대로 몸통 생성
			body_x[i] = cur_pos.x + i;//시작하자마자 죽을 수도 있으니까
			body_y[i] = cur_pos.y;
		}
		break;
	default:
		input_key = direction;//안눌렀을 때 대비
		return;
		break;
	}
}

// 작성자: 배정민(20133079)
void init_color() {
	start_color();
	init_pair(BG, COLOR_BLACK, COLOR_WHITE);
	init_pair(WALL, COLOR_BLACK, COLOR_BLACK);
	init_pair(IWALL, COLOR_WHITE, COLOR_BLACK);
	init_pair(HEAD, COLOR_CYAN, COLOR_CYAN);
	init_pair(BODY, COLOR_BLUE, COLOR_BLUE);
	init_pair(GROWTH, COLOR_GREEN, COLOR_GREEN);
	init_pair(POISON, COLOR_RED, COLOR_RED);
	init_pair(GATE, COLOR_MAGENTA, COLOR_MAGENTA);
	init_pair(9, COLOR_WHITE, COLOR_WHITE);
}

// 작성자: 배정민(20133079)
void draw(WINDOW* win) {
	char ch;
	int color;
	werase(win);    // 화면을 지워준다
	box(win, 0, 0); // win 경계선 표시
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			ch = (MAP[i][j] != 2) ? ' ' : 'X';  // immune wall은 X를 표시한다
			color = MAP[i][j] + 1;  // enum값은 실제 map 값보다 1씩 크다
			wattron(win, COLOR_PAIR(color));
			mvwaddch(win, i + 1, 2 * j + 2, ch);
			mvwaddch(win, i + 1, 2 * j + 3, ch);
			wattroff(win, COLOR_PAIR(color));
		}
	}
	wrefresh(win);
}
// 작성자 : 서동하(20153034) - 초기 게임화면 출력
void draw_startboard(WINDOW* win) {
	init_pair(9, COLOR_WHITE, COLOR_WHITE);
	wbkgd(win, COLOR_PAIR(9));
	init_pair(10, COLOR_BLACK, COLOR_WHITE);
	wattron(win, COLOR_PAIR(10) | A_BOLD | A_UNDERLINE);
	mvwprintw(win, 5, 14, "!!  SNAKE GAME !!");
	mvwprintw(win, 15, 10, " PRESS ANY KEY TO START ");
	wattroff(win, COLOR_PAIR(10) | A_BOLD | A_UNDERLINE);
	wrefresh(win);
}

// 기여자: 배정민(20133079) - 미션 클리어 여부에 따라 다른 문구 출력
void draw_endboard(WINDOW* win, bool IS_CLEAR) {
	wclear(win);
	init_pair(9, COLOR_WHITE, COLOR_WHITE);
	wbkgd(win, COLOR_PAIR(9));
	init_pair(10, COLOR_BLACK, COLOR_WHITE);
	wattron(win, COLOR_PAIR(10) | A_BOLD);
	if (IS_CLEAR) mvwprintw(win, 5, 11, "!!  MISSION SUCCESS  !!");
	else if (!IS_CLEAR) mvwprintw(win, 5, 14, "!!  GAME OVER  !!");
	mvwprintw(win, 15, 9, " PRESS ANY KEY TO CONTINUE ");
	wattroff(win, COLOR_PAIR(10) | A_BOLD);
	wrefresh(win);
	while (!getch()) {}
}
