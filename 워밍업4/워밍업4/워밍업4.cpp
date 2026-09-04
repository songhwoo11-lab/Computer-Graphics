#include<iostream>
#include<vector>
#include<random>
#include<windows.h>

std::random_device rd;
std::mt19937 dre{ rd() };
char Pan[6][6];
bool isPair[6][6];
char cmd;
int wid, hei;
char h1, w1, h2, w2;
int doCnt, pairCnt;
int score;

void counting(int second) {
	for (int i = second; i > 0; --i) {
		std::cout << i << '\n';
		Sleep(1000);
	}
}
void command() 
{
	std::cout << "command > ";
	std::cin >> cmd;
}
void initPan()
{
	pairCnt = 0;
	doCnt = 5;
	h1 = w1 = h2 = w2 = 0;
	score = 0;
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 6; ++j) {
			Pan[i][j] = '\0';
			isPair[i][j] = false;
		}
	}
	while(true) {
		std::cout << "가로(3 ~ 6): ";
		std::cin >> wid;
		if (not std::cin) {
			std::cin.clear();
			std::cin.ignore(1e10, '\n');
			std::cerr << "잘못된 입력\n";
		}
		else if (3 <= wid and wid <= 6) break;
		else {
			std::cout << "잘못된 숫자 입력(3~6가 아님)\n";
		}
	}
	while (true) {
		std::cout << "세로(3 ~ 6): ";
		std::cin >> hei;
		if (not std::cin) {
			std::cin.clear();
			std::cin.ignore(std::string::npos, '\n');
			std::cerr << "잘못된 입력\n";
		}
		else if (3 <= hei and hei <= 6) break;
		else {
			std::cout << "잘못된 숫자 입력(3~6가 아님)\n";
		}
	}


	std::uniform_int_distribution dx{ 0, wid - 1 }, dy{ 0, hei - 1 };
	int cnt = 0;
	int x{}, y{};
	if ((wid * hei) % 2 == 1) {
		Pan[dy(dre)][dx(dre)] = '@';
		++cnt;
		while (cnt < wid * hei) {
			while (true) {
				x = dx(dre); y = dy(dre);
				if (Pan[y][x] == '\0') {
					Pan[y][x] = 'a' + (cnt - 1) / 2;
					++cnt;
					break;
				}
			}
		}
	}
	else {
		while (cnt < wid * hei) {
			while (true) {
				x = dx(dre); y = dy(dre);
				if (Pan[y][x] == '\0') {
					Pan[y][x] = ('a' + cnt / 2);
					++cnt;
					break;
				}
			}

		}
	}
}
void printPan(int who, bool isHint) 
{
	std::cout << "\t";
	for (int i = 0; i < wid; ++i) std::cout << char('a' + i) << "\t";
	std::cout << '\n';

	for (int i = 0; i < hei; ++i) {
		std::cout << i + 1 << "\t";
		for (int j = 0; j < wid; ++j) {
			
			if (who > 0) {
				if (((w1 - 'a') == j) and ((h1 - '1') == i)) 
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					std::cout << Pan[i][j] << "\t";
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				}
				else if (who == 2 and ((w2 - 'a') == j) and ((h2 - '1') == i)) 
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					std::cout << Pan[i][j] << "\t";
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				}
				else {
					if (isPair[i][j]) {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
						std::cout << (char)toupper(Pan[i][j]) << "\t";
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					}
					else std::cout << (isHint ? Pan[i][j] : '*') << "\t";
				}
			}
			else {
				if (isPair[i][j]) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
					std::cout << (char)toupper(Pan[i][j]) << "\t";
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				}
				else std::cout << (isHint ? Pan[i][j] : '*') << "\t";
			}
		}
		std::cout << '\n';
	}
}
bool isWin()
{
	for (int i = 0; i < hei; ++i) {
		for (int j = 0; j < wid; ++j) {
			if (not isPair[i][j]) return false;
		}
	}
	return true;
}
void CheckPan()
{
	if (Pan[h1 - '1'][w1 - 'a'] == '@') {
		isPair[h1 - '1'][w1 - 'a'] = true;
		for (int i = 0; i < hei; ++i) {
			for (int j = 0; j < wid; ++j) {
				if ((i != (h2 - '1')) and
					(j != (w2 - 'a')) and
					Pan[i][j] == Pan[h2 - '1'][w2 - 'a']) {
					isPair[i][j] = isPair[h2 - '1'][w2 - 'a'] = true;
					pairCnt++;
					score += doCnt;
					return;
				}
			}
		}
	} else  if (Pan[h2 - '1'][w2 - 'a'] == '@') {
		isPair[h2 - '1'][w2 - 'a'] = true;
		for (int i = 0; i < hei; ++i) {
			for (int j = 0; j < wid; ++j) {
				if ((i != (h1 - '1')) and
					(j != (w1 - 'a')) and
					Pan[i][j] == Pan[h1 - '1'][w1 - 'a']) {
					isPair[i][j] = isPair[h1 - '1'][w1 - 'a'] = true;
					pairCnt++;
					score += doCnt;
					return;
				}
			}
		}
	}
	else if (Pan[h1 - '1'][w1 - 'a'] == Pan[h2 - '1'][w2 - 'a']) {
		isPair[h1 - '1'][w1 - 'a'] = isPair[h2 - '1'][w2 - 'a'] = true;
		pairCnt++;
		score += doCnt;
	}
	else {
		--doCnt;
	}
}
void run()
{
	while (true) {
		printPan(0, false);
		if (isWin()) {
			std::cout << "게임 승리! (남은 횟수 " << doCnt << "회)" << "<최종점수: " <<score  << "점>\n";
			counting(3);
			break;
		} 
		else if(not doCnt) {
			std::cout << "게임 패배! (짝 맞춘 횟수 " << pairCnt << "회)" << "<최종점수: " << score << "점>\n";
			counting(3);
			break;
		}
		while(true) {
			std::cout << "명령어를 입력하려면 **를 입력해주세요 (두 번째 입력에서는 명령어 입력 불가)\n";
			std::cout << "남은 횟수 (틀리면 사라짐): " << doCnt << "\n";
			std::cout << "첫번째 위치 입력 > ";
			std::cin >> w1 >> h1;
			if (isalpha(w1) and isdigit(h1) and (not isPair[h1 - '1'][w1 - 'a'])) break;
			else if (w1 == h1 and h1 == '*') {
				command();
				switch (cmd) {
				case 'q':
					std::cout << "게임 강제 종료\n";
					counting(2);
					return;
				case'r':
					return;
				case 'h':
					system("cls");
					printPan(0, true);
					std::cout << "힌트 3초\n";
					counting(3);
					system("cls");
					printPan(0, false);
					break;
				}
			}
			else {
				std::cout << "위치 잘못 입력함" << std::endl;
			}
		}
		printPan(1, false);
		while (true) {
			std::cout << "두번째 위치 입력 > ";
			std::cin >> w2 >> h2;
			if (isalpha(w2) and isdigit(h2) and 
				not (w1 == w2 and h1 == h2) and
				(not isPair[h2 - '1'][w2 - 'a'])
				) break;
			else {
				std::cout << "위치 잘못 입력함" << std::endl;
			}
		}
		printPan(2, false);
		CheckPan();
		Sleep(1500);
		system("cls");
	}
}

int main()
{
	while(true) {
		std::cout << "\t카드 맞추기 게임\n";
		std::cout << "\t\t\ts: 게임 시작\n";
		std::cout << "\t\t\tq: 게임 종료\n";
		command();
		switch (cmd) {
		case 'q':
			system("cls");
			std::cout << "게임을 종료합니다." << std::endl;
			Sleep(1000);
			return 0;
		case 's':
			system("cls");
			std::cout << "판 생성하기\n";
			while (true) {
				initPan();
				run();
				if(cmd != 'r') break;
			}
			system("cls");
			break;
		}
	}

}

