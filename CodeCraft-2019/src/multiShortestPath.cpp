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
		tmp_src = it.second._start_node_id;		//�����ڵ� : Ŀ�Ľڵ� : ����
		tmp_des = it.second._end_node_id;
		tmp_cost = it.second._road_length;
		if (it.second._is_2_way == 1)	//�����˫�򳵵�
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
		else							//����ǵ��򳵵�
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
		//��ʼ��relaxMap
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
	path.push_back(des);	//Ŀ�Ľڵ� ѹ��·���洢����

	vector<Node*> que;
	relaxMap[src]->costTillNow = 0;

	for (auto &it : relaxMap)
	{
		que.push_back(it.second);
	}

	make_heap(que.begin(), que.end(), cmp());//����(����)

											 //dijkstra�����㷨
											 //bool breakFlag = false;
	while (!que.empty()) {// ||!breakFlag
		map<int, Node*> S;//�Ѿ�ɨ����Ľڵ㼯��

		pop_heap(que.begin(), que.end(), cmp());//��que.pop_back()����ʹ�ã��Ƚ���СԪ�طŵ�������棬����que.pop_back()�������
		S.insert(pair<int, Node*>(que.back()->key, que.back()));//���ѽڵ���뼯��S,�����Ժ����
		que.pop_back();//��СԪ�س���


					   //�������ӽڵ���ڽӽڵ㣬���½ڵ�costTillNow
		for (auto &it : S) {//���ѭ���������ӽڵ�
			for (auto &iter : adjMap[it.second->key])//���ѭ���������ѽڵ���ڽӽڵ�
			{
				//����if����ɳڲ���
				if ((it.second->costTillNow + iter.second) < relaxMap[iter.first]->costTillNow)
				{
					relaxMap[iter.first]->costTillNow = it.second->costTillNow + iter.second;//�ɳڽڵ�,��һ���ڵ��costTillNow = ��ǰ�ڵ��costTillNow + ����֮��ߵ�cost
					relaxMap[iter.first]->prev.clear();
					relaxMap[iter.first]->prev.push_back(it.first);//ָ��ǰ�ڵ�,��¼·���������������·���Ĺؼ��㣬��vector���洢���ǰ��ڵ�
				}
				else if ((it.second->costTillNow + iter.second) == relaxMap[iter.first]->costTillNow)
				{
					relaxMap[iter.first]->prev.push_back(it.first);
				}
			}
		}
		make_heap(que.begin(), que.end(), cmp());//�ɳ�֮�����ö�
	}//end of while
}

multiShortestPath::~multiShortestPath()
{
	for (auto &it : relaxMap)
	{
		delete it.second;
	}
}
