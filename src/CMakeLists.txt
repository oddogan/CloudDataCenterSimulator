cmake_minimum_required(VERSION 3.5)

set(ALL_FILES 
  main.cpp
  CloudDataCenter/CloudDataCenter.cpp
  EventDispatcher/EventDispatcher.cpp
  GlobalTimer/GlobalTimer.cpp
  Machines/PhysicalMachine/PhysicalMachine.cpp
  RequestManager/RequestManager.cpp
  StatisticsManager/StatisticsManager.cpp)

add_executable(CDCSimulator ${ALL_FILES})