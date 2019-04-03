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

class Node {//����relaxMap��
public:
	int key;//��·�� id
	int costTillNow = INT_MAX;//���ýڵ�Ϊֹ��cost���ɳڵ�ʱ����õ�
	vector<int> prev;//ǰ��ڵ㡣��Ϊ�Ƕ������·��������ǰ��ڵ�����ж��

	Node(int k) : key(k) {};
};

//dijkstra�㷨�е����ȶ��еıȽϺ�������Ϊqueue�д洢����Nodeָ�����ʽ��
//��������ȽϺ�������д��Node����
struct cmp {
	bool operator()(const Node * a, const Node * b) {
		return a->costTillNow > b->costTillNow;
	}
};

class multiShortestPath
{
public:
	map<int, vector<pair<int, int>> > adjMap;//�ڽӱ�,pair<des,cost>,�ڽӱ���Ҫ�����Ǽ�¼�ߣ������Ժ����ĳ���ߵ�Ȩֵ,�ֱ���: �����ڵ� : Ŀ�Ľڵ� : ����
	map<int, Node*> relaxMap;//dijkstra�㷨��������Ҫά�������нڵ㣬���������ɳڲ������Ұѽڵ�ͱ߷ֿ��ˣ�Node�в���������next�����Ĵ���ߵĳ�Ա���������еı߶������洢��adjMap���ˣ�����ǣ��ڵ�id : ��Ӧ��id�ڵ��ַ
	int source;//��¼�������ֵ����displayʹ��
	int destination;//��¼Ŀ�ĵص�ֵ����displayʹ��
	vector<int> path;//��¼·������displayʹ��
	vector<vector<int>> path_all;//��¼����·��
	void dfs(int nodeKey);//��dfs��������������·��
public:
	//	void display();
	void makeMap(map<int, Road> & road);//�����ڽӱ��relaxMap
	void dijkstra(int src, int des);
	vector<vector<int>> display_all();
	~multiShortestPath();//�ͷ�relaxMapռ�õ��ڴ�
};
