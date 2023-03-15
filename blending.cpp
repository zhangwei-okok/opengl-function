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

#include <unistd.h>

// settings
const unsigned int SCR_WIDTH = 256;
const unsigned int SCR_HEIGHT = 128;
const int SIZE = 32;
int cnt;
std::vector<GLfloat> points;

unsigned int *vao, *vbo, textureID1, textureID2, textureID3;
unsigned int *texVBO;
GLSLProgram *shader = nullptr;

GLFWwindow *window;

GLuint perfCheckerTexture(GLsizei width, GLsizei height, int index) {
    auto image = new unsigned char[width * height * 4];
    GLint i, j, k;

    int r = 0;
    int g = 0;
    int b = 0;

    if (index == 0)
        r = 255;
    else if (index == 1) {
        g = 255;
    } else {
        b = 255;
    }


    k = 0;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            //GLubyte color;
            //if (((i / 32) ^ (j / 32)) & 1)
            //    color = 0xff;
            //else
            //    color = 0x0;

            image[k++] = r;
            image[k++] = g;
            image[k++] = b;
            image[k++] = 128;
        }
    }

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    delete[]image;
    return id;
}

void init();

void render();

unsigned int gentexture(const char *);

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
    //glEnable(GL_PROGRAM_POINT_SIZE);

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
    int cnt = points.size() / 9;
    vao = new GLuint[cnt];
    vbo = new GLuint[cnt];
    texVBO = new GLuint[cnt];
    glGenVertexArrays(cnt, vao);
    glGenBuffers(cnt, vbo);
    glGenBuffers(cnt, texVBO);
    for (int i = 0; i < cnt; ++i) {
        glBindVertexArray(vao[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);

        glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), &points[0] + 9 * i, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, texVBO[i]);
        glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), &texList[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 28, (void *) 0);
        glEnableVertexAttribArray(1);

    }


    shader = new GLSLProgram;
    if (!shader->build("../shader/texture.vert", "../shader/texture.frag")) {
        cout << "failed to load shader";
    }

    textureID1 = perfCheckerTexture(64, 64, 0);
    textureID2 = perfCheckerTexture(64, 64, 1);
    textureID3 = perfCheckerTexture(64, 64, 2);
   
   // glEnable(GL_COLOR_LOGIC_OP);
   //  glLogicOp(GL_OR_INVERTED);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); 
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

}


void render() {


    //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
   // glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    shader->use();
    int row, col;

    int i = 0;
    for (int i = 0; i < 2; ++i) {
        glBindVertexArray(vao[i]);
        glBindTexture(GL_TEXTURE_2D, textureID1);

        auto mvp = vmath::ortho(0, SCR_WIDTH, SCR_HEIGHT, 0, 0, 3000);
        float PI = 3.1415926;
        //mvp = mvp * vmath::rotate<float>(PI = 3.1415926, 0, 0, 0);
        auto view = vmath::mat4::identity();

        row = i % (SCR_HEIGHT / SIZE + 1);//(i * SIZE % SCR_HEIGHT) / SIZE;
        col = (i) / (SCR_HEIGHT / SIZE + 1);// % (1 + row);//i /row* SIZE / SCR_WIDTH;
         // cout << "row" << row << ":" << "clo:" << col << "\n";
        int x = SIZE * col + 16;
        int y = SIZE * row + 16;
        // std::cout << x << " - " << y << "\n";
        view *= vmath::translate<float>(x, y, 0);
        view *= vmath::rotate<float>(glfwGetTime() * 5000, 0, 0, 1);
        view *= vmath::translate<float>(-x, -y, 0);
        mvp = mvp * view;
        shader->setUniformMatrix4fv("MVP", mvp.operator float *());
        if(row==0){
         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        
       
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
/*
    for (int i = 0; i < 2; ++i) {
        glBindVertexArray(vao[i]);
        glBindTexture(GL_TEXTURE_2D, textureID2);

        auto mvp = vmath::ortho(0, SCR_WIDTH, SCR_HEIGHT, 0, 0, 3000);
        float PI = 3.1415926;
        mvp = mvp * vmath::rotate<float>(PI = 3.1415926, 0, 0, 0);
        auto view = vmath::mat4::identity();

        row = i % (SCR_HEIGHT / SIZE + 1);//(i * SIZE % SCR_HEIGHT) / SIZE;
        col = (i) / (SCR_HEIGHT / SIZE + 1);// % (1 + row);//i /row* SIZE / SCR_WIDTH;
        
        int x = SIZE * col + 16;
        int y = SIZE * row + 16;
        
        view *= vmath::translate<float>(x, y, 0);
        view *= vmath::rotate<float>(glfwGetTime() * 2000, 0, 0, 1);
        view *= vmath::translate<float>(-x, -y, 0);
        mvp = mvp * view;
        shader->setUniformMatrix4fv("MVP", mvp.operator float *());

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
*/
    
    for (int i = 0; i < 2; ++i) {
        glBindVertexArray(vao[i]);
        glBindTexture(GL_TEXTURE_2D, textureID3);

        auto mvp = vmath::ortho(0, SCR_WIDTH, SCR_HEIGHT, 0, 0, 3000);
        float PI = 3.1415926;
        mvp = mvp * vmath::rotate<float>(PI = 3.1415926, 0, 0, 0);
        auto view = vmath::mat4::identity();

        row = i % (SCR_HEIGHT / SIZE + 1);//(i * SIZE % SCR_HEIGHT) / SIZE;
        col = i % (SCR_HEIGHT / SIZE + 1);// % (1 + row);//i /row* SIZE / SCR_WIDTH;
       // cout << "row" << row << ":" << "clo:" << col << "\n";
        int x = SIZE * col + 16;
        int y = SIZE * row + 16;
        
        view *= vmath::translate<float>(x, y, 0);
        view *= vmath::rotate<float>(glfwGetTime() * 100, 0, 0, 1);
         view *= vmath::translate<float>(-x, -y, 0);
        mvp = mvp * view;
        shader->setUniformMatrix4fv("MVP", mvp.operator float *());
        if(row==0){
         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else {
         glDisable(GL_BLEND);
        } 
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
   
 

    //save bmp
    std::vector<GLubyte> bmpData(SCR_WIDTH * SCR_HEIGHT * 4, 0);
    glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_BGRA, GL_UNSIGNED_BYTE, &bmpData[0]);
    saveBmpForRGBA(&bmpData[0], SCR_WIDTH, SCR_HEIGHT, "blending");
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
    // }
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------



}

void release() {
    glDeleteVertexArrays(points.size() / 9, vao);
    glDeleteBuffers(points.size() / 9, vbo);
    glDeleteBuffers(points.size() / 9, texVBO);
    glDeleteTextures(1, &textureID1);
    glDeleteTextures(1, &textureID2);
    shader->clear();
    delete shader;
    glfwDestroyWindow(window);
    glfwTerminate();
}

unsigned int gentexture(const char *file) {

    // load and create a texture
    // -------------------------
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D,
                  id); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
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
    unsigned char *data = stbi_load(file, &width, &height,
                                    &nrChannels, 0);
    if (data) {
        int format = nrChannels == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);


    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return id;
}
