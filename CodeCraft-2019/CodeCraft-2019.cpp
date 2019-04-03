#include "Car.h"
#include "Road.h"
#include "Magic_Box.h"
#include <iostream>
#include "multiShortestPath.h"
#include <string>
int main()	//int argc, char *argv[]
{
	std::cout << "Begin" << std::endl;
	/*
	if (argc < 5) {
		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
		exit(1);
	}

	std::string carPath(argv[1]);
	std::string roadPath(argv[2]);
	std::string crossPath(argv[3]);
	std::string answerPath(argv[4]);

	std::cout << "carPath is " << carPath << std::endl;
	std::cout << "roadPath is " << roadPath << std::endl;
	std::cout << "crossPath is " << crossPath << std::endl;
	std::cout << "answerPath is " << answerPath << std::endl;
	*/
	// TODO:read input filebuf
	Magic_Box Box;
	Box.Read_from_file("1-map-training-1\\Road.txt", "1-map-training-1\\Car.txt", "1-map-training-1\\Cross.txt");//��ȡ�ļ�����ʼ��
	Box.update_routine_stored_in_cross();//Ѱ����·����ÿһ·�ڵ���������·�ڵ�����·�������Ե�ͼ��100��·������£�����·����������Ϊ1�����Ϊ3	
	// TODO:process
	while (Box._num_of_cars_not_arrived != 0) {//����
		Box.RUN();
		//show_car(Box._cars_all);
	//	cout << "Time is:" << Box._time << endl;
		++Box._time;
	}
	// TODO:write output file
	Box.output_answer("answer.txt");
	
//	cout << Box.weight_of_graph << endl;
	getchar();
	return 0;
}

