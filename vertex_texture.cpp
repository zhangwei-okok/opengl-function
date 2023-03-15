#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <iostream>
#include <vector>
#include <array>
#include "vmath/vmath.h"
#include "saveBMP/saveBMP.h"
#include "stdio.h"
#include "shader/GLSLProgram.h"
#include "stb/stb_image.h"

using namespace std;

template<class T>
inline void gl_BufferData(GLenum target, const std::vector<T> &v, GLenum usage) {
    glBufferData(target, v.size() * sizeof(T), &v[0], usage);
}

// settings
const unsigned int SCR_WIDTH = 424;
const unsigned int SCR_HEIGHT = 240;
const int SIZE = 32;

std::vector<GLfloat> points;

unsigned int vao, vbo, textureID;
unsigned int texVBO;
GLSLProgram *shader = nullptr;

GLFWwindow *window;

void init();

void render();

void release();

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "basicVertex", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    init();
    render();
    release();


}

void init() {
    int col = SCR_WIDTH / SIZE + 1;
    int row = SCR_HEIGHT / SIZE + 1;


    for (int i = 0; i < col; i++)
        for (int j = 0; j < row; j++) {
            float top = j * SIZE;
            float bottom = top + SIZE;
            float left = i * SIZE;
            float right = left + SIZE;
            std::array<GLfloat, 9> data;
            data[0] = left;
            data[1] = bottom;
            data[2] = 0.0;

            data[3] = right;
            data[4] = bottom;
            data[5] = 0.0;

            data[6] = 0.5 * (left + right);
            data[7] = top;
            data[8] = 0.0;

            for (int i = 0; i < 9; i++)
                points.push_back(data[i]);

            for (int k = 0; k < 3; ++k) {
                // std::cout << data[k * 3 + 0] << " " << data[k * 3 + 1] << " " << data[k * 3 + 2] << "\n";

            }

        }


    float texVertx[8] = {
            0, 0,
            0, 1,
            1, 1,
            1, 1
    };

    std::vector<GLfloat> texList;

    for (int i = 0; i < points.size() / 3; i++) {
        int pos = i % 3;
        texList.push_back(texVertx[pos * 2]);
        texList.push_back(texVertx[pos * 2 + 1]);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &texVBO);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), &points[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, texVBO);
    glBufferData(GL_ARRAY_BUFFER, texList.size() * sizeof(float), texList.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(1);

    shader = new GLSLProgram;
    if (!shader->build("../shader/texture.vert", "../shader/texture.frag")) {
        cout << "failed to load shader";
    }

    // load and create a texture
    // -------------------------

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D,
                  textureID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load("../static/55.jpg", &width, &height,
                                    &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

}


void render() {

    //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shader->use();


    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, textureID);

    auto mvp = vmath::ortho(0, SCR_WIDTH, SCR_HEIGHT, 0, 0, 3000);
    shader->setUniformMatrix4fv("MVP", mvp.operator float *());

    glDrawArrays(GL_TRIANGLES, 0, points.size() / 3);
    glDrawArrays(GL_TRIANGLES, 0, points.size() / 3);

    glDrawArrays(GL_TRIANGLES, 0, points.size() / 3);
    //glDrawArrays(GL_POINTS, 0, 10000);
    // glBindVertexArray(0); // no need to unbind it every time


    //save bmp
    std::vector<GLubyte> bmpData(SCR_WIDTH * SCR_HEIGHT * 4, 0);
    glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_BGRA, GL_UNSIGNED_BYTE, &bmpData[0]);
    saveBmpForRGBA(&bmpData[0], SCR_WIDTH, SCR_HEIGHT, "trangle_texture_color");
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
    // }
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------


}

void release() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &textureID);
    shader->clear();
    delete shader;
    glfwDestroyWindow(window);
    glfwTerminate();
}