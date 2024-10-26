cmake_minimum_required(VERSION 3.13)

target_include_directories(CDCSimulator PRIVATE ${CMAKE_CURRENT_LIST_DIR}/cplex)
target_link_libraries(CDCSimulator ${CMAKE_CURRENT_LIST_DIR}/cplex/libcplex.a)
target_link_libraries(CDCSimulator ${CMAKE_CURRENT_LIST_DIR}/cplex/libconcert.a)
target_link_libraries(CDCSimulator ${CMAKE_CURRENT_LIST_DIR}/cplex/libilocplex.a)