#pragma once
#include "Car.h"
#include "Road.h"
#include <map>
#include <vector>
using namespace std;

vector<int> string_2_vector(string str);
map<int, Road> read_road(string file_path);
map<int, Car> read_car(string file_path);
map<int, Cross> read_cross(string file_path);

