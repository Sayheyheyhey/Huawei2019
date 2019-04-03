#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <queue>
#include <algorithm>
#include "Road.h"
#include <limits.h>
using namespace std;

class Node {//用于relaxMap中
public:
	int key;//存路口 id
	int costTillNow = INT_MAX;//到该节点为止的cost，松弛的时候会用到
	vector<int> prev;//前向节点。因为是多条最短路径，所以前向节点可以有多个

	Node(int k) : key(k) {};
};

//dijkstra算法中的优先队列的比较函数，因为queue中存储的是Node指针的形式，
//所以这个比较函数不能写在Node类里
struct cmp {
	bool operator()(const Node * a, const Node * b) {
		return a->costTillNow > b->costTillNow;
	}
};

class multiShortestPath
{
public:
	map<int, vector<pair<int, int>> > adjMap;//邻接表,pair<des,cost>,邻接表主要任务是记录边，便于以后查找某条边的权值,分别是: 出发节点 : 目的节点 : 距离
	map<int, Node*> relaxMap;//dijkstra算法进行中需要维护的所有节点，用来进行松弛操作。我把节点和边分开了，Node中不会有诸如next这样的代表边的成员变量，所有的边都单独存储在adjMap中了，存的是：节点id : 对应此id节点地址
	int source;//记录出发点的值，供display使用
	int destination;//记录目的地的值，供display使用
	vector<int> path;//记录路径，供display使用
	vector<vector<int>> path_all;//记录所有路径
	void dfs(int nodeKey);//用dfs遍历输出多条最短路径
public:
	//	void display();
	void makeMap(map<int, Road> & road);//建立邻接表和relaxMap
	void dijkstra(int src, int des);
	vector<vector<int>> display_all();
	~multiShortestPath();//释放relaxMap占用的内存
};
