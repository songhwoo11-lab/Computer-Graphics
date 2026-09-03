#include<iomanip>
#include<iostream>
#include<vector>
#include<deque>
#include<algorithm>

struct Point {
	int x, y, z;
	bool isValid;
	friend std::ostream& operator<<(std::ostream& out, const Point& p) {
		return out << "(" << p.x << ", " << p.y << ", " << p.z << ")";
	}
};

class List {
public:
	List() : list{ std::deque<Point>(10) }, bottom{}, top{} {}

	void print() {
		std::cout << "<<< Vertex List >>>\n";
		for (int i = 9; i > -1; --i) {
			std::cout << i << ": ";
			if (list[i].isValid) std::cout <<  list[i] << "\n";
			else std::cout << "()\n";
		}
		std::cout << "------------------\n\n";
	}
	void push_top(Point p) {
		if (top != 10) {
			list[top++] = p;
		} else {
			if (top - 1 == bottom) top = 1;
			bottom = 0;
			for (int i = 0; i < 10; ++i) {
				if (not list[i].isValid) {
					list[i] = p;
					return;
				}
			}
			std::cerr << "문제 발생!: 더이상 삽입할 위치가 없음\n\n";
		}
	}
	void push_bottom(Point p) {
		if (bottom != 0) {
			list[--bottom] = p;
		}
		else {
			int tmp = 10;
			for (int i = 0; i < 10; ++i) {
				if (not list[i].isValid) {
					tmp = i;
					break;
				}
			}
			if (tmp != 10) {
				for (int i = tmp; i > 0; --i) {
					list[i] = list[i - 1];
				}
				list[0] = p;
				if (tmp == top) top++;
			}
			else {
				std::cerr << "문제 발생!: 더이상 삽입할 위치가 없음\n\n";
			}
			
		}
	}
	void pop_top() {
		while (top > bottom) {
			if (list[top - 1].isValid) {
				list[--top] = Point{};
				return;
			}
			else --top;
		}
		
		std::cerr << "제거할 데이터 없음\n\n";
		
	}
	void pop_bottom() {
		while (top > bottom) {
			if (list[bottom].isValid) {
				list[bottom++] = Point{};
				return;
			}
			else ++bottom;
		}

		std::cerr << "제거할 데이터 없음\n\n";
	}
	void downSwap()
	{
		Point tmp = list.front();
		list.pop_front();
		list.push_back(tmp);
		for (int i = 0; i < 10; ++i) {
			if (list[i].isValid) {
				bottom = i;
				break;
			}
		}
		for (int i = 10; i > 0; --i) {
			top = i;
			if (list[i - 1].isValid) {
				break;
			}
		}
		if (not list[top - 1].isValid) top = 0;
	}
	void clear()
	{
		top = bottom = 0;
		for (int i = 0; i < 10; ++i) list[i] = Point{};
	}
	int cnt()
	{
		int count{};
		for (int i = 0; i < 10; ++i)
			if (list[i].isValid) ++count;
		std::cout << "점 개수: " << count << "개\n";
		return count;
	}
	void Osort()
	{
		std::sort(list.begin(), list.end(), [](Point a, Point b) {
			return (a.x * a.x + a.y * a.y + a.z * a.z) < (b.x * b.x + b.y * b.y + b.z * b.z);
			});
		int count = cnt();
		for(int i = 0; i < 10 -count; ++i) downSwap();
	}
	void gCommand()
	{
		std::vector<Point> pnts;
		std::vector<std::pair<Point, Point>> far_pairs, near_pairs;
		double far_distance{-1}, near_distance{std::numeric_limits<int>::max()};
		for (int i = 0; i < 10; ++i) 
			if (list[i].isValid) pnts.push_back(list[i]);

		for (int i = 0; i < pnts.size(); ++i) {
			for (int j = 0; j < pnts.size(); ++j) {
				if (i < j) {
					int distance = sqrt(pow(pnts[i].x - pnts[j].x, 2) + 
						pow(pnts[i].y - pnts[j].y, 2) + 
						pow(pnts[i].z - pnts[j].z, 2));
					if (distance > far_distance) {
						far_distance = distance;
						far_pairs.clear();
						far_pairs.push_back(std::pair<Point, Point>( pnts[i], pnts[j] ));
					} else if (distance == far_distance) {
						far_pairs.push_back(std::pair<Point, Point>(pnts[i], pnts[j]));
					}
					if (distance < near_distance) {
						near_distance = distance;
						near_pairs.clear();
						near_pairs.push_back(std::pair<Point, Point>(pnts[i], pnts[j]));
					} else if (distance == near_distance) {
						near_pairs.push_back(std::pair<Point, Point>(pnts[i], pnts[j]));
					}
				}
			}
		}

		std::cout << std::setprecision(3);
		std::cout << "\n가장 먼 거리에 있는 두 점\n";
		for (const auto& far : far_pairs) {
			std::cout << far.first << ", " << far.second;
			std::cout << "\t (거리: " << far_distance << ")\n";
		}
		std::cout << "\n가장 가까운 거리에 있는 두 점\n";
		for (const auto& near : near_pairs) {
			std::cout << near.first << ", " << near.second;
			std::cout << "\t (거리: " << near_distance << ")\n";
		}

	}
private:
	std::deque<Point> list;
	int bottom, top;
};


int main()
{
	List list;
	char ch;
	Point tmp{};

	while (true) {
		list.print();
		std::cout << "command > ";
		std::cin >> ch;

		switch (ch) {
		case 'q':
			return 0;
		case '+': // 맨 위에 데이터 넣기
			std::cin >> tmp.x >> tmp.y >> tmp.z;
			if (not std::cin) {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cerr << "잘못된 형식 입력 - 다시시도\n\n";
				break;
			}
			tmp.isValid = true;
			list.push_top(tmp);
			break;
		case '-': // 맨 위에 데이터 제거
			list.pop_top();
			break;
		case 'e': // 리스트의 맨 아래 넣기 
			std::cin >> tmp.x >> tmp.y >> tmp.z;
			if (not std::cin) {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cerr << "잘못된 형식 입력 - 다시시도\n\n";
				break;
			}
			tmp.isValid = true;
			list.push_bottom(tmp);
			break;
		case 'd': // 맨 아래에 데이터 삭제 (비어있다)
			list.pop_bottom();
			break;
		case 'a': // 리스트에 저장된 점의 개수 출력
			list.cnt();
			break;
		case 'b': // 점들의 리스트 위치를 한 칸씩 내려 보낸다. // 왼쪽으로 이동
			list.downSwap();
			break;
		case 'c': // 리스트 클리어
			list.clear();
			break;
		case 'f': // 각 점에서 원점과의 거리를 계산 후 그 값을 정렬하여 오름차순으로 정렬하여 출력 / 아래에 빈칸이 없도록 만들기
			list.Osort();
			break;
		case 'g': // 리스트에 저장된 점들에서 두 점간의 모든 조합에 대한 거리를 계산하고 가장 먼 두 점, 가장 가까운 두 점을 출력한다. 이때, 두 점의 자표값과 그 점 사이의 거리, 가장 먼 두 점, 가장 가까운 두 점을 출력하고 그 점간의 거리도 출력하기
			list.gCommand();
			break;
		}
	}
}