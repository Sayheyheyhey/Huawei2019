#pragma once
#include "Car.h"
#include "Road.h"
#include "Read_file.h"
#include "multiShortestPath.h"
#include <string>
//用作判断车辆是否出发， 当前可运行距离 == 车辆可行驶最大距离 时，将车辆放入车道行驶
#define START_CONTROL 1


class Magic_Box {

public:
	Magic_Box() {};
	Magic_Box(const Magic_Box & Box);
	Magic_Box& operator=(const Magic_Box& Box);
public:
	map<int, Road> _roads_all;
	map<int, Car> _cars_all;
	map<int, Cross> _cross_all;

	map<int, Car*> _cars_to_go;			//已到达出发时间但还没上路的车辆	

	vector<vector<int>> _route_tab;
	unsigned int _num_of_wainting_cars = 0;
	unsigned int _num_of_cars_not_arrived = 0;

	map<int, Car*> _cars_in_loop;

	long long _time = 0;
public:
	void Read_from_file(string road, string car, string cross);

	int run_car_in_a_lane(deque<Car*> &cars_in_lane);	//WAIT的车运行过后，该函数用来运行同车道后续车辆
	int run_all_cars_on_road();							//遍历所有道路，运行可以行至END的车辆，将所有速度可以出路口的车辆标记为WAIT；将出不了路口但要等待前车的车辆标记为WAIT_CAR;统计前述两种车的数目至_num_of_waiting_car
	int run_waiting_car();								//运行状态为WAIT和WAIT_CAR的车辆
	int drive_car_in_garage();							//将车库中的车运行上路
	int get_cars_to_go();								//获取车库中可以上路的车辆

	void turn_node_to_way();	//将 “节点-节点” 路径转换为 “道路-道路” 路径
	void node_routes_inversion();	//因为用“迪杰斯特拉—深度优先搜索”算法找出的“路口-路口”路径是倒着的，所有需将其反转
	void update_routine_stored_in_cross();//更新路径地图

	void output_answer(string out_file);

	int RUN();//整体运行的函数，发现回闭环返回-1；否则返回0

	void re_arrange_looped_cars();//重新规划车辆

	int cars_num_on_road = 0;	//道路上的车数量
	int cars_num_arrived = 0;	//到达终点车数量
	int car_on_road_num();
	int weight_of_graph;	//由get_weight_of_graph()函数调节，地图权重
	void get_weight_of_graph();
};
