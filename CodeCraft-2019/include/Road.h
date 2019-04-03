#pragma once
#include <vector>
#include <deque>
#include <iostream>
#include <map>
#include "Car.h"
#include <math.h>
using namespace std;

//����BRC��ϵ��
#define ALPHA 0.15
#define BETA 4

class Cross;
class Road;

class Road_state {
public:
	Road_state() {};
	Road_state(Road * _road, int _direction);
	~Road_state() { _road_ptr = nullptr; }; //�������������������ͷ���ָ�ڴ�
public:
	int _direction = -1;									//0��ʾ���򳵵���1��ʾ���򳵵�,-1��ʾδ��ʼ��
	Road* _road_ptr = nullptr;									//ָ���·��ָ��
	vector<deque<Car *>> _cars_in_road;							//ָ��һ�����飬��i������jλ�õĳ�����Ϣ���洢��(i-1)*_num_of_lanes + j
	double _Q_brc = 0;	//BRC�е�Q����ʾ��·�г�������Ŀ
	double _ti_brc = 0;
public:
	int get_num_of_lanes();		//��õ�·�еĳ�����
	int get_road_length();		//��õ�·����
	int get_max_v_of_road();	//��õ�·������ٶ�
	int get_road_id();			//��õ�·id
public:
	Car* get_first_waiting_car();								//��õ�ǰ��·�е�һ���ȼ��ġ�״̬����WAIT�ĳ������û�У�����NULL
	int get_position_to_insert(int &lanes, int &position);		//��õ�ǰ��·����һ˳��Ĳ���λ�ã����ز���λ��ǰһλ�ó�����״̬�����ǰһλ��û�г���������-1
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
	int _road_id = -1;			//��·id
	int _road_length = -1;		//��·����
	int _max_v = -1;			//��·����
	int _num_of_lanes = -1;		//������
	int _start_node_id = -1;	//��ʼ�ڵ�id
	int _end_node_id = -1;		//��ֹ�ڵ�id
	int _is_2_way = -1;		//�Ƿ�˫����

	double _t0_brc;	//��·ͨ��ʱ��
	double _C_brc;	//��·����
public:
	Cross * _start_node_ptr = nullptr;
	Cross * _end_node_ptr = nullptr;
public:
	Road_state _forward;//ǰ�򳵵���״̬
	Road_state _backward;//���򳵵���״̬
public:
	int update_adjacent_node_ptr(map<int, Cross>& cross_all);	//����ָ��·�ڵ�ָ��
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
	//����������˳ʱ��洢
	vector<int> _road_id_adjacent = { -1,-1,-1,-1 };
	vector<int> _node_id_adjacent = { -1,-1,-1,-1 };
	//����������ķ�ʽ�洢��·ָ��
	//vector<Road*> _road_ptr_adjacent = { nullptr,nullptr,nullptr,nullptr };
	vector<Cross*> _node_ptr_adjacent = { nullptr,nullptr,nullptr,nullptr };
	vector<Road_state*> _road_state_ptr_adjacent_out = { nullptr,nullptr,nullptr,nullptr };	//�洢��·�ڷ����Road_state��ָ��
	vector<Road_state*> _road_state_ptr_adjacent_in = { nullptr,nullptr,nullptr,nullptr };	//�洢��·�ڷ����Road_state��ָ��
	map<int, vector<vector<int>>> destination_routes;	//��ǰ�ڵ㵽�������нڵ��·�ߣ�·��-·��-·��-·�ڣ�
	map<int, vector<vector<int>>> destination_way;	//��ǰ�ڵ㵽�������нڵ��·�ߣ���·-��·-��·��

	bool visit_flag = 0;
	bool inStack_flag = 0;

	int _flag = 0;		//flag = 1��ʾ�Ѵ����꣬ flag = 0��ʾδ����;
public:
	int get_next_cross_and_road_state(int road_id_cur, map<int, Road>& All_Roads, int &node_id_next, \
		Cross* &node_ptr_next, Road_state* &road_state_out, Road_state* &road_state_in);		//��ȡ�ڽӽڵ��id��ָ��
	void update_4_neighbor(map<int, Road>& All_Roads);//���½ڵ��Ա���������vector��ֵ
};
