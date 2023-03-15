#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vmath/vmath.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ctime>
#include <vector>
#include <saveBMP/saveBMP.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

float particles[2000][3];
// settings
const unsigned int SCR_WIDTH = 424;
const unsigned int SCR_HEIGHT = 240;

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "out vec3 after;\n"
                                 "uniform mat4 model;\n"
                                 "uniform mat4 view;\n"
                                 "uniform mat4 projection;\n"
                                 "uniform mat4 MVP;\n"
                                 "void main()\n"
                                 "{\n"
                                 "after = aPos;\n"
                                 "gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
                                   "in vec3 after;\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(after.x, after.y, after.z, 1.0f);\n"
                                   "}\n\0";


int N = 2000;
float rtri = 0;

int main() {

    srand((unsigned int) time(0));    //random seeds
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < 3; j++) {
            particles[i][j] = (rand() % 800 / 800.0 - 0.5) * 2;
        }
    }


    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);


    const GLchar *feedbackVaryings[] = {"after"};
    glTransformFeedbackVaryings(shaderProgram, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);


    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            -1.0f, -1.0f, 0.0f, // left
            1.0f, -1.0f, 0.0f, // right
            0.0f, 1.0f, 0.0f  // top
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    //---
    GLuint tbo;
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Perform feedback transform
    // glEnable(GL_RASTERIZER_DISCARD);

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);

    glBeginTransformFeedback(GL_TRIANGLES

    );

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------

    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw our first triangle
    glUseProgram(shaderProgram);
    glBindVertexArray(
            VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

    for (int i = 0; i < 10; ++i) {
        rtri++;
        //--
        // create transformations
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                      100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // pass transformation matrices to the shader
        int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

        int viewLocation = glGetUniformLocation(shaderProgram, "view");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
        int modelLocation = glGetUniformLocation(shaderProgram, "model");
        glUseProgram(shaderProgram);


        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
        model = glm::rotate(model, float(glfwGetTime()), glm::vec3(0.1, 1.0, 0.1));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
        for (int i = 0; i < N; i++) {

            //平移到第i个粒子位置
            model = glm::translate(model, glm::vec3(particles[i][1]));
            model = glm::rotate(model, float(glfwGetTime()), glm::vec3(0.1, 1.0, 0.1));
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_LINE_LOOP, 0, 3);
        }
    }
    for (int i = 0; i < 10; ++i) {

        //--
        // create transformations
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                      100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // pass transformation matrices to the shader
        int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

        int viewLocation = glGetUniformLocation(shaderProgram, "view");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
        int modelLocation = glGetUniformLocation(shaderProgram, "model");
        glUseProgram(shaderProgram);


        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
        model = glm::rotate(model, rtri * i, glm::vec3(0.1, 1.0, 0.1));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
        for (int i = 0; i < N; i++) {

            //平移到第i个粒子位置
            model = glm::translate(model, glm::vec3(particles[i][2]));
            model = glm::rotate(model, float(glfwGetTime()) * 2000, glm::vec3(0.1, 1.0, 0.1));
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_LINE_LOOP, 0, 3);
        }
    }
    for (int i = 0; i < 10; ++i) {

        //--
        // create transformations
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                      100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // pass transformation matrices to the shader
        int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

        int viewLocation = glGetUniformLocation(shaderProgram, "view");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
        int modelLocation = glGetUniformLocation(shaderProgram, "model");
        glUseProgram(shaderProgram);


        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
        model = glm::rotate(model, rtri * i, glm::vec3(0.1, 1.0, 0.1));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
        for (int i = 0; i < N; i++) {

            //平移到第i个粒子位置
            model = glm::translate(model, glm::vec3(particles[i][3]));

            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_LINE_LOOP, 0, 3);
        }
    }
    for (int i = 0; i < 10; ++i) {

        //--
        // create transformations
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                      100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // pass transformation matrices to the shader
        int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

        int viewLocation = glGetUniformLocation(shaderProgram, "view");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
        int modelLocation = glGetUniformLocation(shaderProgram, "model");
        glUseProgram(shaderProgram);


        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
        model = glm::rotate(model, rtri * (i + 200), glm::vec3(0.1, 1.0, 0.1));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
        for (int i = 0; i < N; i++) {

            //平移到第i个粒子位置
            model = glm::translate(model, glm::vec3(particles[i][0]));
            model = glm::rotate(model, float(glfwGetTime()) * 5000, glm::vec3(0.1, 1.0, 0.1));
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_LINE_LOOP, 0, 3);
        }
    }
    for (int i = 0; i < 10; ++i) {

        //--
        // create transformations
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                      100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // pass transformation matrices to the shader
        int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

        int viewLocation = glGetUniformLocation(shaderProgram, "view");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
        int modelLocation = glGetUniformLocation(shaderProgram, "model");
        glUseProgram(shaderProgram);


        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
        model = glm::rotate(model, rtri * (i + 100), glm::vec3(0.1, 1.0, 0.1));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
        for (int i = 0; i < N; i++) {

            //平移到第i个粒子位置
            model = glm::translate(model, glm::vec3(particles[i][0]));

            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_LINE_LOOP, 0, 3);
        }
    }
    for (int i = 0; i < 10; ++i) {

        //--
        // create transformations
        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                      100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // pass transformation matrices to the shader
        int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

        int viewLocation = glGetUniformLocation(shaderProgram, "view");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
        int modelLocation = glGetUniformLocation(shaderProgram, "model");
        glUseProgram(shaderProgram);


        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
        model = glm::rotate(model, rtri * 120.0f, glm::vec3(0.1, 1.0, 0.1));
        model = glm::rotate(model, 45.0f, glm::vec3(0.1, 1.0, 0.1));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
        for (int i = 0; i < N; i++) {

            //平移到第i个粒子位置
            model = glm::translate(model, glm::vec3(particles[i][0]));

            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_LINE_LOOP, 0, 3);
        }


        // glBindVertexArray(0); // no need to unbind it every time
        //save bmp
        std::vector<GLubyte> bmpData(SCR_WIDTH * SCR_HEIGHT * 4, 0);

        glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_BGRA, GL_UNSIGNED_BYTE, &bmpData[0]);
        saveBmpForRGBA(&bmpData[0], SCR_WIDTH, SCR_HEIGHT, "feed_back");


        glfwSwapBuffers(window);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}