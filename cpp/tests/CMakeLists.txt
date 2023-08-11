enable_testing()

aux_source_directory(. KKD_CPP_TEST_SRCS)

if (NOT KKD_CPP_OPT_ASYNC)
	list(REMOVE_ITEM KKD_CPP_TEST_SRCS ./TestTask.cpp)
endif (NOT KKD_CPP_OPT_ASYNC)

find_package(GTest REQUIRED)

add_executable(kkd-cpp-test ${KKD_CPP_TEST_SRCS})

target_link_libraries(kkd-cpp-test kkd-cpp gtest)
add_test(kkd-cpp-test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kkd-cpp-test)

target_include_directories(kkd-cpp-test PRIVATE ../include)
target_include_directories(kkd-cpp-test PRIVATE ../src)
