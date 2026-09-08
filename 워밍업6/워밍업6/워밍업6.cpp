#include<iostream>
#include<fstream>
#include<vector>
#include<string>

using namespace std::string_literals;

struct Pnt2 {
	double x;
	double y;
};
struct Pnt3 {
	double x;
	double y;
	double z;
};
struct Face {
	int first;
	int second;
	int third;
};

bool static operator==(const Pnt3& lhs, const Pnt3& rhs)  {
	return lhs.x == rhs.x and lhs.y == rhs.y and lhs.z == rhs.z;
}
bool static operator==(const Face& lhs, const Face& rhs) {
	return lhs.first == rhs.first and lhs.second == rhs.second and lhs.third == rhs.third;
}
bool isTriangle(const Pnt3& A, const Pnt3& B, const Pnt3& C) {
	// 1. 두 벡터 AB와 AC 구하기
	double ab_x = B.x - A.x;
	double ab_y = B.y - A.y;
	double ab_z = B.z - A.z;

	double ac_x = C.x - A.x;
	double ac_y = C.y - A.y;
	double ac_z = C.z - A.z;

	// 2. 두 벡터의 외적(Cross Product) 계산
	double cross_x = ab_y * ac_z - ab_z * ac_y;
	double cross_y = ab_z * ac_x - ab_x * ac_z;
	double cross_z = ab_x * ac_y - ab_y * ac_x;

	// 3. 외적 벡터의 길이의 제곱 계산 (루트 연산을 피해 속도 향상)
	double cross_length_sq = cross_x * cross_x + cross_y * cross_y + cross_z * cross_z;

	// 4. 부동 소수점 오차(EPSILON)를 고려하여 0인지 확인
	const double EPSILON = 1e-9;
	return cross_length_sq > EPSILON;
}
bool isdigits(const std::string& str) {
	for (const char& ch : str) {
		if (not isdigit(ch)) return false;
	}
	return true;
}

class Program {
public:
	void Parsing(const std::string& filename)
	{
		std::ifstream read{ filename };
		if (not read) {
			std::cerr << "cannot do parsing: this file(" << filename << ") is not existed\n";
			return;
		}
		std::string str;
		while (read >> str) {
			if (str == "v"s) {
				Pnt3 p3D{};
				read >> p3D.x >> p3D.y >> p3D.z;
				if (not read) {
					std::cerr << "parsing failed: invalid file format" << std::endl;
					clear();
					break;
				}
				vertexs.push_back(p3D);
			}
			else if (str == "vt"s) {
				Pnt2 p2D{};
				read >> p2D.x >> p2D.y;
				if (not read) {
					std::cerr << "parsing failed: invalid file format" << std::endl;
					clear();
					break;
				}
				textures.push_back(p2D);
			}
			else if (str == "f") {
				Face face{}, texture{};
				std::string first, second, third;
				read >> first >> second >> third;
				auto seqf = first.find('/');
				auto seqs = second.find('/');
				auto seqt = third.find('/');
				if (seqf != std::string::npos and
					seqs != std::string::npos and
					seqt != std::string::npos) {

					face = { stoi(first.substr(0, seqf)), stoi(second.substr(0, seqs)), stoi(third.substr(0, seqt)) };
					texture = { stoi(first.substr(seqf+1)), stoi(second.substr(seqs + 1)), stoi(third.substr(seqt + 1)) };
					faces.push_back(face);
					faxture.push_back(texture);

				} else if (seqf == std::string::npos and
					seqs == std::string::npos and
					seqt == std::string::npos and
					isdigits(first) and
					isdigits(second) and
					isdigits(third)) {
					face = {stoi(first), stoi(second), stoi(third)};
					faces.push_back(face);
				} else {
					std::cerr << "parsing failed: invalid face file format" << std::endl;
					clear();
					break;
				}
				
			}
			else {
				std::cerr << "parsing failed: invalid keyword  - " << str << std::endl;
				clear();
				break;
			}
		}
	}

	void clear() {
		vertexs.clear();
		textures.clear();
		faces.clear();
		faxture.clear();
	}

	std::pair<bool, std::string> Checking() const{
		// 검사할 거
		// 1. vertex 중복이 있는가?
		for (int i = 0; i < vertexs.size(); ++i) {
			for (int j = 0; j < vertexs.size(); ++j) {
				if (i != j and vertexs[i] == vertexs[j])
					return std::make_pair(false, "vertex overlapping detected"s);
			}
		}

		// 2. vertex와 texture 좌표가 올바르지 않은가?
		for (const Pnt3& vertex : vertexs) {
			if ((vertex.x < -1.0 or vertex.x > 1.0) or
				(vertex.y < -1.0 or vertex.y > 1.0) or
				(vertex.z < -1.0 or vertex.z > 1.0))
				return std::make_pair(false, "invalid vertex value detected"s);
		}
		for (const Pnt2& texture : textures) {
			if ((texture.x < -1.0 or texture.x > 1.0) or
				(texture.y < -1.0 or texture.y > 1.0))
				return std::make_pair(false, "invalid texture value detected"s);
		}

		// 3. 삼각형이 이루어지지 않는가?
		// 3-1. 중복된 인덱스로 구성
		for (int i = 0; i < faces.size(); ++i) {
			if (faces[i].first == faces[i].second or
				faces[i].second == faces[i].third or
				faces[i].third == faces[i].first) {
				return std::make_pair(false, "vertex index overlapping detected"s);
			}
		}
		// 3-2. 인덱스 범위 외의 인덱스로 구성
		// 3-3. 세 점이 한 직선에 존재
		for (const Face face : faces) {
			if (face.first > vertexs.size() or face.first <= 0 or
				face.second > vertexs.size() or face.second <= 0 or
				face.third > vertexs.size() or face.third <= 0)
				return std::make_pair(false, "not existed vertex index detected"s);

			// 확인법 : 각 성분의 차이(비율)가 모두 일치하는지 확인합니다.
			// 분모가 0이 되는 성분이 있다면, 대응하는 분자도 0이어야 합니다.
			if (not isTriangle(vertexs[face.first - 1], vertexs[face.second - 1], vertexs[face.third - 1]))
				return std::make_pair(false, "three vertexs in one line detected"s);
		}
		return std::make_pair(true, std::string());
	}
	void print() const {
		for (int i = 0; i < faces.size(); ++i) {
			std::cout << "face[" << i + 1 << "]\n";
			std::cout << "vertex1(" << vertexs[faces[i].first - 1].x
				<< ", " << vertexs[faces[i].first - 1].y
				<< ", " << vertexs[faces[i].first - 1].z
				<< ")\n";
			std::cout << "vertex2(" << vertexs[faces[i].second - 1].x
				<< ", " << vertexs[faces[i].second - 1].y
				<< ", " << vertexs[faces[i].second - 1].z
				<< ")\n";
			std::cout << "vertex3(" << vertexs[faces[i].third - 1].x
				<< ", " << vertexs[faces[i].third - 1].y
				<< ", " << vertexs[faces[i].third - 1].z
				<< ")\n";
			if(faxture.size()){
				std::cout << "texture1(" << textures[faxture[i].first - 1].x
					<< ", " << textures[faxture[i].first - 1].y << ")\n";
				std::cout << "texture2(" << textures[faxture[i].second - 1].x
					<< ", " << textures[faxture[i].second - 1].y << ")\n";
				std::cout << "texture3(" << textures[faxture[i].third - 1].x
					<< ", " << textures[faxture[i].third - 1].y << ")\n";
			}
			std::cout << "\n";
		}
	}
private:
	std::vector<Pnt3> vertexs;
	std::vector<Pnt2> textures;
	std::vector<Face> faces;
	std::vector<Face> faxture;
};

int main()
{
	Program p;
	std::string filename;
	char cmd;
	std::pair<bool, std::string> check;
	while(true) {
		std::cout << "command > ";
		std::cin >> cmd;

		switch (cmd) {
		case 'q':
			return 0;
		case 'p': // parsing: txt파일 이름을 읽어 파싱한다.
			p.clear();
			std::cout << "파일 이름? > ";
			std::cin >> filename;
			p.Parsing(filename);
			check = p.Checking();
			if (not check.first) {
				std::cerr << check.second << std::endl;
				p.clear();
			}
			break;
		case 's': // see: 파싱한 내용이 있으면 그 내용 출력하기 (삼각형의 좌표값 출력)
			if (check.first) p.print();
			else std::cerr << "not exist parsing content\n";
			break;
		}
	}

}

// 예외 처리 테스트:
// 1) 삼각형이 이루어지지 않는 경우(즉 면의 꼭짓점이 3개가 아닌 경우)
// 2) 삼각형 꼭짓점의 값이 중복된 경우
// 3) 인덱스 범위 외의 인덱스로 삼각형, 텍스처 구성
// 4) 허용되지 않는 문자값 포함 여부 O
// 5) 즉, f의 인덱스들을 체크하여 각면이삼각형을이루는지
//      확인하여삼각형이이루어지지않으면에러메시지를출력하시오