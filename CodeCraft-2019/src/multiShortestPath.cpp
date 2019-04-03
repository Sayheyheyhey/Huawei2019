#include"multiShortestPath.h"
#include "Road.h"
vector<vector<int>> multiShortestPath::display_all()
{
	dfs(destination);
	/*
	for (auto &path_tmp1 : path_all)
	{
		for (auto &path_tmp2 : path_tmp1)
			cout << path_tmp2 << " ";
		cout << endl;
	}
	*/
	return path_all;
}

void multiShortestPath::dfs(int nodeKey)
{
	if (nodeKey == source)
	{
		path_all.push_back(path);
		return;
	}
	for (auto &it : relaxMap[nodeKey]->prev)
	{
		path.push_back(it);
		dfs(it);
		path.pop_back();
	}
}

void multiShortestPath::makeMap(map<int, Road> & road)
{
	//	int NodeNum = road.size();
	//	int edgeNum = road.size();
	int tmp_src;
	int tmp_des;
	int tmp_cost;

	for (auto & it : road)
	{
		tmp_src = it.second._start_node_id;		//出发节点 : 目的节点 : 距离
		tmp_des = it.second._end_node_id;
		tmp_cost = it.second._road_length;
		if (it.second._is_2_way == 1)	//如果是双向车道
		{
			if (adjMap.find(tmp_src) != adjMap.end())
			{
				adjMap[tmp_src].push_back(pair<int, int>(tmp_des, tmp_cost));
			}
			else
			{
				adjMap[tmp_src] = vector<pair<int, int>>{ pair<int, int>(tmp_des, tmp_cost) };
			}
			if (adjMap.find(tmp_des) != adjMap.end())
			{
				adjMap[tmp_des].push_back(pair<int, int>(tmp_src, tmp_cost));
			}
			else
			{
				adjMap[tmp_des] = vector<pair<int, int>>{ pair<int, int>(tmp_src, tmp_cost) };
			}
		}
		else							//如果是单向车道
		{
			if (adjMap.find(tmp_src) != adjMap.end())
			{
				adjMap[tmp_src].push_back(pair<int, int>(tmp_des, tmp_cost));
			}
			else
			{
				adjMap[tmp_src] = vector<pair<int, int>>{ pair<int, int>(tmp_des, tmp_cost) };
			}
		}
		//初始化relaxMap
		if (relaxMap.find(tmp_src) == relaxMap.end())
		{
			relaxMap[tmp_src] = new Node(tmp_src);
		}
		if (relaxMap.find(tmp_des) == relaxMap.end())
		{
			relaxMap[tmp_des] = new Node(tmp_des);
		}
	}

}

void multiShortestPath::dijkstra(int src, int des)
{
	source = src;
	destination = des;
	path.push_back(des);	//目的节点 压入路径存储数组

	vector<Node*> que;
	relaxMap[src]->costTillNow = 0;

	for (auto &it : relaxMap)
	{
		que.push_back(it.second);
	}

	make_heap(que.begin(), que.end(), cmp());//建堆(最大堆)

											 //dijkstra核心算法
											 //bool breakFlag = false;
	while (!que.empty()) {// ||!breakFlag
		map<int, Node*> S;//已经扫描过的节点集合

		pop_heap(que.begin(), que.end(), cmp());//与que.pop_back()搭配使用，先将最小元素放到堆最后面，再用que.pop_back()让其出堆
		S.insert(pair<int, Node*>(que.back()->key, que.back()));//出堆节点进入集合S,便于以后检索
		que.pop_back();//最小元素出队


					   //遍历出队节点的邻接节点，更新节点costTillNow
		for (auto &it : S) {//这层循环遍历出队节点
			for (auto &iter : adjMap[it.second->key])//这层循环遍历出堆节点的邻接节点
			{
				//两个if完成松弛操作
				if ((it.second->costTillNow + iter.second) < relaxMap[iter.first]->costTillNow)
				{
					relaxMap[iter.first]->costTillNow = it.second->costTillNow + iter.second;//松弛节点,下一个节点的costTillNow = 当前节点的costTillNow + 二者之间边的cost
					relaxMap[iter.first]->prev.clear();
					relaxMap[iter.first]->prev.push_back(it.first);//指向前节点,记录路径，这里是求多条路径的关键点，用vector来存储多个前向节点
				}
				else if ((it.second->costTillNow + iter.second) == relaxMap[iter.first]->costTillNow)
				{
					relaxMap[iter.first]->prev.push_back(it.first);
				}
			}
		}
		make_heap(que.begin(), que.end(), cmp());//松弛之后重置堆
	}//end of while
}

multiShortestPath::~multiShortestPath()
{
	for (auto &it : relaxMap)
	{
		delete it.second;
	}
}
