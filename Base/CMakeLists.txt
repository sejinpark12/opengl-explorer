add_library(Base
    include/Base/maths_utils.h
    include/Base/shader_utils.h
    include/Base/logs.h
    src/maths_utils.cpp
    src/shader_utils.cpp)

target_include_directories(Base
    PUBLIC include)

target_compile_features(Base
    PUBLIC cxx_std_17)

target_compile_definitions(Base
    PUBLIC HOME_PATH="${CMAKE_HOME_DIRECTORY}")

find_package(glfw3 3.4 REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(Base 
    PUBLIC ${OPENGL_INCLUDE_DIR})

if (APPLE)
    target_link_libraries(Base "-framework Cocoa")
    target_link_libraries(Base "-framework OpenGL")
    target_link_libraries(Base "-framework IOKit")
endif (APPLE)
target_link_libraries(Base glfw ${OPENGL_gl_LIBRARY})