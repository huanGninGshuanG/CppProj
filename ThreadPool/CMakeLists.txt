file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.cpp include/*.h)

message("sources is ${sources}")
add_library(tp SHARED ${sources})
target_include_directories(tp PUBLIC include)