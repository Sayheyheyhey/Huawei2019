#pragma once
#include "Car.h"
#include "Road.h"
#include "Read_file.h"
#include "multiShortestPath.h"
#include <string>
//�����жϳ����Ƿ������ ��ǰ�����о��� == ��������ʻ������ ʱ�����������복����ʻ
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

	map<int, Car*> _cars_to_go;			//�ѵ������ʱ�䵫��û��·�ĳ���	

	vector<vector<int>> _route_tab;
	unsigned int _num_of_wainting_cars = 0;
	unsigned int _num_of_cars_not_arrived = 0;

	map<int, Car*> _cars_in_loop;

	long long _time = 0;
public:
	void Read_from_file(string road, string car, string cross);

	int run_car_in_a_lane(deque<Car*> &cars_in_lane);	//WAIT�ĳ����й��󣬸ú�����������ͬ������������
	int run_all_cars_on_road();							//�������е�·�����п�������END�ĳ������������ٶȿ��Գ�·�ڵĳ������ΪWAIT����������·�ڵ�Ҫ�ȴ�ǰ���ĳ������ΪWAIT_CAR;ͳ��ǰ�����ֳ�����Ŀ��_num_of_waiting_car
	int run_waiting_car();								//����״̬ΪWAIT��WAIT_CAR�ĳ���
	int drive_car_in_garage();							//�������еĳ�������·
	int get_cars_to_go();								//��ȡ�����п�����·�ĳ���

	void turn_node_to_way();	//�� ���ڵ�-�ڵ㡱 ·��ת��Ϊ ����·-��·�� ·��
	void node_routes_inversion();	//��Ϊ�á��Ͻ�˹��������������������㷨�ҳ��ġ�·��-·�ڡ�·���ǵ��ŵģ������轫�䷴ת
	void update_routine_stored_in_cross();//����·����ͼ

	void output_answer(string out_file);

	int RUN();//�������еĺ��������ֻرջ�����-1�����򷵻�0

	void re_arrange_looped_cars();//���¹滮����

	int cars_num_on_road = 0;	//��·�ϵĳ�����
	int cars_num_arrived = 0;	//�����յ㳵����
	int car_on_road_num();
	int weight_of_graph;	//��get_weight_of_graph()�������ڣ���ͼȨ��
	void get_weight_of_graph();
};
