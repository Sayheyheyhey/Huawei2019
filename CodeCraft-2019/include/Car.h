#pragma once

#include <vector>
#include <deque>
#include <iostream>
#include <queue>
#include <map>
using namespace std;
//�궨�壺��������״̬���ֱ�Ϊ��δ��������׼���õ�δ��·�������ȴ���·�ڡ������ȴ�ǰһ������������ֹ�������ѵ��
#define STOP		-1
#define READY		0
#define END			1
#define WAIT		2
#define WAIT_CAR	3
#define ARRIVED		-2

//��������ת��״̬�����ȼ��ߵ�����С
#define STRAIGHT	0
#define LEFT		1
#define	RIGHT		2

//����
#define FORWARD		0
#define BACK		1

class Road;
class Road_state;
class Cross;
class Car {
	friend ostream & operator<<(ostream &out, Car &obj);
public:
	Car() {};//Ĭ�Ϲ��캯��
	Car(int car_id, int start_id, int end_id, int max_v, int time_to_go);//���ļ��ж�ȡ��ֵ���й���
	Car(vector<int>& data) :Car(data[0], data[1], data[2], data[3], data[4])
	{
		if (data.size() != 5) cout << "Car:Initializing Failed" << endl;
	}
	~Car() { _road_ptr = nullptr; }
public:
	//���в���
	int _car_id = -1;			//�������
	int _start_id = -1;			//��ʼλ��
	int _end_id = -1;			//��ֹλ��
	int _max_v = -1;			//����ٶ�
	int _time_to_go = -1;		//�ƻ�����ʱ��
	//״̬��
	int _road_id = -1;			//��ǰ�������ڵ�·
	int _direction = -1;		//�������򳵵����Ƿ��򳵵�
	int _state = STOP;			//������ʻ״̬
	int _lanes_id = -1;			//��ǰ��ʻ�ĳ�����
	int _position = -1;			//��·�����ľ���
	int _v_current = -1;		//�ڵ�ǰ��·����ʻ�������ʻ�ٶ�
	int _next_road_id = -1;		//��һ·�ڵ�id�����ڳ�·��ʱ��ֵ
	Road * _road_ptr = nullptr;
public:
	//������
	vector<int> Routine = vector<int>();	//�洢����·�����ڽ����·ʱ������·idѹ��
	vector<int> _route_pre_planned;
public:
	//����
	Road_state * get_current_road_state_pointer();			//������ڵ�·״̬��ָ��
	Road_state * get_next_road_state_pointer();				//�����һ��·��Ӧ����������·��ָ��
	int moveToNextRoad();									//�ƶ�����һ����·
	int get_max_distance_to_move(int next_road_max_v, int length_of_next_road);	//�����һ����·�п������е���Զ���룬����ǰһ������״̬�����û���򷵻�-1��
	int set_car_and_update_road(Road_state * current_road_state, Road_state * next_road_state, int lanes_to_insert, int position_to_insert);//���ó�����ָ��λ�ã����Ҹ��µ�·״̬
	int get_direction();									//����·�ں�·����Ϣ����ó��������з���ע�⣺��Ҫ��get_next_road_id���к����У�
	bool is_prior_to_others(vector<Car*>& cars_near_cross, Cross &cross);	//�жϳ������ȼ��Ƿ����������·����
	bool is_first_in_road();
	//��Ҫ����

	int get_next_road_id(map<int, Road> &roads);	//�������ɳ�������·ʱʹ��
	int get_next_road_id();		//����������Ϣ�͵�·״������_next_road_id��ֵ��������������յ㣬����ARRIVED

public:
	void get_planned_route(map<int, Cross> &cross, map<int, Road> &road);
	int _location_in_route = 0;//������·���е�λ��
};