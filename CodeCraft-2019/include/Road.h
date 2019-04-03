#pragma once
#include <vector>
#include <deque>
#include <iostream>
#include <map>
#include "Car.h"
#include <math.h>
using namespace std;

//计算BRC的系数
#define ALPHA 0.15
#define BETA 4

class Cross;
class Road;

class Road_state {
public:
	Road_state() {};
	Road_state(Road * _road, int _direction);
	~Road_state() { _road_ptr = nullptr; }; //定义析构函数，避免释放所指内存
public:
	int _direction = -1;									//0表示正向车道，1表示反向车道,-1表示未初始化
	Road* _road_ptr = nullptr;									//指向道路的指针
	vector<deque<Car *>> _cars_in_road;							//指向一个数组，第i车道第j位置的车辆信息，存储于(i-1)*_num_of_lanes + j
	double _Q_brc = 0;	//BRC中的Q，表示道路中车辆的数目
	double _ti_brc = 0;
public:
	int get_num_of_lanes();		//获得道路中的车道数
	int get_road_length();		//获得道路长度
	int get_max_v_of_road();	//获得道路中最大速度
	int get_road_id();			//获得道路id
public:
	Car* get_first_waiting_car();								//获得当前道路中第一优先级的、状态处于WAIT的车，如果没有，返回NULL
	int get_position_to_insert(int &lanes, int &position);		//获得当前道路，第一顺序的插入位置，返回插入位置前一位置车辆的状态；如果前一位置没有车辆，返回-1
	double get_brc_ti();
};

class Road {
	friend ostream & operator<<(ostream &out, Road &obj);
public:
	Road();
	Road(int road_id, int road_length, int max_v, int num_of_lanes,
		int start_node_id, int end_node_id, int is_2_way);
	Road(vector<int>& data) :Road(data[0], data[1], data[2], data[3], data[4], data[5], data[6])
	{
		if (data.size() != 7) cout << "Road: Initializing Failed" << endl;
	}
	~Road() { _start_node_ptr = nullptr; _end_node_ptr = nullptr; }
public:
	int _road_id = -1;			//道路id
	int _road_length = -1;		//道路长度
	int _max_v = -1;			//道路限速
	int _num_of_lanes = -1;		//车道数
	int _start_node_id = -1;	//起始节点id
	int _end_node_id = -1;		//终止节点id
	int _is_2_way = -1;		//是否双车道

	double _t0_brc;	//道路通行时间
	double _C_brc;	//道路容量
public:
	Cross * _start_node_ptr = nullptr;
	Cross * _end_node_ptr = nullptr;
public:
	Road_state _forward;//前向车道的状态
	Road_state _backward;//反向车道的状态
public:
	int update_adjacent_node_ptr(map<int, Cross>& cross_all);	//更新指向路口的指针
};


class Cross {
	friend ostream & operator<<(ostream &out, Cross &obj);
public:
	Cross() {};
	Cross(int node_id, int road_id_up, int road_id_right, int road_id_down, int road_id_left);
	Cross(vector<int>& data) :Cross(data[0], data[1], data[2], data[3], data[4])
	{
		if (data.size() != 5) cout << "Cross:Initializing Failed" << endl;
	}
public:
	int _node_id = -1;
	//按上右下左顺时针存储
	vector<int> _road_id_adjacent = { -1,-1,-1,-1 };
	vector<int> _node_id_adjacent = { -1,-1,-1,-1 };
	//按上右下左的方式存储道路指针
	//vector<Road*> _road_ptr_adjacent = { nullptr,nullptr,nullptr,nullptr };
	vector<Cross*> _node_ptr_adjacent = { nullptr,nullptr,nullptr,nullptr };
	vector<Road_state*> _road_state_ptr_adjacent_out = { nullptr,nullptr,nullptr,nullptr };	//存储出路口方向的Road_state的指针
	vector<Road_state*> _road_state_ptr_adjacent_in = { nullptr,nullptr,nullptr,nullptr };	//存储入路口方向的Road_state的指针
	map<int, vector<vector<int>>> destination_routes;	//当前节点到其它所有节点的路线（路口-路口-路口-路口）
	map<int, vector<vector<int>>> destination_way;	//当前节点到其它所有节点的路线（道路-道路-道路）

	bool visit_flag = 0;
	bool inStack_flag = 0;

	int _flag = 0;		//flag = 1表示已处理完， flag = 0表示未处理;
public:
	int get_next_cross_and_road_state(int road_id_cur, map<int, Road>& All_Roads, int &node_id_next, \
		Cross* &node_ptr_next, Road_state* &road_state_out, Road_state* &road_state_in);		//获取邻接节点的id和指针
	void update_4_neighbor(map<int, Road>& All_Roads);//更新节点成员变量中五个vector的值
};
