#include<iostream>
#include<vector>
#include <windows.h>

struct Rect {
	int x1, y1, x2, y2;
};

class RectManager {
public:
	RectManager() : resource{} {}
	void init()
	{
		int x1, y1, x2, y2;
		std::cout << "초기화\n";
		for (int i = 0; i < 2; ++i) {
			std::cout << "[" << i + 1 << "번째]" << "4개의 정수를 입력하세요 (x1, y1), (x2, y2) > ";
			std::cin >> x1 >> y1 >> x2 >> y2;
			resource.push_back(Rect{ x1, y1, x2, y2 });
		}
	}
	void clear() { resource.clear(); }

	int select() const {
		char cmd;
		std::cout << "어느 도형을?  > ";
		std::cin >> cmd;
		return (isdigit(cmd) ? cmd - '0' : -1);
	}
	void moveX(int idx, bool isRight, int boardSize) {
		if (isRight) {
			if (resource[idx].x2 + 1 <= boardSize) {
				++resource[idx].x1;
				++resource[idx].x2;
			}
		}
		else {
			if (resource[idx].x1 > 1) {
				--resource[idx].x1;
				--resource[idx].x2;
			}
		}
	}
	void moveY(int idx, bool isDown, int boardSize) {
		if (isDown) {
			if (resource[idx].y2 + 1 <= boardSize) {
				++resource[idx].y1;
				++resource[idx].y2;
			}
		}
		else {
			if (resource[idx].y1 > 1) {
				--resource[idx].y1;
				--resource[idx].y2;
			}
		}
	}

	void sizeX(int idx, bool isExtend, int boardSize) {
		if (isExtend) {
			if (resource[idx].x2 + 1 <= boardSize) ++resource[idx].x2;
		}
		else {
			if (resource[idx].x1 < resource[idx].x2)  --resource[idx].x2;
		}

	}
	void sizeY(int idx, bool isExtend, int boardSize) {
		if (isExtend) {
			if (resource[idx].y2 + 1 <= boardSize) ++resource[idx].y2;
		}
		else {
				if (resource[idx].y1 < resource[idx].y2) --resource[idx].y2;
			}
	}

	void calArea() const {
		for (int i = 0; i < resource.size(); ++i) {
			int area = (resource[i].x2 - resource[i].x1) * (resource[i].y2 - resource[i].y1);
			std::cout << i + 1 << "번째 도형의 넓이: " << area << '\n';
		}
		std::cout << std::endl;
	}

	const Rect& operator[](int idx) const {
		return resource[idx];
	}
	size_t size() const { return resource.size(); }
private:
	std::vector<Rect> resource;
};

class Board {
public:
	Board() : board{ 40, std::vector<char>(40, '*') }, size{ 30 } {}
	void draw(const RectManager& rm) {
		for (auto& rows : board)
			for (auto& ch : rows)
				ch = '*';

		// 그리기 
		for (int idx = 0; idx < rm.size(); ++idx) {
			for (int i = rm[idx].x1 - 1; i < rm[idx].x2; ++i) {
				for (int j = rm[idx].y1 - 1; j < rm[idx].y2; ++j) {
					if (board[j][i] != '*') board[j][i] = '#';
					else board[j][i] = '0' + idx;
				}
			}
		}
	}
	void print() const {
		for (int i = 0; i < size; ++i) {
			for (int j = 0; j < size; ++j) {
				if (board[i][j] == '#') {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					std::cout << board[i][j] << " ";
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				}
				else std::cout << board[i][j] << " ";
			}
			std::cout << std::endl;
		}
	}
	void sizeUp() { if (size < 40) ++size; }
	void sizeDown(RectManager& rm) {
		if (DownCheck(rm) and size > 10)
			--size;
		else std::cout << "현재 크기를 줄일 수 없는 상태입니다" << '\n';
	}
	int getSize() const { return size; }
private:
	std::vector<std::vector<char>> board;
	int size;

	bool DownCheck(RectManager& rm) {
		for (int i = 0; i < rm.size(); ++i) {
			if ((rm[i].x2 == size and rm[i].x1 == 1) or
				(rm[i].y2 == size and rm[i].y1 == 1)) return false;
			else {
				if (rm[i].x2 == size and rm[i].x1 > 1) rm.moveX(i, false, size);
				if (rm[i].y2 == size and rm[i].y1 > 1) rm.moveY(i, false, size);
			}
		}
		// 여기서는 직사각형이 경계에 있으면 변경하기
		return true;
	}
};


void command(char* cmd) {
	std::cout << "command > ";
	std::cin >> *cmd;
}

int main()
{
	Board board;
	RectManager rm;
	char cmd;
	int idx;

	rm.init();

	while (true) {
		board.draw(rm);
		board.print();
		command(&cmd);
		switch (cmd) {
		case 'q':
			return 0;
		case 'r':
			rm.clear();
			rm.init();
			break;
		case 'x': // x축 우측 이동
			idx = rm.select();
			if (0 <= idx and idx < rm.size()) {
				rm.moveX(idx, true, board.getSize());
			}
			break;
		case 'X': // x축 좌측 이동
			idx = rm.select();
			if (0 <= idx and idx < rm.size()) {
				rm.moveX(idx, false, board.getSize());
			}
			break;
		case 'y': // y축 아래측 이동
			idx = rm.select();
			if (0 <= idx and idx < rm.size()) {
				rm.moveY(idx, true, board.getSize());
			}
			break;
		case 'Y': // y축 위측 이동
			idx = rm.select();
			if (0 <= idx and idx < rm.size()) {
				rm.moveY(idx, false, board.getSize());
			}
			break;
		case 's': // 전체 축소
			idx = rm.select();
			if (0 <= idx and idx < rm.size() and
				rm[idx].x2 - rm[idx].x1 >= 1 and
				rm[idx].y2 - rm[idx].y1 >= 1) {
				rm.sizeX(idx, false, board.getSize());
				rm.sizeY(idx, false, board.getSize());
			}
			break;
		case 'S': // 전체 확대
			idx = rm.select();
			if (0 <= idx and idx < rm.size() and
				rm[idx].x2 < board.getSize() and
				rm[idx].y2 < board.getSize()) {
				rm.sizeX(idx, true, board.getSize());
				rm.sizeY(idx, true, board.getSize());
			}
			break;
		case 'i': // x축 한 칸 축소
			idx = rm.select();
			if (0 <= idx and idx < rm.size())
				rm.sizeX(idx, false, board.getSize());
			break;
		case 'I': // x축 한 칸 확대
			idx = rm.select();
			if (0 <= idx and idx < rm.size())
				rm.sizeX(idx, true, board.getSize());
			break;
		case 'j': // y축 한 칸 축소
			idx = rm.select();
			if (0 <= idx and idx < rm.size())
				rm.sizeY(idx, false, board.getSize());
			break;
		case 'J': // y축 한 칸 확대
			idx = rm.select();
			if (0 <= idx and idx < rm.size())
				rm.sizeY(idx, true, board.getSize());
			break;
		case 'a': // x축 한 칸 확대, y축 한 칸축소
			idx = rm.select();
			if (0 <= idx and idx < rm.size() and
				rm[idx].x2 <= board.getSize() and
				rm[idx].y2 - rm[idx].y1 >= 1) {
				rm.sizeX(idx, true, board.getSize());
				rm.sizeY(idx, false, board.getSize());
			}
			break;
		case 'A': // x축 한칸축소, y축한 칸확대
			idx = rm.select();
			if (0 <= idx and idx < rm.size() and
				rm[idx].x2 - rm[idx].x1 >= 1 and
				rm[idx].y2 <= board.getSize()) {
				rm.sizeX(idx, false, board.getSize());
				rm.sizeY(idx, true, board.getSize());
			}
			break;
		case 'b': // 사각형의 면적출력
			rm.calArea();
			break;
		case 'c': // 보드칸 전체 확대 (40칸까지)
			board.sizeUp();
			break;
		case 'd': // 보드칸 전체 축소 (10칸까지)
			board.sizeDown(rm);
			break;
		}
	}
}
