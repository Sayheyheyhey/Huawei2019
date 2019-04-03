#include "Read_file.h"
#include <fstream>
#include <string>
#include <iostream>
using namespace std;

//将字符串转换为数组：省略括号、空格和逗号
vector<int> string_2_vector(string str) {
	vector<int> result;
	int num = 0;
	int flag = 1;
	for (unsigned int i = 0; i < str.size(); i++) {
		if (str[i] == '(' || str[i] == ' ') continue;
		else if (str[i] == '-') flag = -1;
		else if (str[i] >= '0' && str[i] <= '9')
			num = 10 * num + str[i] - '0';
		else if (str[i] == ',' || str[i] == ')') {
			result.push_back(flag * num);
			num = 0;
			flag = 1;
		}
		else {
			cout << "illegal input from txt" << endl;
			break;
		}
	}
	return result;
}


//读取Road.txt,按road_id存储并返回（返回map中Value为Road类型的变量，Key为变量中的_road_id，）
map<int, Road> read_road(string file_path) {
	ifstream in_file(file_path);
	map<int, Road> Roads_all;
	string line;
	vector<int> data;
	int road_id = 0;

	if (in_file) {
		//按行读取txt中的数据
		while (getline(in_file, line)) {
			//std::cout << line << std::endl;
			if (line[0] == '#') continue;//跳过注释
			data.clear();
			data = string_2_vector(line);
			road_id = data[0];
			//Roads_all.insert(pair<int, Road>(road_id, Road(data[0], data[1], data[2], data[3], data[4], data[5], bool(data[6]))));
			Roads_all.insert(pair<int, Road>(road_id, Road(data)));
		}
	}
	else {
		cout << "read_road: No such file" << endl;
	}
	in_file.close();
	return Roads_all;
}

map<int, Car> read_car(string file_path) {
	ifstream in_file(file_path);
	map<int, Car> Cars_all;
	string line;
	vector<int> data;
	int car_id = 0;

	if (in_file) {
		//按行读取txt中的数据
		while (getline(in_file, line)) {
			//	std::cout << line << std::endl;
			if (line[0] == '#') continue;//跳过注释
			data.clear();
			data = string_2_vector(line);
			car_id = data[0];
			Cars_all.insert(pair<int, Car>(car_id, Car(data)));
		}
	}
	else {
		cout << "read_car: No such file" << endl;
	}
	in_file.close();
	return Cars_all;
}

map<int, Cross> read_cross(string file_path) {
	ifstream in_file(file_path);
	map<int, Cross> Cross_all;
	string line;
	vector<int> data;
	int car_id = 0;

	if (in_file) {
		//按行读取txt中的数据
		while (getline(in_file, line)) {
			//std::cout << line << std::endl;
			if (line[0] == '#') continue;//跳过注释
			data.clear();
			data = string_2_vector(line);
			car_id = data[0];
			Cross_all.insert(pair<int, Cross>(car_id, Cross(data)));
		}
	}
	else {
		cout << "read_car: No such file" << endl;
	}
	in_file.close();
	return Cross_all;
}
