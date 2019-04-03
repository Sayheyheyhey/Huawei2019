#include "Magic_Box.h"
#include <ctime>

#include <algorithm>
Magic_Box& Magic_Box::operator=(const Magic_Box& Box) {
	if (this == &Box)
		return *this;

	_cars_all = Box._cars_all;
	for (auto &cars : Box._cars_to_go) {
		_cars_to_go.insert(pair<int, Car*>(cars.first, &(_cars_all.at(cars.first))));
	}
	for (auto &cars : Box._cars_in_loop) {
		_cars_to_go.insert(pair<int, Car*>(cars.first, &(_cars_all.at(cars.first))));
	}
	return *this;
}


Magic_Box::Magic_Box(const Magic_Box & Box) {
	_cars_all = Box._cars_all;
	for (auto &cars : Box._cars_to_go) {
		_cars_to_go.insert(pair<int, Car*>(cars.first, &(_cars_all.at(cars.first))));
	}
	for (auto &cars : Box._cars_in_loop) {
		_cars_to_go.insert(pair<int, Car*>(cars.first, &(_cars_all.at(cars.first))));
	}
}



void show_car_loop(Car * car) {
	//cout << "************************************************************" << endl;
	cout << "car_id:" << car->_car_id << "; road_id:" << car->_road_id << "; car_state: " << car->_state  \
		<< "; lane_id:" << car->_lanes_id << "; _position:" << car->_position << "; next_road_id: "
		<< car->_next_road_id << "; current_v: " << car->_v_current << endl;
	if (car->_road_ptr != nullptr)
		cout << "road_length:" << car->_road_ptr->_road_length << " ;road_max_v:" << car->_road_ptr->_max_v << endl;
}
void show_car_loop(map<int, Car*> cars_all) {
	for (auto & car : cars_all) {
		show_car_loop(car.second);
	}
	cout << "************************************************************" << endl;
}

void Magic_Box::Read_from_file(string road, string car, string cross) {
	_roads_all = read_road(road);
	_cars_all = read_car(car);
	_cross_all = read_cross(cross);
	_num_of_cars_not_arrived = _cars_all.size();
	//�ȸ��µ�·��cross��������Ϣ
	for (auto &a : _roads_all) {
		a.second.update_adjacent_node_ptr(_cross_all);
	}
	//����Road_state��ָ��Road��ָ�루ʹ��thisָ���ʼ��ʧ�ܣ���ѡȡ���ַ�����
	for (auto &a : _roads_all) {
		Road * road = &(a.second);
		road->_forward._road_ptr = road;
		if (road->_is_2_way == 1) road->_backward._road_ptr = road;
	}
	//����cross��Χ�ڵ���Ϣ
	for (auto &a : _cross_all) {
		a.second.update_4_neighbor(_roads_all);
	}
}

//@brief ����һ��������״̬ΪWAIT_CAR�ĳ�	
int Magic_Box::run_car_in_a_lane(deque<Car*> &cars_in_lane) {
	if (cars_in_lane.size() == 0) return 0;
	for (auto iter = cars_in_lane.begin(); iter != cars_in_lane.end(); iter++) {

		if ((*iter)->_state == WAIT) {
			//	cout << "run_car_in_a_lane NOTE:"<<"Car-"<<(*iter)->_car_id<<" The state is  WAIT in road-"<<(*iter)->_road_id<<"  when run in straight road" << endl;
			return 0;

		}
		if ((*iter)->_state == END) {
			continue;
		}
		else if ((*iter)->_state == WAIT_CAR) {
			if (iter == cars_in_lane.begin()) {//���ǰ��û�г���
				int expect_position = (*iter)->_position - (*iter)->_v_current;
				if (expect_position < 0)
				{
					cout << "run_car_in_a_lane() Error: The Car's state is falsly set before" << endl;
					return 0;
				}
				else {
					(*iter)->_position = expect_position;
					(*iter)->_state = END;
					--_num_of_wainting_cars;
				}
			}
			else {//���ǰ���г���
				int former_car_position = (*(iter - 1))->_position;
				int expect_position = (*iter)->_position - (*iter)->_v_current;
				if (former_car_position < expect_position) {
					(*iter)->_position = expect_position;
					(*iter)->_state = END;
					--_num_of_wainting_cars;
				}
				else {
					(*iter)->_position = former_car_position + 1;
					(*iter)->_state = END;
					--_num_of_wainting_cars;
				}
			}
		}
		else {
			cout << "Misuse of run_car_in_a_lane()" << endl;
			return 0;
		}
	}
	return 0;
}

//@brief ���е���·�Ͽ���������END�ĳ�������ǳ��������صȴ��ĳ�������Ŀ,run_all_cars_on_road���Ӻ���
int run_all_cars_on_road_core(Road_state* road_state, int & a) {
	int sum = 0;
	for (auto &one_lane : road_state->_cars_in_road) {//��������
		for (auto iter = one_lane.begin(); iter != one_lane.end(); iter++) {//��������
			int car_id = (*iter)->_car_id;
			if ((*iter)->_state != END) {
				//cout << " run_all_cars_on_road_core(): Car's state is no END" << endl;
				return 0;
			}
			else if ((*iter)->_v_current > (*iter)->_position) {
				if (iter == one_lane.begin()) {//���ǰ�����赲����ΪWAIT
					(*iter)->_state = WAIT;
					++sum;
					a++;
				}
				else {
					if ((*(iter - 1))->_state == WAIT) {//���ǰ���������ڵȴ�����ΪWAIT
						(*iter)->_state = WAIT;
						++sum;
						a++;
					}
					else if ((*(iter - 1))->_state == WAIT_CAR) {
						(*iter)->_state = WAIT_CAR;
						++sum;
						a++;
					}
					else if ((*(iter - 1))->_state == END) {//���ǰ��������END���ƶ�����ΪEND
						(*iter)->_position = (*(iter - 1))->_position + 1;
						int p_f = (*iter - 1)->_position;
						int p = (*iter - 1)->_position + 1;
						//(*iter)->_state = WAIT;
						(*iter)->_state = END;
						a++;
					}
					else {
						cout << "run_all_cars_on_road_core Error: Wrong state before this car" << endl;
					}
				}
			}
			else if (iter == one_lane.begin()) {
				int expect_position = (*iter)->_position - (*iter)->_v_current;
				if (expect_position < 0) cout << "Get wrong position��run_all_cars_on_road_core() " << endl;
				(*iter)->_state = END;
				a++;
				(*iter)->_position = expect_position;
			}
			else {//���ǰ���г�
				int former_car_position = (*(iter - 1))->_position;
				int expect_position = (*iter)->_position - (*iter)->_v_current;
				if (former_car_position < expect_position) {//��������赲
					(*iter)->_position = expect_position;
					(*iter)->_state = END;
					a++;
				}
				else {//������赲
					if ((*(iter - 1))->_state == WAIT || (*(iter - 1))->_state == WAIT_CAR) {
						(*(iter))->_state = WAIT_CAR;
						++sum;
						a++;
					}
					else if ((*(iter - 1))->_state == END) {
						(*iter)->_position = former_car_position + 1;
						(*iter)->_state = END;
						a++;
					}
					else {
						cout << "run_all_cars_on_road_core Error: Wrong state of former car" << endl;
					}
				}
			}
		}
	}
	return sum;
}
//@brief �������е�·�����п�������END�ĳ����������������
//		 �������ٶȿ��Գ�·�ڵĳ������ΪWAIT��
//		 ��������·�ڵ�Ҫ�ȴ�ǰ���ĳ������ΪWAIT_CAR;ͳ��ǰ�����ֳ�����Ŀ��_num_of_waiting_car
//		 ���ر������ӵĵȴ�������Ŀ
int Magic_Box::run_all_cars_on_road() {
	cars_num_on_road = 0;
	int sum = 0;
	for (auto &road : _roads_all) {
		Road_state * road_state = &(road.second._forward);
		sum += run_all_cars_on_road_core(road_state, cars_num_on_road);
		if (road.second._is_2_way == 1) {
			road_state = &(road.second._backward);
			sum += run_all_cars_on_road_core(road_state, cars_num_on_road);
		}
	}
	_num_of_wainting_cars += sum;
	return sum;
}

//����״̬ΪWAIT�ĳ��������к�����ͬ����״̬ΪWAIT_CAR�ĳ���
int Magic_Box::run_waiting_car() {
	int _last_num_of_waiting_cars = -1;
	while (_num_of_wainting_cars != 0) {
		if (_num_of_wainting_cars == _last_num_of_waiting_cars) {
			cout << "Error: Waiting Loop Detected!" << endl;
			//	cout << "The number of waiting car is:" << _num_of_wainting_cars << endl;
				//�洢�ջ�������Ϣ
			for (auto &road : _roads_all) {
				for (auto &lane : (road.second._forward._cars_in_road)) {
					if (lane.size() == 0) continue;
					if ((lane.front())->_state == WAIT || (lane.front())->_state == WAIT_CAR) {
						if ((lane.front())->_next_road_id != -1)
							_cars_in_loop.insert(pair<int, Car*>(lane.front()->_car_id, lane.front()));
					}
				}
				if (road.second._is_2_way == 1) {
					for (auto &lane : (road.second._backward._cars_in_road)) {
						if (lane.size() == 0) continue;
						if ((lane.front())->_state == WAIT || (lane.front())->_state == WAIT_CAR) {
							if ((lane.front())->_next_road_id != -1)
								_cars_in_loop.insert(pair<int, Car*>(lane.front()->_car_id, lane.front()));
						}
					}

				}
			}
			show_car_loop(_cars_in_loop);
			cout << "Here" << endl;
			return -1;
		}
		_last_num_of_waiting_cars = _num_of_wainting_cars;
		//��ѭ���еĺ������������л�ı�_num_of_waiting_cars����Ŀ
		for (auto &cross : _cross_all) {
			int node_id_look = cross.second._node_id;
			//1.����·id˳�򣬻�ȡ·����Χ��һ���ȼ��ĳ���
			int num_of_valid_road_state = 0;//��Ч�ĵ�·��<=4��
			vector<Road_state *> road_near_cross;//����·�ڵ�Road_state
			for (auto &road_state : cross.second._road_state_ptr_adjacent_in) {
				if (road_state != nullptr) {
					road_near_cross.push_back(road_state);
					++num_of_valid_road_state;
				}
			}
			if (num_of_valid_road_state < 2) cout << "1.invalid cross: maybe something wrong when initializing" << endl;
			//����·ָ�밴�յ�·id��С��������(��Ҫ����Ϊ��ָ������µ�����)
			sort(road_near_cross.begin(), road_near_cross.end(), [](Road_state *a, Road_state *b) {
				return a->get_road_id() < b->get_road_id();
			});
			vector<Car*> car_near_cross(num_of_valid_road_state);//����·�ڵ�Car
			Car* car_tmp = nullptr;
			int num_of_car = 0;//��¼·�ڳ�����Ŀ
			for (int i = 0; i < num_of_valid_road_state; i++) {
				if (road_near_cross[i] == nullptr) cout << "2.invalid cross: maybe something wrong when initializing" << endl;;
				car_tmp = road_near_cross[i]->get_first_waiting_car();
				car_near_cross[i] = car_tmp;
				if (car_tmp != nullptr) ++num_of_car;
			}
			if (num_of_car == 0) {
				//���·��û�г������˳���������һ��ѭ��
				continue;
			}
			//2.����·�ڳ�������·��
			for (auto &car : car_near_cross) {
				if (car == nullptr) continue;
				car->get_next_road_id();
			}
			vector<Car *> last_car_near_cross(num_of_valid_road_state);
			//3.ѭ�����µ�ǰ·������Ӧ�ĳ���
			while (last_car_near_cross != car_near_cross) {	//������ѭ��·�ڳ���״̬����ʱ���˳�ѭ��
				last_car_near_cross = car_near_cross;
				for (int i = 0; i < num_of_valid_road_state; i++) {
					Car* car = car_near_cross[i];//����·id��ȡ������Ϣ
					int look_car_id;
					if (car != nullptr) look_car_id = car->_car_id;
					if (car == nullptr) continue;//������Ϊ��ʱ��������һ��ѭ��
					if (!car->is_first_in_road()) {//������ǳ�����һ����
						car_near_cross[i] = nullptr;
						Road_state *road_state = car->get_current_road_state_pointer();
						deque<Car*> &cars_in_lane = road_state->_cars_in_road[car->_lanes_id];
						//��������WAIT����ΪWAIT_CAR
						for (auto &car_in_lane : cars_in_lane) {
							if (car_in_lane->_state == WAIT)  car_in_lane->_state = WAIT_CAR;
						}
						//���к���������END,��������һ��ѭ��
						run_car_in_a_lane(cars_in_lane);
						////��ȡ��ǰ��·����һ�����ȼ���ߵĳ���������car_near_cross
						Car* car_of_first_class = road_state->get_first_waiting_car();

						int car_id_look;
						if (car_of_first_class != nullptr) {
							car_id_look = car_of_first_class->_car_id;
						}
						//�����Ϊ�գ������next_road_id
						if (car_of_first_class != nullptr) car_of_first_class->get_next_road_id();
						else {
							for (auto &lane : road_state->_cars_in_road) {
								run_car_in_a_lane(lane);
							}
						}
						car_near_cross[i] = car_of_first_class;//����car_near_cross
						continue;
					}
					Road_state *road_state = car->get_current_road_state_pointer();//�洢��ǰ������road_state,���ں�������pop
					//�����ȼ���������·�ڵĳ���������
					bool flag = car->is_prior_to_others(car_near_cross, cross.second);
					if (flag == true) {//�����������
						//Road_state *road_state = car->get_current_road_state_pointer();
						int lane_of_car = car->_lanes_id;
						int status = car->moveToNextRoad();//���г�����ע��˴���_num_of_waiting_car�������޸�
						if (status == WAIT) {//OUT:���ǰ������������ǰ������WAIT״̬����ɨ����һ����·�ĳ���
							continue;
						}
						else if (status == ARRIVED) {//��������յ�
							--_num_of_cars_not_arrived;
							--_num_of_wainting_cars;
							run_car_in_a_lane(road_state->_cars_in_road[lane_of_car]);//���������WAIT_CAR,���У����û�У�����������0						
							cars_num_on_road--;
							cars_num_arrived++;
							//Car* car_of_first_class = road_state->get_first_waiting_car();
							////����car_near_cross,���ʿ�ָ��
							////�����Ϊ�գ������next_road_id
							//if (car_of_first_class != nullptr) car_of_first_class->get_next_road_id();
							//car_near_cross[i] = car_of_first_class;//����car_near_cross
						}
						else if (status == WAIT_CAR) {//�������û���ܹ�ʻ��·�ڣ�����ͣ���˵�·ĩ��
							--_num_of_wainting_cars;
							int trans_wait_to_waitcar_count = 0;
							//������WAIT�������WAIT_CAR,�������ƶ���END
							if ((road_state->_cars_in_road[lane_of_car]).size() - 1 == 0) {
								//�������û�г���,��������car_neaar_cross
							}
							else {
								//����г��������е�WAIT���ΪWAIT_CAR,����������
								for (auto car : (road_state->_cars_in_road[lane_of_car])) {
									if (car->_state == WAIT) car->_state = WAIT_CAR;
									++trans_wait_to_waitcar_count;
								}
								if (trans_wait_to_waitcar_count != 0) {
									deque<Car*> &cars_in_lane = road_state->_cars_in_road[lane_of_car];
									int run_state = run_car_in_a_lane(cars_in_lane);
								}
							}

						}
						else if (status == END) {
							//�����ƶ������С����
							--_num_of_wainting_cars;//!!!!!!!!!!!!!!!!!
							//��ȡ��ǰ������һ������״̬

							Car* next_car;
							if ((road_state->_cars_in_road[lane_of_car]).empty()) {
								next_car = nullptr;
							}
							else {
								next_car = (road_state->_cars_in_road[lane_of_car]).front();
							}
							//���ݵ�ǰ������һ����ȷ������ʽ
							if (next_car == nullptr || next_car->_state == WAIT || next_car->_state == END) {//����ó������ƶ����������car_near_cross
								//��������
							}
							else if (next_car->_state == WAIT_CAR) {//������������ƶ��������к���������ʻ��END�ĳ���
								//deque<Car*> &cars_in_lane = (next_car->get_current_road_state_pointer())->_cars_in_road[lane_of_car];
								deque<Car*> &cars_in_lane = road_state->_cars_in_road[lane_of_car];
								int run_state = run_car_in_a_lane(cars_in_lane);
							}
							else {
								cout << "run_waiting_car() Error: Wrong car state" << endl;
							}
							////��ȡ��ǰ��·����һ�����ȼ���ߵĳ���
							//Car* car_of_first_class = road_state->get_first_waiting_car();
							////�����Ϊ�գ������next_road_id
							//if (car_of_first_class != nullptr) car_of_first_class->get_next_road_id();
							//car_near_cross[i] = car_of_first_class;//����car_near_cross
						}
						else {
							cout << "run_waiting_car : Impatible status" << endl;
						}
						////��ȡ��ǰ��·����һ�����ȼ���ߵĳ���������car_near_cross
						Car* car_of_first_class = road_state->get_first_waiting_car();
						//�����Ϊ�գ������next_road_id
						if (car_of_first_class != nullptr) car_of_first_class->get_next_road_id();
						else {//���Ϊ�գ���Ӧ���µ�ǰ��·����WAIT_CAR����
							for (auto &lane : road_state->_cars_in_road) {
								run_car_in_a_lane(lane);
							}
						}
						car_near_cross[i] = car_of_first_class;//����car_near_cross
					}
					else {//������ȼ���������·�ĳ�����
						continue;//��������
					}
				}
			}
		}
	}
	return 0;
}

//��ȡ�����п�����·�ĳ���
int Magic_Box::get_cars_to_go() {
	for (auto &car_pair : _cars_all) {
		Car * car = &(car_pair.second);
		if (car->_state == STOP && _time >= car->_time_to_go) {
			_cars_to_go.insert(pair<int, Car*>(car->_car_id, car));
			car->_state = READY;
		}
	}
	return 0;
}
//��������·�ĳ��������·�����������·���ƺ���·ʱ��
int Magic_Box::drive_car_in_garage() {
	vector<int> cars_to_delete;
	for (auto &car_pair : _cars_to_go) {
		Car* car = car_pair.second;

		car->get_planned_route(_cross_all, _roads_all);

		int state = car->get_next_road_id(_roads_all);//����_road_id������road_ptr��_direction
		if (state == 0) {//������ʺϳ���
			(car->_time_to_go) = 5 + _time;
			--car->_location_in_route;
			_cars_to_go.erase(car->_car_id);
			car->_next_road_id = -1;
			continue;
		}
		Road_state * road_state = car->get_current_road_state_pointer();
		int lanes = 0, position_can_insert = 0;

		int road_flag = road_state->get_position_to_insert(lanes, position_can_insert);
		if (lanes == -1 || position_can_insert == -1) {//���û�еط��ų�
			car->_time_to_go = _time + 1;
			//++(car->_time_to_go);
			--car->_location_in_route;
			car->_next_road_id = -1;
			continue;
		}


		//����Car�ĳ�Ա����
		car->_v_current = (car->_max_v < car->_road_ptr->_max_v) ? car->_max_v : car->_road_ptr->_max_v;
		car->_lanes_id = lanes;

		int expect_position = car->_road_ptr->_road_length - car->_v_current;
		if (expect_position < 0) cout << "drive_car_in_garage Error�� Invalid speed" << endl;
		/********************************************************************************************/
		//��������������������룬ѡ������ȴ�
		if (START_CONTROL) {
			if (expect_position <= position_can_insert) {
				car->_time_to_go = _time + 1;
				//++(car->_time_to_go);
				--car->_location_in_route;
				car->_next_road_id = -1;
				continue;
			}
		}
		/********************************************************************************************/
		car->_position = (expect_position > position_can_insert) ? expect_position : position_can_insert;
		car->_time_to_go = _time;
		car->_state = END;

		car->Routine.push_back(car->_road_id);

		//����road_state
		deque<Car*> * cars_in_road = &(road_state->_cars_in_road[lanes]);

		((road_state->_cars_in_road)[lanes]).push_back(car);//Ϊʹ�ø���䣬��Ϊ�Զ�����ඨ����������

		//BRC���㣬ͳ�Ƶ�·�ڳ��������������µ�tiֵ
		road_state->_Q_brc++;
		road_state->get_brc_ti();

		cars_to_delete.push_back(car->_car_id);
	}
	//�Ƴ��Ѿ�����ĳ���
	//ע�⣺ɾ�����ܷ���forѭ����
	for (auto id : cars_to_delete) {
		_cars_to_go.erase(id);
	}
	return 0;
}

//���ڵ�·����Ϣת���ɵ�··����Ϣ
void Magic_Box::turn_node_to_way()
{
	//	int i = 0;
	for (auto & cross : _cross_all)	//�������нڵ�
	{
		for (auto & route_tmp1 : cross.second.destination_routes)	//�����ڵ㵽�������нڵ������·��
		{
			vector<vector<int>> ways_1;
			int id = route_tmp1.first;
			for (auto & route_tmp2 : route_tmp1.second)	//������һ���ڵ����������·��
			{
				vector<int> ways_2;
				for (vector<int>::reverse_iterator it = route_tmp2.rbegin(); it < route_tmp2.rend() - 1; it++)	//����һ������·��
				{
					for (int i = 0; i < 4; i++)
					{
						if (_cross_all.at(*it)._node_id_adjacent[i] == *(it + 1))
							ways_2.push_back(_cross_all.at(*it)._road_id_adjacent[i]);
					}
				}
				ways_1.push_back(ways_2);
			}
			cross.second.destination_way[id] = ways_1;
		}
		//		cout << ++i << endl;
	}
}

//ʹ�øú���֮ǰ��������turn_node_to_way()����������·-��·��·��ת������
void Magic_Box::node_routes_inversion()
{
	for (auto & cross : _cross_all)	//�������нڵ�
	{
		map<int, vector<vector<int>>> routes_tmp;
		for (auto & route_tmp1 : cross.second.destination_routes)	//�����ڵ㵽�������нڵ������·��
		{
			vector<vector<int>> ways_1;
			int id = route_tmp1.first;
			for (auto & route_tmp2 : route_tmp1.second)	//������һ���ڵ����������·��
			{
				vector<int> ways_2;
				for (vector<int>::reverse_iterator it = route_tmp2.rbegin(); it < route_tmp2.rend(); it++)	//����һ������·��
					ways_2.push_back(*it);
				ways_1.push_back(ways_2);
			}
			routes_tmp[id] = ways_1;
		}
		cross.second.destination_routes.clear();
		cross.second.destination_routes = routes_tmp;
	}
}

void Magic_Box::update_routine_stored_in_cross() {
	//int i = 0;
	for (auto & cross_tmp1 : _cross_all)	//�����ڵ�
	{
		for (auto & cross_tmp2 : _cross_all)	//Ŀ�Ľڵ�
		{
			multiShortestPath s;
			s.makeMap(_roads_all);
			s.dijkstra(cross_tmp1.second._node_id, cross_tmp2.second._node_id);
			cross_tmp1.second.destination_routes[cross_tmp2.second._node_id] = s.display_all();
		}
	//	cout << i++ << endl;
	}
	//·��ת�� ·��->·��->·��  ת��  ��·->��·->��·
	turn_node_to_way();
	get_weight_of_graph();
	//��ԭ�����ŵġ�·��-·��-·�ڡ�·��ת��
	node_routes_inversion();
/*
	for (int m = 1; m <= 1; m++)
	{
		for (int i = 1; i <= 16; i++)
		{
			cout << _cross_all.at(m).destination_routes.at(i).size() << endl;
			cout << _cross_all.at(m).destination_routes.at(i)[0].size() << endl;
			for (int j = 0; j < _cross_all.at(m).destination_routes.at(i)[0].size(); j++)
			{
				cout << _cross_all.at(m).destination_routes.at(i)[0][j] << " ";
			}
			cout << endl;
			cout << _cross_all.at(m).destination_way.at(i).size() << endl;
			cout << _cross_all.at(m).destination_way.at(i)[0].size() << endl;
			for (int j = 0; j < _cross_all.at(m).destination_way.at(i)[0].size(); j++)
			{
				cout << _cross_all.at(m).destination_way.at(i)[0][j] << " ";
			}
			cout << endl;
		}
	}
*/
}

void Magic_Box::output_answer(string out_file_name) {
	ofstream out_file(out_file_name);
	for (auto &car : _cars_all) {
		vector<int> &route = car.second.Routine;
		out_file << "(" << car.second._car_id << "," << car.second._time_to_go << ",";
		for (auto iter = route.begin(); iter < route.end() - 1; iter++) {
			out_file << *iter << ",";
		}
		out_file << route.back() << ")" << endl;
	}
	out_file.close();
	return;
}

int Magic_Box::RUN() {
	int flag = 0;
	run_all_cars_on_road();
	flag = run_waiting_car();//������ֱջ�������-1
	get_cars_to_go();
	if (cars_num_on_road < 500)
	{
		drive_car_in_garage();
	}
	return flag;//������ֱջ�������-1
}
//���¹滮����
void Magic_Box::re_arrange_looped_cars() {
	for (auto &car_pair : _cars_in_loop) {
		auto car = car_pair.second;
		Cross *cross;
		if (car->_direction == 0) {
			cross = car->_road_ptr->_end_node_ptr;
		}
		else {
			cross = car->_road_ptr->_start_node_ptr;
		}
		auto &routes = cross->destination_way.at(car->_end_id);
		int n = routes.size();
		//���ѡȡһ��
		if (n > 1) {
			//srand((unsigned)time(NULL));//��ȡ��������
			int k = rand() % n;
			car->_route_pre_planned = routes[k];
			car->_location_in_route = 0;
			car->_next_road_id = -1;
		}
		else {
			int road_id = car->_road_ptr->_road_id;
			int pre_next_road_id = car->_next_road_id;
			for (auto road_id_adjacent : cross->_road_id_adjacent) {
				if (road_id_adjacent == road_id || road_id_adjacent == pre_next_road_id) continue;
				if (_roads_all.at(road_id)._start_node_ptr == cross) {
					auto cross_next = _roads_all.at(road_id)._end_node_ptr;
					if (cross_next->destination_way.find(car->_end_id) != cross_next->destination_way.end()) {
						routes = cross_next->destination_way.at(car->_end_id);
						n = routes.size();
						int k = rand() % n;
						car->_route_pre_planned = routes[k];
						car->_location_in_route = 0;
						car->_next_road_id = road_id_adjacent;
					}
				}
				else {
					auto cross_next = _roads_all.at(road_id)._start_node_ptr;
					if (cross_next->destination_way.find(car->_end_id) != cross_next->destination_way.end()) {
						routes = cross_next->destination_way.at(car->_end_id);
						n = routes.size();
						int k = rand() % n;
						car->_route_pre_planned = routes[k];
						car->_location_in_route = 0;
						car->_next_road_id = road_id_adjacent;
					}
				}
			}
		}
	}
	//srand((unsigned)time(NULL));//��ȡ��������
	//int k = rand() % n;
}

int Magic_Box::car_on_road_num()
{
	int num = 0;
	int arrived_car = 0;
	for (auto &car : _cars_all)
	{
		if (car.second._state == END || car.second._state == WAIT || car.second._state == WAIT_CAR)
			num++;
		if (car.second._state == ARRIVED)
			arrived_car++;
	}
	//cout << "���공����Ϊ��" << arrived_car << endl;
	return num;
}

void Magic_Box::get_weight_of_graph()
{
	int lengh_of_all_roads = 0;
	int weight;
	for (auto & road : _roads_all)
	{
		lengh_of_all_roads += road.second._road_length * road.second._num_of_lanes;
		if (road.second._is_2_way)
			lengh_of_all_roads += road.second._road_length * road.second._num_of_lanes;
	}

	weight_of_graph = lengh_of_all_roads * 0.1;
}