cmake_minimum_required(VERSION 3.13)
project(unittest C CXX)
find_package(GTest)

include_directories(
    ../kernel
)

add_executable(pixel_writer pixel_writer_test.cpp)
# add_executable(net_device_register net_device_register_test.cpp)
# add_executable(intr_request_irq intr_request_irq_test.cpp)

target_link_libraries(pixel_writer ${GTEST_BOTH_LIBRARIES} pthread kernel)
target_include_directories(pixel_writer PUBLIC ${GTEST_INCLUDE_DIRS})
# foreach(target net_run_test net_device_register intr_request_irq)
#     target_link_libraries(${target} ${GTEST_BOTH_LIBRARIES} pthread source)
#     target_include_directories(${target} PUBLIC ${GTEST_INCLUDE_DIRS})
# endforeach()
