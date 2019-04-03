# Huawei2019
华为2019判题器

1.很可惜没能进入复赛；
2.判题器与网站效果基本相同；
3.主要的流程存储于Magic_Box类中；
4.修改运行逻辑可以参照get_next_road_id函数和Box::RUN()；
5.修改车辆出发逻辑可以参照drive_car_in_garage函数；
6.车辆行驶过的路径存储在Car::Routine
7.车辆预先规划号的路径存储在Car::_route_pre_planned;
8.修改车辆预先规划的路径参考multiShortestPath.h和multiShortestPath.cpp,这两个文件根据迪杰特斯拉算法生成初始路径；
9.个人时间问题，没有写出重新选路的模块。
