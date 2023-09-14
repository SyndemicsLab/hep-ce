cmake_minimum_required(VERSION 3.19)
set (CMAKE_CXX_STANDARD 20)

add_executable(hepce main.cpp)

add_subdirectory(data)
add_subdirectory(event)
add_subdirectory(person)
add_subdirectory(simulation)
add_subdirectory(utils)

# set(MAIN_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
# target_include_directories(src PUBLIC ${MAIN_INCLUDES})

target_link_libraries(hepce
  data
  event
  person
  simulation
  libs
  utils
)
