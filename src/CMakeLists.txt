cmake_minimum_required(VERSION 3.13)

set(ALL_FILES
  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
  ${CMAKE_CURRENT_LIST_DIR}/CloudDataCenter/CloudDataCenter.cpp
  ${CMAKE_CURRENT_LIST_DIR}/EventDispatcher/EventDispatcher.cpp
  ${CMAKE_CURRENT_LIST_DIR}/GlobalTimer/GlobalTimer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Machines/PhysicalMachine/PhysicalMachine.cpp
  ${CMAKE_CURRENT_LIST_DIR}/RequestManager/RequestManager.cpp
  ${CMAKE_CURRENT_LIST_DIR}/StatisticsManager/StatisticsManager.cpp
  ${CMAKE_CURRENT_LIST_DIR}/AllocationStrategies/BasicAllocationStrategy/BasicAllocationStrategy.cpp
  ${CMAKE_CURRENT_LIST_DIR}/AllocationStrategies/OpenStackWithoutMigration/OpenStackWithoutMigration.cpp
  ${CMAKE_CURRENT_LIST_DIR}/AllocationStrategies/ILPStrategy/ILPStrategy.cpp
  ${CMAKE_CURRENT_LIST_DIR}/MachineRoom/MachineRoom.cpp)

target_sources(CDCSimulator PRIVATE ${ALL_FILES})
