#include<iostream>
#include<random>

std::random_device rd;
std::default_random_engine dre{ rd() };
std::uniform_int_distribution uid{ 0, 9 };

int mat4_1[4][4];
int mat4_2[4][4];
bool trans, eToggle, fToggle;

void init(int mat4[4][4])
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			mat4[i][j] = uid(dre);
		}
	}
}

void print(int mat4[4][4], int who)
{
	if(who != -1) std::cout << "mat " << who << '\n';
	
	if (trans) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				std::cout << mat4[j][i] << ' ';
			}
			std::cout << "\n";
		}
	}
	else {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				std::cout << mat4[i][j] << ' ';
			}
			std::cout << "\n";
		}
	}
	std::cout << "\n";
}

void MultipleMat(int mat41[4][4], int mat42[4][4])
{
	int result[4][4]{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 4; ++k) {
				result[i][j] += mat41[i][k] * mat42[k][j];
			}
		}
	}
	print(result, -1);
}
void PlusMat(int mat41[4][4], int mat42[4][4])
{
	int result[4][4]{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result[i][j] = mat41[i][j] + mat42[i][j];
		}
	}
	print(result, -1);
}
void MinusMat(int mat41[4][4], int mat42[4][4])
{
	int result[4][4]{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result[i][j] = mat41[i][j] - mat42[i][j];
		}
	}
	print(result, -1);
}

void Determinant(int mat4[4][4])
{
	int det = 
	mat4[0][0] * (mat4[1][1] * mat4[2][2] * mat4[3][3] + mat4[1][2] * mat4[2][3] * mat4[3][1] + mat4[1][3] * mat4[2][1] * mat4[3][2]
		- mat4[1][3] * mat4[2][2] * mat4[3][1] - mat4[1][1] * mat4[2][3] * mat4[3][2] - mat4[1][2] * mat4[2][1] * mat4[3][3])
		-
	mat4[0][1] * (mat4[1][0] * mat4[2][2] * mat4[3][3] + mat4[1][2] * mat4[2][3] * mat4[3][0] + mat4[1][3] * mat4[2][0] * mat4[3][2]
		- mat4[1][3] * mat4[2][2] * mat4[3][0] - mat4[1][0] * mat4[2][3] * mat4[3][2] - mat4[1][2] * mat4[2][0] * mat4[3][3])
		+
	mat4[0][2] * (mat4[1][0] * mat4[2][1] * mat4[3][3] + mat4[1][1] * mat4[2][3] * mat4[3][0] + mat4[1][3] * mat4[2][0] * mat4[3][1]
		- mat4[1][3] * mat4[2][1] * mat4[3][0] - mat4[1][0] * mat4[2][3] * mat4[3][1] - mat4[1][1] * mat4[2][0] * mat4[3][3])
		-
	mat4[0][3] * (mat4[1][0] * mat4[2][1] * mat4[3][2] + mat4[1][1] * mat4[2][2] * mat4[3][0] + mat4[1][2] * mat4[2][0] * mat4[3][1]
		- mat4[1][2] * mat4[2][1] * mat4[3][0] - mat4[1][0] * mat4[2][2] * mat4[3][1] - mat4[1][1] * mat4[2][0] * mat4[3][2]);
	std::cout << "행렬식: " << det << "\n";
}
void plus1Mat(int mat4[4][4]) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			mat4[i][j] = abs(mat4[i][j]+1) % 10;
		}
	}
}
void minus1Mat(int mat4[4][4]) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (mat4[i][j] > 0) mat4[i][j] = abs(mat4[i][j] - 1) % 10;
			else mat4[i][j] = 9;
		}
	}
}

int main()
{
	char cm;
	init(mat4_1);
	init(mat4_2);


	while (true) {
		print(mat4_1, 1);
		print(mat4_2, 2);

		std::cout << "command > ";
		std::cin >> cm;
		switch (cm) {
		case 'q':
			return 0;
		case 's':
			std::cout << "초기화" << '\n';
			init(mat4_1);
			init(mat4_2);
			break;
		case 'm':
			std::cout << "곱셈 결과: " << '\n';
			MultipleMat(mat4_1, mat4_2);
			break;
		case 'a':
			std::cout << "덧셈 결과: " << '\n';
			PlusMat(mat4_1, mat4_2);
			break;
		case 'd':
			std::cout << "뺄셈 결과: " << '\n';
			MinusMat(mat4_1, mat4_2);
			break;
		case 'r': // 행렬식
			Determinant(mat4_1);
			Determinant(mat4_2);
			break;
		case 't': // 전치 행렬 토글
			trans ^= 1;
			break;
		case 'e': // 최솟값 찾고 빼기 토글
			eToggle ^= 1;
			break;
		case 'f': // 최댓값 찾고 더하기 토글
			fToggle ^= 1;
			break;
		case '+': // +1
			plus1Mat(mat4_1);
			std::cout << "+1 결과: ";
			break;
		case '-': // -1
			minus1Mat(mat4_1);
			minus1Mat(mat4_2);
			std::cout << "-1 결과: ";
			break;
		}
	}
}