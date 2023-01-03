#ifdef __APPLE__
/* Defined before OpenGL and GLUT includes to avoid deprecation messages */
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
// #else
//  #include <GL/gl.h>
// #include <GL/glut.h>
#endif

#include <vector>
#include <string>
#include "../../glm/glm/glm.hpp"
#include "../../glm/glm/gtc/matrix_transform.hpp"
#include "../../glm/glm/gtc/type_ptr.hpp"
#include "../../glm/glm/gtx/string_cast.hpp"
#include <Base/logs.h>
#include <Base/shader_utils.h>
#include <Base/maths_utils.h>
#include <iostream>
#include <fstream>

// GLuint CreateSSBO(std::vector<glm::vec4> &varray)
// {
//     GLuint ssbo;
//     glGenBuffers(1, &ssbo);
//     glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo );
//     glBufferData(GL_SHADER_STORAGE_BUFFER, varray.size()*sizeof(*varray.data()), varray.data(), GL_STATIC_DRAW); 
//     return ssbo;
// }

const size_t WIDTH = 1080;
const size_t HEIGHT = 1920;
const char *WINDOW_NAME = "OpenGL";
auto shader_utils = ShaderUtils::Program{};

/*
 * Callback to handle the "close window" event, once the user pressed the Escape key.
 */
static void quitCallback(GLFWwindow *window, int key, int scancode, int action, int _mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

/**
 * @brief Load the shaders, in order to display the result
 *
 * @param erase_if_program_registered Allow to erase the shader if it exists
 * @return true The shader has been successfully registered
 * @return false The shader has not been registered, due to an error
 */
const bool loadShaderProgram(const bool erase_if_program_registered);

/*
 * Callback to handle the "reload" event, once the user pressed the 'r' key.
 */
static void reloadShaders(GLFWwindow *window, int key, int scancode, int action, int _mods)
{
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        debug("reloading...");
        loadShaderProgram(true);
    }
}

/*
 * Initializes the window and viewport via GLFW.
 * The viewport takes the all window.
 * If an error happens, the function returns `NULL` but **does not** free / terminate the GLFW library.
 * Then, do not forget to call `glfwTerminate` if this function returns `NULL`.
 */
GLFWwindow *initializeWindow()
{
    // Minimum target is OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(HEIGHT, WIDTH, WINDOW_NAME, NULL, NULL);
    if (!window)
    {
        error("window creation failed");
        return NULL;
    }
    // Close the window as soon as the Escape key has been pressed
    glfwSetKeyCallback(window, quitCallback);
    // Easy reload
    glfwSetKeyCallback(window, reloadShaders);
    // Makes the window context current
    glfwMakeContextCurrent(window);
    // Enable the viewport
    glViewport(0, 0, HEIGHT, WIDTH);

    return window;
}

/**
 * @brief Returns the all file, as a string, which the file path has been passed
 * as parameter
 *
 * @param path The path of the file
 * @return The content of the file, as a string (read all file)
 */
inline auto readFile(const std::string_view path) -> const std::string
{
    // Avoid dynamic allocation: read the 4096 first bytes
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(path.data());
    stream.exceptions(std::ios_base::badbit);

    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(&buf[0], read_size))
    {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

const bool loadShaderProgram(const bool erase_if_program_registered = true)
{
    const std::string basicVertexShaderSource = readFile("/Users/parksejin/Documents/opengl-explorer/uniformblock/shaders/vertex_shader.glsl");
    const std::string basicFragmentShaderSource = readFile("/Users/parksejin/Documents/opengl-explorer/uniformblock/shaders/fragment_shader.glsl");

    if (!shader_utils.registerShader(ShaderUtils::Type::VERTEX_SHADER_TYPE, basicVertexShaderSource.c_str()))
    {
        error("failed to register the vertex shader...");
        return false;
    }

    if (!shader_utils.registerShader(ShaderUtils::Type::FRAGMENT_SHADER_TYPE, basicFragmentShaderSource.c_str()))
    {
        error("failed to register the fragment shader...");
        return false;
    }

    if (!shader_utils.registerProgram(erase_if_program_registered))
    {
        error("failed to register the program...");
        return false;
    }
    return true;
}

int main(void)
{
    // Initialize the lib
    if (!glfwInit())
    {
        error("could not start GLFW3");
        return -1;
    }

    GLFWwindow *window = initializeWindow();
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Note: Once you have a current OpenGL context, you can use OpenGL normally
    // get version info
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    info("Renderer: " << renderer);
    info("OpenGL version supported: " << version);

    if (!loadShaderProgram(false))
    {
        error("can't load the shaders to initiate the program");
        glfwTerminate();
        return -1;
    }
    /* END OF SHADER PART */

    GLuint program = shader_utils.getProgram().value();

    GLint  loc_mvp  = glGetUniformLocation(program, "u_mvp");
    GLint  loc_res  = glGetUniformLocation(program, "u_resolution");
    GLint  loc_thi  = glGetUniformLocation(program, "u_thickness");

    glUseProgram(program);

    glUniform1f(loc_thi, 20.0);

    // GLushort pattern = 0x18ff;
    // GLfloat  factor  = 2.0f;

    glm::vec4 p00(-1.0f, -0.5f, 0.0f, 1.0f);
    glm::vec4 p01(-0.5f,  0.5f, 0.0f, 1.0f);
    glm::vec4 p02( 0.0f, -0.5f, 0.0f, 1.0f);
    glm::vec4 p03( 0.5f,  0.5f, 0.0f, 1.0f);
    glm::vec4 p04( 1.0f, -0.5f, 0.0f, 1.0f);
    glm::vec4 p05( 1.5f,  0.5f, 0.0f, 1.0f);
    glm::vec4 p06( 2.0f, -0.5f, 0.0f, 1.0f);
    glm::vec4 p07( 2.5f,  0.5f, 0.0f, 1.0f);

    std::vector<glm::vec4> varray0{p00, p01, p02, p03, p04, p05, p06, p07};

    glm::vec4 p0(-1.0f, -1.0f, 0.0f, 1.0f);
    glm::vec4 p1(1.0f, -1.0f, 0.0f, 1.0f);
    glm::vec4 p2(1.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 p3(-1.0f, 1.0f, 0.0f, 1.0f);
    std::vector<glm::vec4> varray1{ p3, p0, p1, p2, p3, p0, p1 };
    // GLuint ssbo1 = CreateSSBO(varray1);
    // std::cout << "varray1" << std::endl;
    // for (auto &el : varray1) {
    //     std::cout << el[0] << ", " << el[1] << ", " << el[2] << ", " << el[3] << std::endl;
    // }

    std::vector<glm::vec4> varray2;
    for (int u=-8; u <= 368; u += 8)
    {
        double a = u*M_PI/180.0;
        double c = cos(a), s = sin(a);
        varray2.emplace_back(glm::vec4((float)c, (float)s, 0.0f, 1.0f));
        std::cout << "u = " << u << ", a = " << a << " (" << c << ", " << s << ")" << std::endl;
    }

    //std::cout << "varray2" << std::endl;
    //for (auto &el : varray2) {
    //    std::cout << el[0] << ", " << el[1] << ", " << el[2] << ", " << el[3] << std::endl;
    //}

    // GLuint ssbo2 = CreateSSBO(varray2);

    GLuint ubo0 = 0;
    glGenBuffers(1, &ubo0);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo0);
    glBufferData(GL_UNIFORM_BUFFER, varray0.size() * sizeof(*varray0.data()), varray0.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    unsigned int block = glGetUniformBlockIndex(program, "BlockRect");
    GLuint bind0 = 0;
    glUniformBlockBinding(program, block, bind0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo0, 0, sizeof(varray0));

    GLuint ubo1 = 0;
    glGenBuffers(1, &ubo1);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo1);
    glBufferData(GL_UNIFORM_BUFFER, varray1.size() * sizeof(*varray1.data()), varray1.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint bind = 1;
    glUniformBlockBinding(program, block, bind);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, ubo1, 0, sizeof(varray1));

    GLuint ubo2 = 0;
    glGenBuffers(1, &ubo2);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo2);
    glBufferData(GL_UNIFORM_BUFFER, varray2.size() * sizeof(*varray2.data()), varray2.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint bind2 = 2;
    glUniformBlockBinding(program, block, bind2);
    glBindBufferRange(GL_UNIFORM_BUFFER, 2, ubo2, 0, sizeof(varray2));

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glm::mat4(project);
    int vpSize[2]{0, 0};

    while (!glfwWindowShouldClose(window))
    {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        if (w != vpSize[0] ||  h != vpSize[1])
        {
            vpSize[0] = w; vpSize[1] = h;
            glViewport(0, 0, vpSize[0], vpSize[1]);
            float aspect = (float)w/(float)h;
            project = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -10.0f, 10.0f);
            glUniform2f(loc_res, (float)w, (float)h);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        // line1 
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glm::mat4 modelview1( 1.0f );
        modelview1 = glm::translate(modelview1, glm::vec3(-1.0f, 0.6f, 0.0f) );
        modelview1 = glm::scale(modelview1, glm::vec3(0.2f, 0.2f, 1.0f) );
        glm::mat4 mvp1 = project * modelview1;

        glBindBuffer(GL_UNIFORM_BUFFER, ubo0);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, varray0.size() * sizeof(*varray0.data()), varray0.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glUniformBlockBinding(program, block, bind0);

        glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp1));
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo1);
        GLsizei N1 = (GLsizei)varray0.size()-2;
        glDrawArrays(GL_TRIANGLES, 0, 6*(N1-1));

        // line2
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        modelview1 = glm::mat4( 1.0f );
        modelview1 = glm::translate(modelview1, glm::vec3(-1.0f, -0.6f, 0.0f) );
        modelview1 = glm::scale(modelview1, glm::vec3(0.2f, 0.2f, 1.0f) );
        mvp1 = project * modelview1;

        glBindBuffer(GL_UNIFORM_BUFFER, ubo0);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, varray0.size() * sizeof(*varray0.data()), varray0.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glUniformBlockBinding(program, block, bind0);

        glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp1));
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo1);
        N1 = (GLsizei)varray0.size()-2;
        glDrawArrays(GL_TRIANGLES, 0, 6*(N1-1));

        // rectangle1
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        modelview1 = glm::mat4( 1.0f );
        modelview1 = glm::translate(modelview1, glm::vec3(0.0f, 0.6f, 0.0f) );
        modelview1 = glm::scale(modelview1, glm::vec3(0.3f, 0.3f, 1.0f) );
        mvp1 = project * modelview1;

        glBindBuffer(GL_UNIFORM_BUFFER, ubo1);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, varray1.size() * sizeof(*varray1.data()), varray1.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glUniformBlockBinding(program, block, bind);

        glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp1));
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo1);
        N1 = (GLsizei)varray1.size()-2;
        glDrawArrays(GL_TRIANGLES, 0, 6*(N1-1));

        // rectangle2
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        modelview1 = glm::mat4( 1.0f );
        modelview1 = glm::translate(modelview1, glm::vec3(0.0f, -0.6f, 0.0f) );
        modelview1 = glm::scale(modelview1, glm::vec3(0.3f, 0.3f, 1.0f) );
        mvp1 = project * modelview1;

        glBindBuffer(GL_UNIFORM_BUFFER, ubo1);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, varray1.size() * sizeof(*varray1.data()), varray1.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glUniformBlockBinding(program, block, bind);

        glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp1));
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo1);
        N1 = (GLsizei)varray1.size()-2;
        glDrawArrays(GL_TRIANGLES, 0, 6*(N1-1));

        // circle1
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glm::mat4 modelview2( 1.0f );
        modelview2 = glm::translate(modelview2, glm::vec3(1.0f, 0.6f, 0.0f) );
        modelview2 = glm::scale(modelview2, glm::vec3(0.3f, 0.3f, 1.0f) );
        glm::mat4 mvp2 = project * modelview2;

        glBindBuffer(GL_UNIFORM_BUFFER, ubo2);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, varray2.size() * sizeof(*varray2.data()), varray2.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glUniformBlockBinding(program, block, bind2);
        
        glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp2));
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo2);
        GLsizei N2 = (GLsizei)varray2.size()-2;
        glDrawArrays(GL_TRIANGLES, 0, 6*(N2-1));

        // circle2
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        modelview2 = glm::mat4( 1.0f );
        modelview2 = glm::translate(modelview2, glm::vec3(1.0f, -0.6f, 0.0f) );
        modelview2 = glm::scale(modelview2, glm::vec3(0.3f, 0.3f, 1.0f) );
        mvp2 = project * modelview2;

        glBindBuffer(GL_UNIFORM_BUFFER, ubo2);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, varray2.size() * sizeof(*varray2.data()), varray2.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glUniformBlockBinding(program, block, bind2);
        
        glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, glm::value_ptr(mvp2));
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo2);
        N2 = (GLsizei)varray2.size()-2;
        glDrawArrays(GL_TRIANGLES, 0, 6*(N2-1));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}