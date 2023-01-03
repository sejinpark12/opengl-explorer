#ifndef GL_TEST_UTIL_H
#define GL_TEST_UTIL_H

#define GL_TEST(function) do { \
    function; \
    GLenum error = glGetError(); \
    if (error != GL_NO_ERROR) { \
        throw std::runtime_error("Err to call GL function."); \
    } \
} while(false)

#endif
        // spdlog::error("{} with 0x{:x}.", STRING(function), error); \