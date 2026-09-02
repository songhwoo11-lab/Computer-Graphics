#include<iostream>
#include<fstream>
#include<vector>
#include<deque>
#include<string>
#include<algorithm>
#include <windows.h>


size_t wordCnt(const std::string& str)
{
	size_t cnt = 0;
	bool isWord = false;
	for (const auto& c : str) {
		if ((c != '*' and c != ' ') and not isWord) {
			isWord = true;
			++cnt;
		}
		else if ((c == '*' or c == ' ') and isWord) {
			isWord = false;
		}
	}
	return cnt;
}

struct WORDS {
	std::string::const_iterator data;
	int jul;
	std::size_t size;

};

std::vector<WORDS> UperFirstWord(const std::deque<std::string>& list)
{
	std::vector<WORDS> words;
	bool isWord = false;
	bool isFirstUpper = false;
	size_t start{}, end{};
	for (int j = 0; j < list.size(); ++j) {
		isWord = false;
		isFirstUpper = false;
		for (int i = 0; i < list[j].size(); ++i) {
			if ((list[j][i] != '*' and list[j][i] != ' ') and not isWord) {
				isWord = true;
				if (isupper(list[j][i])) isFirstUpper = true;
				start = i;
			}
			else if ((list[j][i] == '*' or list[j][i] == ' ') and isWord) {
				isWord = false;
				end = i;
				if(isFirstUpper) {
					words.push_back({ list[j].begin() + start, j, end - start });
					isFirstUpper = false;
				}
			}
		}
		if (isFirstUpper)words.push_back({ list[j].begin() + start, j, size_t(list[j].end() - (list[j].begin() + start)) });
	}
	return words;
}

void reverseWord(std::deque<std::string>& list)
{
	bool isWord = false;
	int start = 0;
	for(auto& str: list){
		for (int i = 0; i < str.size(); ++i) {
			if ((str[i] != '*' and str[i] != ' ') and not isWord) {
				isWord = true;
				start = i;
			}
			else if ((str[i] == '*' or str[i] == ' ') and isWord) {
				isWord = false;
				std::reverse(str.begin() + start, str.begin() + i);
			}
		}
		if (isWord) {
			isWord = false;
			std::reverse(str.begin() + start, str.end());
		}
	}
}
int finding(std::string str, std::string word, size_t offset)
{
	if (word.size() == 0) return -1;
	for (auto& c : str) c = tolower(c);
	for (auto& c : word) c = tolower(c);
	while(true) {
		size_t cnt = str.find(word, offset);
		if (cnt == std::string::npos) return -1;
		else if (
			(
				(cnt > 0 and str[cnt - 1] != ' ' and str[cnt - 1] != '*') and
				(cnt + word.size() < str.size() and str[cnt + word.size() + 1] != ' ' and str[cnt + word.size() + 1] != '*')
				)
			or
			(
				(cnt == 0) and
				(cnt + word.size() < str.size() and str[cnt + word.size()] != ' ' and str[cnt + word.size()] != '*')
				)
			or
			(
				(cnt > 0 and str[cnt - 1] != ' ' and str[cnt - 1] != '*') and
				(cnt + word.size() < str.size())
				)
			) {
			offset += word.size();
		}
		else return cnt;
	}
}

int main()
{
	std::string word;
	char from{}, to{};
	bool bflag = false, cflag = false, gflag = false, hflag = false;
	std::vector<WORDS> words{};
	std::ifstream read{ "data.txt" };
	if (not read) {
		std::cerr << "파일을 읽을 수 없음" << std::endl;
		return 1;
	}
	std::deque<std::string> lists(10);
	for (auto& str : lists) {
		str.resize(1000);
		read.getline(str.data(), 1000, '\n');
		str.resize(strlen(str.data()));
	}

	while (true) {
		std::cout << "--------------------------\n";
		for (int j = 0; j < lists.size(); ++j) {
			auto itr = finding(lists[j], word, 0);

			for (int i = 0; i < lists[j].size(); ++i) {
				if (itr == i) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
					for (int k = 0; k < word.size(); ++k) {
						std::cout << lists[j][itr + k];
					}
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					i += word.size();
					itr = finding(lists[j], word, i);
				}
				if (cflag) {
					words = UperFirstWord(lists);
					for (int k = 0; k < words.size(); ++k) {
						if (words[k].jul < j) continue;
						if (words[k].jul > j) break;
						if (words[k].jul == j) {
							if (words[k].data == (lists[j].begin() + i)) {
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
								for (auto itr = words[k].data; itr < words[k].data + words[k].size; ++itr) {
									std::cout << *itr;
								}
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
								i += words[k].size;
								break;
							}
							else continue;
						}
					}
					if (lists[j][i] == from and gflag) std::cout << to;
					else if (isdigit(lists[j][i]) and hflag) std::cout << lists[j][i] << '\n';
					else std::cout << lists[j][i];
				}
				else if (lists[j][i] == from and gflag) std::cout << to;
				else if (isdigit(lists[j][i]) and hflag) std::cout << lists[j][i] << '\n';
				else std::cout << lists[j][i];
			}
			if (bflag) {
				std::cout << "\t\t\t단어 개수: " << wordCnt(lists[j]);
			}
			std::cout << '\n';
		}
		std::cout << "--------------------------\n";
		if (cflag) std::cout << "대문자로 시작하는 단어 개수: " << words.size() << '\n';
		char ch{};
		std::cout << "command > ";
		std::cin >> ch;
		switch (ch) {
		case 'q':
			return 0;
		case 'a':
			for (auto& str : lists) {
				for (char& character : str) {
					if (isalpha(character)) character = isupper(character) ? tolower(character) : toupper(character);
				}
			}
			break;
		case 'b':
			bflag ^= 1;
			break;
		case 'c':
			if (not cflag and gflag) {
				std::cerr << "이미 변경된 문자가 존재합니다. 원래상태로 되돌린 후 실행하십시오\n";
				break;
			}
			cflag ^= 1;

			break;
		case 'd':
			for (auto& str : lists) {
				std::reverse(str.begin(), str.end());
			}
			break;
		case 'e':
				for (auto& str : lists) {
					for (char& character : str) {
						if (character == ' ') character = '*';
						else if (character == '*') character = ' ';
					}
				}

			break;
		case 'f':			
			reverseWord(lists);
			break;
		case 'g':
		{
			if (cflag and not gflag) {
				std::cout << "대문자로 시작하는 단어가 표시되고 있습니다. 제거 후 실행하십시오\n";
				break;
			}
			gflag ^= 1;
			if(gflag) {
				std::cout << "바꿀 문자>  ";
				std::cin >> from;
				std::cout << "새로 바꿀 문자>  ";
				std::cin >> to;
				std::cout << "새로운 문자로 변경\n";
			}
			else
				std::cout << "원래 상태로 변경\n";
		}
			break;
		case 'h':
			hflag ^= 1;
			break;
		case 'i':
		{
			std::cin >> word;
		}
			break;
		case 'j':
			{
				
				std::string tmp = lists.back();
				lists.pop_back();
				lists.push_front(tmp);
			}
			break;
		}
	}
}