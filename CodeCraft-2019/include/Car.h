#pragma once

#include <vector>
#include <deque>
#include <iostream>
#include <queue>
#include <map>
using namespace std;
//宏定义：描述汽车状态，分别为“未出发”“准备好但未上路”、“等待出路口”、“等待前一辆车”、“终止”、“已到达”
#define STOP		-1
#define READY		0
#define END			1
#define WAIT		2
#define WAIT_CAR	3
#define ARRIVED		-2

//描述汽车转弯状态，优先级高的数字小
#define STRAIGHT	0
#define LEFT		1
#define	RIGHT		2

//正向
#define FORWARD		0
#define BACK		1

class Road;
class Road_state;
class Cross;
class Car {
	friend ostream & operator<<(ostream &out, Car &obj);
public:
	Car() {};//默认构造函数
	Car(int car_id, int start_id, int end_id, int max_v, int time_to_go);//以文件中读取的值进行构造
	Car(vector<int>& data) :Car(data[0], data[1], data[2], data[3], data[4])
	{
		if (data.size() != 5) cout << "Car:Initializing Failed" << endl;
	}
	~Car() { _road_ptr = nullptr; }
public:
	//固有参数
	int _car_id = -1;			//车辆编号
	int _start_id = -1;			//起始位置
	int _end_id = -1;			//终止位置
	int _max_v = -1;			//最大速度
	int _time_to_go = -1;		//计划出发时间
	//状态量
	int _road_id = -1;			//当前车辆所在道路
	int _direction = -1;		//处于正向车道还是反向车道
	int _state = STOP;			//车辆行驶状态
	int _lanes_id = -1;			//当前行驶的车道号
	int _position = -1;			//到路口起点的距离
	int _v_current = -1;		//在当前道路可行驶的最大行驶速度
	int _next_road_id = -1;		//下一路口的id，仅在出路口时赋值
	Road * _road_ptr = nullptr;
public:
	//输出结果
	vector<int> Routine = vector<int>();	//存储汽车路径，在进入道路时，将道路id压入
	vector<int> _route_pre_planned;
public:
	//函数
	Road_state * get_current_road_state_pointer();			//获得所在道路状态的指针
	Road_state * get_next_road_state_pointer();				//获得下一道路对应的正向或反向道路的指针
	int moveToNextRoad();									//移动到下一条道路
	int get_max_distance_to_move(int next_road_max_v, int length_of_next_road);	//获得下一条道路中可以运行的最远距离，返回前一辆车的状态（如果没有则返回-1）
	int set_car_and_update_road(Road_state * current_road_state, Road_state * next_road_state, int lanes_to_insert, int position_to_insert);//放置车辆到指定位置，并且更新道路状态
	int get_direction();									//根据路口和路径信息，获得车辆的运行方向（注意：需要在get_next_road_id运行后运行）
	bool is_prior_to_others(vector<Car*>& cars_near_cross, Cross &cross);	//判断车辆优先级是否大于其他道路车辆
	bool is_first_in_road();
	//重要函数

	int get_next_road_id(map<int, Road> &roads);	//将车辆由车库放入道路时使用
	int get_next_road_id();		//根据先验信息和道路状况，对_next_road_id赋值，如果即将到达终点，返回ARRIVED

public:
	void get_planned_route(map<int, Cross> &cross, map<int, Road> &road);
	int _location_in_route = 0;//车处于路径中的位置
};