cmake_minimum_required(VERSION 3.0.0)
project(CoordinatorServer VERSION 0.1.0)

include(CTest)
enable_testing()

aux_source_directory(Sources SRC)
include_directories(Sources)

add_executable(CoordinatorServer main.cpp ${SRC})
target_link_libraries(CoordinatorServer -lpthread)

add_executable(TestingClient TestClient/main.cpp ${SRC})
target_link_libraries(TestingClient -lpthread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
