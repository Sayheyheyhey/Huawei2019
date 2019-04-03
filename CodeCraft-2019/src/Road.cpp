#include "Road.h"
#include <string> //为使用 out<<"road_id ="引入
#include <set>

/*************************************Road_state**********************************************/
//计算Road_state中的的BRC
double Road_state::get_brc_ti() {
	_ti_brc = _road_ptr->_t0_brc * (1 + ALPHA * pow((_Q_brc / _road_ptr->_C_brc), BETA));
	return _ti_brc;
}


Road_state::Road_state(Road * road, int direction) {
	//_road_ptr = road; //注意：这里使用this指针传入的地址，与实际存储的地址不符，但不知道为什么！！！！

	_direction = direction;

	//初始化_car_in_road为 车道数*道路长度 大小的二维vector，其中元素均设置为nullptr
	deque<Car*> tmp;
	_cars_in_road = vector<deque<Car*>>(road->_num_of_lanes);
}
//获得当前道路第一优先级的车辆
Car* Road_state::get_first_waiting_car() {
	vector<Car*> first_in_lane(this->get_num_of_lanes());
	//将每个车道第一辆状态为WAIT的车压入Vector（跳过END）
	for (auto &lane : _cars_in_road) {
		for (auto &car : lane) {
			if (car->_state == END) continue;
			else if (car->_state == WAIT_CAR) break;
			else if (car->_state == WAIT) {
				first_in_lane[car->_lanes_id] = car;
				break;
			}
			else {
				cout << "get_first_waiting_car() Error : Wrong State" << endl;
				return nullptr;
			}
		}
	}
	Car* first_waiting_car = nullptr;
	//选取Vector中优先级最高的车返回
	for (auto &car : first_in_lane) {
		if (car == nullptr) {
			continue;
		}
		else if (first_waiting_car == nullptr) {
			first_waiting_car = car;
		}
		else if (car->_position < first_waiting_car->_position) {
			first_waiting_car = car;
		}
		else {
			continue;
		}
	}
	return first_waiting_car;
	//要考虑第一辆车为END，但第二辆车为WAIT的情况
	////将各车道第一辆车压入vector，如果车道为空则跳过	
	//vector<Car*>  tmp;
	//for (int i = 0; i < this->get_num_of_lanes(); i++) {
	//	if (_cars_in_road[i].empty()) continue;
	//	else if ((_cars_in_road[i].front())->_state == WAIT)
	//		tmp.push_back(_cars_in_road[i].front());
	//}
	////遍历tmp，获取第一辆WAIT的车
	//Car * first_waiting_car = nullptr;
	//if (tmp.empty()) return nullptr;
	//else {
	//	for (auto car : tmp) {
	//		if (first_waiting_car == nullptr) first_waiting_car = car;
	//		else if (first_waiting_car->_position > car->_position)
	//			first_waiting_car = car;
	//	}
	//	return first_waiting_car;
	//}
	//cout << "Someting wrong in get_first_waiting_car()" << endl;
	//return nullptr;
}

//返回值：返回插入位置位置的前一个车的状态，如果没有车，返回0；
//如果没有可以插入的位置，x和y的值都被置为-1
//注意，从优先级高的到优先级低的，如果有一个车道末尾车辆状态为WAIT，则返回的状态为WAIT
int Road_state::get_position_to_insert(int &lanes, int &position) {
	int i = -1;	//记录车道号
	for (auto &one_lane : _cars_in_road) {
		i++;
		if (one_lane.empty()) {
			lanes = i;
			position = 0;
			return READY;
		}
		else if ((one_lane.back()->_position) == this->get_road_length() - 1) {
			if (one_lane.back()->_lanes_id != i) cout << " get_porition_to_insert Error： car's lanes_id not match true lanes_id" << endl;
			//如果有一个车道末尾车辆状态为WAIT，则返回的状态为STOP
			if (one_lane.back()->_state == WAIT || one_lane.back()->_state == WAIT_CAR)
				return STOP;
		}
		else if ((one_lane.back()->_position) < this->get_road_length() - 1) {
			lanes = (one_lane.back())->_lanes_id;

			if (lanes != i)  cout << " get_porition_to_insert Error： car's lanes_id not match true lanes_id" << endl;

			position = (one_lane.back())->_position + 1;
			if (one_lane.back()->_lanes_id != i) cout << " get_porition_to_insert Error： car's lanes_id not match true lanes_id" << endl;
			return (one_lane.back())->_state;
		}
		else
			cout << "get_position_to_insert Note: lane_id-" << one_lane.back()->_lanes_id << "   in Road   " << one_lane.back()->_road_id << "is blocked now" << endl;
	}
	lanes = -1;
	position = -1;
	//如果没有位置可以放车
	return END;
}
//由于Road和Road_state相互包含，所以不能写成内联函数
int Road_state::get_num_of_lanes() { return _road_ptr->_num_of_lanes; }	//获得道路中的车道数
int Road_state::get_road_length() { return _road_ptr->_road_length; }	//获得道路长度
int Road_state::get_max_v_of_road() { return _road_ptr->_max_v; }		//获得道路中最大速度
int Road_state::get_road_id() { return _road_ptr->_road_id; }			//获得道路id
/*************************************Road**********************************************/
Road::Road() {
	//初始化道路状态
	_forward = Road_state(this, FORWARD);
	//双行道则初始化_backward, 否则对_backward进行默认初始化
	if (_is_2_way)
		_backward = Road_state(this, BACK);
	else
		_backward = Road_state();
}

Road::Road(int road_id, int road_length, int max_v, int num_of_lanes,
	int start_node_id, int end_node_id, int is_2_way) {

	_road_id = road_id;
	_road_length = road_length;
	_max_v = max_v;
	_num_of_lanes = num_of_lanes;
	_start_node_id = start_node_id;
	_end_node_id = end_node_id;
	_is_2_way = is_2_way;

	_t0_brc = double(road_length) / double(max_v);
	_C_brc = double(num_of_lanes * _road_length);

	//初始化道路状态
	_forward = Road_state(this, FORWARD);
	//双行道则初始化_backward, 否则对_backward进行默认初始化
	if (is_2_way)
		_backward = Road_state(this, BACK);
	else
		_backward = Road_state();

}

ostream & operator << (ostream &out, Road &obj) {
	out << "road_id=" << obj._road_id << "; ";
	out << "road_length=" << obj._road_length << "; ";
	out << "max_v=" << obj._max_v << "; ";
	out << "num_of_lanes=" << obj._num_of_lanes << "; ";
	out << "start_node_id=" << obj._start_node_id << "; ";
	out << "end_node_id=" << obj._end_node_id << "; ";
	out << "is_2_way=" << obj._is_2_way << "; ";
	return out;
}

int Road::update_adjacent_node_ptr(map<int, Cross>& cross_all) {
	_start_node_ptr = &(cross_all.at(_start_node_id));
	_end_node_ptr = &(cross_all.at(_end_node_id));
	if (_start_node_ptr == nullptr || _end_node_ptr == nullptr) {
		cout << "update_adjacent_node_ptr : Failed to get node pointer" << endl;
	}
	return 0;
}
/*************************************Cross**********************************************/
Cross::Cross(int node_id, int road_id_up, int road_id_right, int road_id_down, int road_id_left) {
	_node_id = node_id;

	_road_id_adjacent[0] = road_id_up;
	_road_id_adjacent[1] = road_id_right;
	_road_id_adjacent[2] = road_id_down;
	_road_id_adjacent[3] = road_id_left;
}
int Cross::get_next_cross_and_road_state(int road_id_cur, map<int, Road>& All_Roads, int &node_id_next, Cross* &node_ptr_next, Road_state* &road_state_out, Road_state* &road_state_in) {
	node_id_next = -1;
	node_ptr_next = nullptr;
	road_state_out = nullptr;
	road_state_in = nullptr;
	//读入的无道路状态为-1，初始化的无道路状态为-1
	if (road_id_cur != 0 && road_id_cur != -1) {
		auto iter = All_Roads.find(road_id_cur);
		if (iter != All_Roads.end()) {
			auto road_ptr = &(iter->second);
			//如果是双向车道
			if (road_ptr->_is_2_way == 1) {
				if (_node_id == road_ptr->_start_node_id) {
					node_id_next = road_ptr->_end_node_id;
					node_ptr_next = road_ptr->_end_node_ptr;
					road_state_out = &(road_ptr->_forward);
					road_state_in = &(road_ptr->_backward);
					if (node_ptr_next == nullptr)
						cout << "get_next_cross_from_Road Error: roads_all should be initialized before cross_all" << endl;
				}
				else if (_node_id == road_ptr->_end_node_id) {
					node_id_next = road_ptr->_start_node_id;
					node_ptr_next = road_ptr->_start_node_ptr;
					road_state_out = &(road_ptr->_backward);
					road_state_in = &(road_ptr->_forward);
					if (node_ptr_next == nullptr)
						cout << "get_next_cross_from_Road Error: roads_all should be initialized before cross_all" << endl;
				}
				else {
					cout << "get_next_cross_from_Road Error: Impatible id between road and cross" << endl;
				}
			}
			else {//处理单向车道
				if (_node_id == road_ptr->_start_node_id) {
					node_id_next = road_ptr->_end_node_id;
					node_ptr_next = road_ptr->_end_node_ptr;
					road_state_out = &(road_ptr->_forward);
					road_state_in = nullptr;
					if (node_ptr_next == nullptr)
						cout << "get_next_cross_from_Road Error: roads_all should be initialized before cross_all" << endl;
				}
				else if (_node_id == road_ptr->_end_node_id) {
					node_id_next = -1;
					node_ptr_next = nullptr;
					road_state_out = nullptr;
					road_state_in = &(road_ptr->_forward);
				}
				else {
					cout << "get_next_cross_from_Road Error: Impatible id between road and cross" << endl;
				}
			}
		}
		else
		{
			cout << "Can not find the road in roads_all" << endl;
			return -1;
		}
	}
	else {
		//如果id为-1，不作操作，返回默认值
		return 0;
	}
	return 0;
}
void Cross::update_4_neighbor(map<int, Road>& All_Roads) {
	int node_id_next = -1;
	Cross *node_ptr_next = nullptr;
	Road_state *road_state_out = nullptr;
	Road_state *road_state_in = nullptr;

	int result;
	for (int i = 0; i < 4; i++) {
		result = get_next_cross_and_road_state(_road_id_adjacent[i], All_Roads, node_id_next, node_ptr_next, road_state_out, road_state_in);
		_node_id_adjacent[i] = node_id_next;
		_node_ptr_adjacent[i] = node_ptr_next;
		_road_state_ptr_adjacent_out[i] = road_state_out;
		_road_state_ptr_adjacent_in[i] = road_state_in;
	}
}

ostream & operator << (ostream &out, Cross &obj) {
	out << "Cross's Data:{ ";
	out << "node_id=" << obj._node_id << "; ";
	out << "road_id_up=" << obj._road_id_adjacent[0] << "; ";
	out << "road_id_right=" << obj._road_id_adjacent[1] << "; ";
	out << "road_id_down=" << obj._road_id_adjacent[2] << "; ";
	out << "road_id_left=" << obj._road_id_adjacent[3] << "; ";
	out << "}";

	out << "Linking Cross:{ ";

	out << "node_id_up=" << obj._node_id_adjacent[0] << "; ";
	out << "node_id_right=" << obj._node_id_adjacent[1] << "; ";
	out << "node_id_down=" << obj._node_id_adjacent[2] << "; ";
	out << "node_id_left=" << obj._node_id_adjacent[3] << "; ";
	out << "}";

	return out;
}