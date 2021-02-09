cmake_minimum_required(VERSION 3.0.0)
project(CoordinatorServer VERSION 0.1.0)

include(CTest)
enable_testing()

aux_source_directory(Sources SRC)
aux_source_directory(/usr/include/postgresql/ PSQL)
include_directories(Sources)


find_package(PostgreSQL REQUIRED)
#find_package(PostgreSQL BOOST)
set(PostgreSQL_ADDITIONAL_VERSIONS "11")

add_executable(CoordinatorServer main.cpp ${SRC} ${PostgreSQL_INCLUDE_DIRS})
target_include_directories(CoordinatorServer PRIVATE ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(CoordinatorServer -lpthread ${PostgreSQL_LIBRARIES} -lpqxx -lpq -lboost_system)


add_executable(TestingClient TestClient/main.cpp TestClient/Client.h TestClient/Stream.cpp)
target_link_libraries(TestingClient -lpthread -std=c++1z -lboost_system)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
