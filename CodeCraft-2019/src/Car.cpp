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

	for (; i < 4; i++) {//搜索与_next_road_id匹配的指针，保存下标
		if (cross->_road_id_adjacent[i] == _next_road_id) {
			is_found = 1;
			break;
		}
	}
	//找到则返回road_state的指针
	if (is_found == 1) {
		Road_state * road_state = cross->_road_state_ptr_adjacent_out[i];//获取出路口方向的Road_state
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

//需要在前一步将_next_road_id赋值
//移动汽车通过路口到下一条道路，如果下一条道路堵塞，则返回WAIT
//返回该车应当运行到的状态
int Car::moveToNextRoad() {
	/*
	Car * first_car_in_road = this->get_current_road_state_pointer()->_cars_in_road[_lanes_id].front();
	if (this != first_car_in_road) {//如果该车所在道路前方有车行驶至END
		Road_state *road_state = get_current_road_state_pointer();
		deque<Car*> &cars_in_lane = road_state->_cars_in_road[_lanes_id];
		//将后续的WAIT都置为WAIT_CAR
		for (auto &car_in_lane : cars_in_lane) {
			if (car_in_lane->_state == WAIT)  car_in_lane->_state = WAIT_CAR;
		}
		return STOP;
	}*/
	//如果下一步可以到达终点,返回ARRIVED，将车弹出deque
	if (_next_road_id == ARRIVED) {
		//Routine.push_back(_road_id);
		Road_state * road_state = get_current_road_state_pointer();
		road_state->_cars_in_road[_lanes_id].pop_front();
		//BRC计算，统计道路内车的数量，计算新的ti值
		road_state->_Q_brc--;
		road_state->get_brc_ti();


		_state = ARRIVED;
		return ARRIVED;
	}
	Road_state * next_road_state = get_next_road_state_pointer();
	Road_state * road_state = get_current_road_state_pointer();
	if (next_road_state == nullptr) {
		cout << "Error in moveToNextRoad()，invalid pointer， maybe the car should go to end" << endl;
	}

	int lanes, position_can_insert;		//下一车道可插入的位置
	int state_of_insert = next_road_state->get_position_to_insert(lanes, position_can_insert);		//获取前一辆车的状态（如果没有车则返回-1），和可插入的位置

	int max_distance = get_max_distance_to_move(next_road_state->get_max_v_of_road(), next_road_state->get_road_length());	//获取车辆过路口后可以行驶的最大距离

	//运行车辆
	if (state_of_insert == STOP) {//如果前方道路末端有车处于等待状态，则返回WAIT
		return WAIT;
	}
	else if (lanes == -1 && position_can_insert == -1) {//如果因道路挤满
		if (state_of_insert == END) {//如果前车停止，则将车停在道路尽头,返回WAIT_CAR;便于将后续的WAIT视为可以运行至END的车辆，然后进行处理
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
		//判断是否会被前车阻挡
		int expect_position;
		if (max_distance == 0) expect_position = next_road_state->get_road_length() - 1;
		else if (max_distance < 0) cout << "Error in position calculation!" << endl;
		else expect_position = next_road_state->get_road_length() - max_distance;	//车辆可以行驶的最远位置（position数字最小）

		if (position_can_insert >= expect_position) {	//如果被前车阻挡
			if (state_of_insert == END) {//停在前车后方
				set_car_and_update_road(road_state, next_road_state, lanes, position_can_insert);
				return END;
			}
			else if (state_of_insert == WAIT || state_of_insert == WAIT_CAR || state_of_insert) {//如果阻挡的车为WAIT或WAIT_CAR，则不做处理
				return WAIT;
			}
			else {
				cout << "moveToNextRoad : The state_of_insert is invalid" << endl;
				return END;
			}
		}
		else {//如果前方道路无阻挡
			set_car_and_update_road(road_state, next_road_state, lanes, expect_position);
			return END;
		}
	}
}

//获取车辆过路口后可以行驶的最大距离
int Car::get_max_distance_to_move(int next_road_max_v, int length_of_next_road) {

	int distance_left_in_road = _position;	//当前道路需要行进的距离
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
//放置车辆到指定位置，并且更新道路状态
int Car::set_car_and_update_road(Road_state * current_road_state, Road_state * next_road_state, int lanes_to_insert, int position_to_insert) {
	if (current_exception != nullptr)
		current_road_state->_cars_in_road[_lanes_id].pop_front();
	//BRC计算，统计道路内车的数量，计算新的ti值
	current_road_state->_Q_brc--;
	current_road_state->get_brc_ti();


	_lanes_id = lanes_to_insert;
	_position = position_to_insert;
	_road_id = _next_road_id;
	_next_road_id = -1;	//标识符，-1表示未更新
	int max_v_next_road = next_road_state->get_max_v_of_road();
	_v_current = (max_v_next_road < _max_v) ? max_v_next_road : _max_v;
	_road_ptr = next_road_state->_road_ptr;
	_direction = next_road_state->_direction;
	_state = END;

	next_road_state->_cars_in_road[lanes_to_insert].push_back(this);
	//BRC计算，统计道路内车的数量，计算新的ti值
	next_road_state->_Q_brc++;
	next_road_state->get_brc_ti();

	Routine.push_back(_road_id);//添加路径

	return 0;
}

//获得车辆当前形势方向，需要自get_next_road_id之后运行
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

//判断当前车辆的优先级是否高于路口处的其他车辆
bool Car::is_prior_to_others(vector<Car*>& cars_near_cross, Cross &cross) {
	/********本段程序用来判断是否有对应关系的错误，提交前可以删除**************/
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
		//如果冲突的车优先级高于本车，则返回false
		for (auto car_conflict : conflict_cars) {
			if (car_conflict->get_direction() < this->get_direction()) {
				return false;
			}
			else if (car_conflict->get_direction() == this->get_direction()) {
				cout << "Wrong in is_prior_to_others, two cars can't go to the same road in same direction" << endl;
			}
		}
		return true;//否则返回true
	}
}

////根据先验信息和道路状况，对_next_road_id赋值，如果即将到达终点，返回ARRIVED
//int Car::get_next_road_id() {
//	//2.状态为WAIT时，设置_next_road_id,
//	//3.下一步到达终点时，返回ARRIVED
//	//4.如果道路过于拥堵，需要后推一个时刻时，返回0；政策运行返回1
//	//5.失败则返回-1
//	if (_next_road_id != -1) {//如果上一步路口调度时，车辆未能通过路口，则_next_road_id未更新，使用此步跳过_route_pre_planned的pop操作
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
//上路时
/*
int Car::get_next_road_id(map<int,Road> &roads_all){
	//需要加判断，适不适合出发，如果不适合则返回0
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
	//2.状态为WAIT时，设置_next_road_id,
	//3.下一步到达终点时，返回ARRIVED
	//4.如果道路过于拥堵，需要后推一个时刻时，返回0；运行政策返回1
	//5.失败则返回-1
	if (_next_road_id != -1) {//如果上一步路口调度时，车辆未能通过路口，则_next_road_id未更新，使用此步跳过_route_pre_planned的pop操作
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
	//	srand((unsigned)time(NULL));//获取真的随机数
	//	int k = rand() % n;
		_route_pre_planned = (*routes_ways)[bast_routes_id];
	}
}
//判断是否是车道内的第一辆车
bool Car::is_first_in_road() {
	Car * first_car_in_road = this->get_current_road_state_pointer()->_cars_in_road[_lanes_id].front();
	return this == first_car_in_road;
}