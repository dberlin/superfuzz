add_library(support STATIC option.cc type.cc)
add_executable(superfuzz superfuzz.cc)
target_link_libraries(superfuzz support)
