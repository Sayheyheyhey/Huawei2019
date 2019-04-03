#include "Car.h"
#include "Road.h"
#include <ctime>
Car::Car(int car_id, int start_id, int end_id, int max_v, int time_to_go)
{
	_car_id = car_id;
	_start_id = start_id;
	_end_id = end_id;
	_max_v = max_v;
	_time_to_go = time_to_go;
};

Road_state * Car::get_next_road_state_pointer() {
	Cross * cross = nullptr;
	if (_direction == FORWARD) cross = _road_ptr->_end_node_ptr;
	else if (_direction == BACK) cross = _road_ptr->_start_node_ptr;
	else {
		cout << "get_next_road_state_pointer() ERROR" << endl;
		return nullptr;
	}
	int i = 0, is_found = 0;

	for (; i < 4; i++) {//������_next_road_idƥ���ָ�룬�����±�
		if (cross->_road_id_adjacent[i] == _next_road_id) {
			is_found = 1;
			break;
		}
	}
	//�ҵ��򷵻�road_state��ָ��
	if (is_found == 1) {
		Road_state * road_state = cross->_road_state_ptr_adjacent_out[i];//��ȡ��·�ڷ����Road_state
		if (road_state == nullptr)
			cout << "Cannot find the road state in Cross, the next_road_id or road_state_ptr_adjacent_out may be wrong" << endl;
		return road_state;
	}
	else {
		cout << "Mismatch in get_next_road_state_pointer()" << endl;
		return nullptr;
	};
}
Road_state * Car::get_current_road_state_pointer() {
	if (_direction == FORWARD) return &(_road_ptr->_forward);
	else if (_direction == BACK) return &(_road_ptr->_backward);
	else {
		cout << "get_current_road_state_pointer() ERROR: car's _road_ptr is NULL, or car's direction is not update" << endl;
		return nullptr;
	}
}

ostream & operator << (ostream &out, Car &obj) {
	out << "Original Data:{ ";
	out << "car_id=" << obj._car_id << "; ";
	out << "start_id=" << obj._start_id << "; ";
	out << "end_id=" << obj._end_id << "; ";
	out << "max_v=" << obj._max_v << "; ";
	out << "time_to_go=" << obj._time_to_go << "; ";
	out << "}";

	out << "Car's State:{ ";
	out << "road_id=" << obj._road_id << "; ";
	out << "v_current=" << obj._v_current << "; ";
	out << "direction=" << obj._direction << "; ";
	out << "state=" << obj._state << "; ";
	out << "lanes_id=" << obj._lanes_id << "; ";
	out << "next_road_id=" << obj._next_road_id << "; ";
	out << "position = " << obj._position << ";";
	out << "}";

	return out;
}

//��Ҫ��ǰһ����_next_road_id��ֵ
//�ƶ�����ͨ��·�ڵ���һ����·�������һ����·�������򷵻�WAIT
//���ظó�Ӧ�����е���״̬
int Car::moveToNextRoad() {
	/*
	Car * first_car_in_road = this->get_current_road_state_pointer()->_cars_in_road[_lanes_id].front();
	if (this != first_car_in_road) {//����ó����ڵ�·ǰ���г���ʻ��END
		Road_state *road_state = get_current_road_state_pointer();
		deque<Car*> &cars_in_lane = road_state->_cars_in_road[_lanes_id];
		//��������WAIT����ΪWAIT_CAR
		for (auto &car_in_lane : cars_in_lane) {
			if (car_in_lane->_state == WAIT)  car_in_lane->_state = WAIT_CAR;
		}
		return STOP;
	}*/
	//�����һ�����Ե����յ�,����ARRIVED����������deque
	if (_next_road_id == ARRIVED) {
		//Routine.push_back(_road_id);
		Road_state * road_state = get_current_road_state_pointer();
		road_state->_cars_in_road[_lanes_id].pop_front();
		//BRC���㣬ͳ�Ƶ�·�ڳ��������������µ�tiֵ
		road_state->_Q_brc--;
		road_state->get_brc_ti();


		_state = ARRIVED;
		return ARRIVED;
	}
	Road_state * next_road_state = get_next_road_state_pointer();
	Road_state * road_state = get_current_road_state_pointer();
	if (next_road_state == nullptr) {
		cout << "Error in moveToNextRoad()��invalid pointer�� maybe the car should go to end" << endl;
	}

	int lanes, position_can_insert;		//��һ�����ɲ����λ��
	int state_of_insert = next_road_state->get_position_to_insert(lanes, position_can_insert);		//��ȡǰһ������״̬�����û�г��򷵻�-1�����Ϳɲ����λ��

	int max_distance = get_max_distance_to_move(next_road_state->get_max_v_of_road(), next_road_state->get_road_length());	//��ȡ������·�ں������ʻ��������

	//���г���
	if (state_of_insert == STOP) {//���ǰ����·ĩ���г����ڵȴ�״̬���򷵻�WAIT
		return WAIT;
	}
	else if (lanes == -1 && position_can_insert == -1) {//������·����
		if (state_of_insert == END) {//���ǰ��ֹͣ���򽫳�ͣ�ڵ�·��ͷ,����WAIT_CAR;���ڽ�������WAIT��Ϊ����������END�ĳ�����Ȼ����д���
			_position = 0;
			_state = END;
			return WAIT_CAR;
		}
		else {
			cout << "moveToNextRoad Error: Somethin wrong when next road is blocked" << endl;
			return END;
		}
	}
	else {
		//�ж��Ƿ�ᱻǰ���赲
		int expect_position;
		if (max_distance == 0) expect_position = next_road_state->get_road_length() - 1;
		else if (max_distance < 0) cout << "Error in position calculation!" << endl;
		else expect_position = next_road_state->get_road_length() - max_distance;	//����������ʻ����Զλ�ã�position������С��

		if (position_can_insert >= expect_position) {	//�����ǰ���赲
			if (state_of_insert == END) {//ͣ��ǰ����
				set_car_and_update_road(road_state, next_road_state, lanes, position_can_insert);
				return END;
			}
			else if (state_of_insert == WAIT || state_of_insert == WAIT_CAR || state_of_insert) {//����赲�ĳ�ΪWAIT��WAIT_CAR����������
				return WAIT;
			}
			else {
				cout << "moveToNextRoad : The state_of_insert is invalid" << endl;
				return END;
			}
		}
		else {//���ǰ����·���赲
			set_car_and_update_road(road_state, next_road_state, lanes, expect_position);
			return END;
		}
	}
}

//��ȡ������·�ں������ʻ��������
int Car::get_max_distance_to_move(int next_road_max_v, int length_of_next_road) {

	int distance_left_in_road = _position;	//��ǰ��·��Ҫ�н��ľ���
	if (_v_current <= _position) {
		cout << "get_max_distance_to_move Error: The _v_current must bigger than the _position" << endl;
	}

	int v_current_in_next_road = (_max_v > next_road_max_v) ? next_road_max_v : _max_v;

	int distance = 0;
	distance = v_current_in_next_road - distance_left_in_road;

	if (distance > length_of_next_road) cout << "get_max_distance_to_move Error: Calculation is wrong" << endl;
	if (distance < 0) distance = 0;
	return distance;
}
//���ó�����ָ��λ�ã����Ҹ��µ�·״̬
int Car::set_car_and_update_road(Road_state * current_road_state, Road_state * next_road_state, int lanes_to_insert, int position_to_insert) {
	if (current_exception != nullptr)
		current_road_state->_cars_in_road[_lanes_id].pop_front();
	//BRC���㣬ͳ�Ƶ�·�ڳ��������������µ�tiֵ
	current_road_state->_Q_brc--;
	current_road_state->get_brc_ti();


	_lanes_id = lanes_to_insert;
	_position = position_to_insert;
	_road_id = _next_road_id;
	_next_road_id = -1;	//��ʶ����-1��ʾδ����
	int max_v_next_road = next_road_state->get_max_v_of_road();
	_v_current = (max_v_next_road < _max_v) ? max_v_next_road : _max_v;
	_road_ptr = next_road_state->_road_ptr;
	_direction = next_road_state->_direction;
	_state = END;

	next_road_state->_cars_in_road[lanes_to_insert].push_back(this);
	//BRC���㣬ͳ�Ƶ�·�ڳ��������������µ�tiֵ
	next_road_state->_Q_brc++;
	next_road_state->get_brc_ti();

	Routine.push_back(_road_id);//���·��

	return 0;
}

//��ó�����ǰ���Ʒ�����Ҫ��get_next_road_id֮������
int Car::get_direction() {
	if (_next_road_id == -1) {
		cout << "There is no next road in the plann, maybe there is a problem with dealing with car that could arrive the end" << endl;
		return -1;
	}
	else if (_next_road_id == ARRIVED) {
		return ARRIVED;
	}
	int road_order = -1, next_road_order = -1;
	Road_state *road_state = get_current_road_state_pointer();
	Road * road = road_state->_road_ptr;
	Cross * cross = (road_state->_direction == FORWARD) ? road->_end_node_ptr : road->_start_node_ptr;

	for (int i = 0; i < 4; i++) {
		if (cross->_road_id_adjacent[i] == this->_road_id) road_order = i;
		if (cross->_road_id_adjacent[i] == this->_next_road_id) next_road_order = i;
	}
	if (road_order == -1 || next_road_order == -1) {
		cout << "Car::get_direction Error: something wrong in the relation between car, road and cross" << endl;
	}
	int distance = next_road_order - road_order;
	if (distance == 1 || distance == -3) return LEFT;
	else if (distance == 2 || distance == -2) return STRAIGHT;
	else if (distance == 3 || distance == -1) return RIGHT;
	else {
		cout << "Car::get_direction Error" << endl;
		return -1;
	}
}

//�жϵ�ǰ���������ȼ��Ƿ����·�ڴ�����������
bool Car::is_prior_to_others(vector<Car*>& cars_near_cross, Cross &cross) {
	/********���γ��������ж��Ƿ��ж�Ӧ��ϵ�Ĵ����ύǰ����ɾ��**************/
	if (_next_road_id == ARRIVED)
		return true;
	Road_state *road_state = get_current_road_state_pointer();
	Road * road = road_state->_road_ptr;
	Cross * cross_calculate = (road_state->_direction == FORWARD) ? road->_end_node_ptr : road->_start_node_ptr;
	if (cross_calculate != &cross) {
		cout << "Error: Please check the same part in is_prior_to_others and get_direction" << endl;
	}
	/**************************************************************************/
	int next_road_id = this->_next_road_id;
	vector <Car *> conflict_cars;
	for (auto another_car : cars_near_cross) {
		if (another_car == this) continue;
		if (another_car == nullptr) continue;
		if (another_car->_next_road_id == next_road_id) {
			conflict_cars.push_back(another_car);
		}
	}
	if (conflict_cars.size() == 0) return true;
	else {
		//�����ͻ�ĳ����ȼ����ڱ������򷵻�false
		for (auto car_conflict : conflict_cars) {
			if (car_conflict->get_direction() < this->get_direction()) {
				return false;
			}
			else if (car_conflict->get_direction() == this->get_direction()) {
				cout << "Wrong in is_prior_to_others, two cars can't go to the same road in same direction" << endl;
			}
		}
		return true;//���򷵻�true
	}
}

////����������Ϣ�͵�·״������_next_road_id��ֵ��������������յ㣬����ARRIVED
//int Car::get_next_road_id() {
//	//2.״̬ΪWAITʱ������_next_road_id,
//	//3.��һ�������յ�ʱ������ARRIVED
//	//4.�����·����ӵ�£���Ҫ����һ��ʱ��ʱ������0���������з���1
//	//5.ʧ���򷵻�-1
//	if (_next_road_id != -1) {//�����һ��·�ڵ���ʱ������δ��ͨ��·�ڣ���_next_road_idδ���£�ʹ�ô˲�����_route_pre_planned��pop����
//		return 0;
//	}
//	if (_route_pre_planned.size() == 0) {
//		_next_road_id = ARRIVED;
//		return ARRIVED;
//	}
//	else {
//		_next_road_id = _route_pre_planned.front();
//		_route_pre_planned.pop();
//	}
//	return 0;
//}
//��·ʱ
/*
int Car::get_next_road_id(map<int,Road> &roads_all){
	//��Ҫ���жϣ��ʲ��ʺϳ�����������ʺ��򷵻�0
	if (_state == READY) {
		_road_id = _route_pre_planned.front();
		_route_pre_planned.pop();
		_road_ptr = &(roads_all.at(_road_id));
		if (_start_id == _road_ptr->_start_node_id)
			_direction = FORWARD;
		else if (_start_id == _road_ptr->_end_node_id)
			_direction = BACK;
		else
			cout << "get_next_road_id Error: Mismatch between path and cross" << endl;
	}
	else {
		cout << "get_next_road_id Error: The car must be in garage now" << endl;
	}
	return 1;
}
*/
int Car::get_next_road_id(map<int, Road> &roads_all) {
	if (_state == READY) {
		_road_id = _route_pre_planned[_location_in_route];
		++_location_in_route;

		_road_ptr = &(roads_all.at(_road_id));
		if (_start_id == _road_ptr->_start_node_id)
			_direction = FORWARD;
		else if (_start_id == _road_ptr->_end_node_id)
			_direction = BACK;
		else
			cout << "get_next_road_id Error: Mismatch between path and cross" << endl;
	}
	else {
		cout << "get_next_road_id Error: The car must be in garage now" << endl;
	}
	return 1;
}
int Car::get_next_road_id() {
	//2.״̬ΪWAITʱ������_next_road_id,
	//3.��һ�������յ�ʱ������ARRIVED
	//4.�����·����ӵ�£���Ҫ����һ��ʱ��ʱ������0���������߷���1
	//5.ʧ���򷵻�-1
	if (_next_road_id != -1) {//�����һ��·�ڵ���ʱ������δ��ͨ��·�ڣ���_next_road_idδ���£�ʹ�ô˲�����_route_pre_planned��pop����
		return 0;
	}
	if (_location_in_route == _route_pre_planned.size()) {
		_next_road_id = ARRIVED;
		return ARRIVED;
	}
	else {
		_next_road_id = _route_pre_planned[_location_in_route];
		++_location_in_route;
	}
	return 0;
}


void Car::get_planned_route(map<int, Cross> &cross, map<int, Road> &road) {
	Cross * start_node = &cross.at(_start_id);
	vector<vector<int>> *routes_ways = &(start_node->destination_way.at(_end_id));
	vector<vector<int>> *routes_node = &(start_node->destination_routes.at(_end_id));
	int n = routes_ways->size();
	if (n == 1) {
		_route_pre_planned = (*routes_ways)[0];
	}
	else {
		int min_num_of_cars = INT_MAX;
		int bast_routes_id = 0;
		for (int i = 0; i < n; i++)
		{
			int num_of_cars = 0;
			for (int j = 0; j < (*routes_ways)[i].size(); j++)
			{
				if (cross.at((*routes_node)[i][j])._node_id == road.at((*routes_ways)[i][j])._start_node_id)
					for (int m = 0; m < road.at((*routes_ways)[i][j])._forward._cars_in_road.size(); m++)
						num_of_cars = road.at((*routes_ways)[i][j])._forward._cars_in_road.at(m).size() + num_of_cars;
				else
					for (int m = 0; m < road.at((*routes_ways)[i][j])._backward._cars_in_road.size(); m++)
						num_of_cars = road.at((*routes_ways)[i][j])._backward._cars_in_road.at(m).size() + num_of_cars;
			}
			if (min_num_of_cars > num_of_cars)
				bast_routes_id = i;
		}
	//	srand((unsigned)time(NULL));//��ȡ��������
	//	int k = rand() % n;
		_route_pre_planned = (*routes_ways)[bast_routes_id];
	}
}
//�ж��Ƿ��ǳ����ڵĵ�һ����
bool Car::is_first_in_road() {
	Car * first_car_in_road = this->get_current_road_state_pointer()->_cars_in_road[_lanes_id].front();
	return this == first_car_in_road;
}