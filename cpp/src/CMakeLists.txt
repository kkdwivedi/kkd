aux_source_directory(. KKD_CPP_SRCS)

if (NOT KKD_CPP_OPT_ASYNC)
	list(REMOVE_ITEM KKD_CPP_SRCS )
endif (NOT KKD_CPP_OPT_ASYNC)

add_library(kkd-cpp ${KKD_CPP_SRCS})
target_link_libraries(kkd-cpp )
set_target_properties(kkd-cpp PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(kkd-cpp PRIVATE ../include)
target_include_directories(kkd-cpp PRIVATE .)
