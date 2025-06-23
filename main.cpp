#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>

// Crappy 3x5 digit font (each digit is 3*5 = 15 bits)
const char* digits[] = {
    "111101101101111", // 0
    "010110010010111", // 1
    "111001111100111", // 2
    "111001111001111", // 3
    "101101111001001", // 4
    "111100111001111", // 5
    "111100111101111", // 6
    "111001001001001", // 7
    "111101111101111", // 8
    "111101111001111"  // 9
};

// Simple shader utils
GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    return s;
}

// Draw one digit using quads
void drawDigit(int digit, float x, float y, float size, GLuint shader) {
    glUseProgram(shader);
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (digits[digit][row * 3 + col] == '1') {
                float xpos = x + col * size;
                float ypos = y - row * size;
                float verts[] = {
                    xpos,     ypos,
                    xpos+size,ypos,
                    xpos+size,ypos+size,
                    xpos,     ypos+size
                };
                GLuint VAO, VBO;
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                glBindVertexArray(VAO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                glDeleteBuffers(1, &VBO);
                glDeleteVertexArrays(1, &VAO);
            }
        }
    }
}

void drawDepthText(float depth, GLuint shader) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << depth;
    std::string str = ss.str();
    float startX = -0.95f;
    for (char c : str) {
        if (c >= '0' && c <= '9')
            drawDigit(c - '0', startX, 0.9f, 0.04f, shader);
        else if (c == '.') {
            drawDigit(0, startX, 0.9f, 0.0f, shader); // just skip/space
        }
        startX += 0.19f;
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(800, 600, "Rotating Cube + Depth Debug", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int w, int h) { glViewport(0, 0, w, h); });
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Cube vertex data
    float vertices[] = {
        // positions          // colors
        -0.5f,-0.5f,-0.5f,  1.0f,0.0f,0.0f,
         0.5f,-0.5f,-0.5f,  0.0f,1.0f,0.0f,
         0.5f, 0.5f,-0.5f,  0.0f,0.0f,1.0f,
        -0.5f, 0.5f,-0.5f,  1.0f,1.0f,0.0f,
        -0.5f,-0.5f, 0.5f,  0.0f,1.0f,1.0f,
         0.5f,-0.5f, 0.5f,  1.0f,0.0f,1.0f,
         0.5f, 0.5f, 0.5f,  1.0f,1.0f,1.0f,
        -0.5f, 0.5f, 0.5f,  0.0f,0.0f,0.0f
    };
    unsigned int indices[] = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        0,1,5, 5,4,0,
        2,3,7, 7,6,2,
        0,3,7, 7,4,0,
        1,2,6, 6,5,1
    };

    GLuint cubeVAO, VBO, EBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    const char* cubeVS = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aColor;
        uniform mat4 mvp;
        out vec3 color;
        void main() {
            color = aColor;
            gl_Position = mvp * vec4(aPos, 1.0);
        }
    )";
    const char* cubeFS = R"(
        #version 330 core
        in vec3 color;
        out vec4 FragColor;
        void main() { FragColor = vec4(color, 1.0); }
    )";

    GLuint vs = compileShader(GL_VERTEX_SHADER, cubeVS);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, cubeFS);
    GLuint cubeShader = glCreateProgram();
    glAttachShader(cubeShader, vs);
    glAttachShader(cubeShader, fs);
    glLinkProgram(cubeShader);
    glDeleteShader(vs);
    glDeleteShader(fs);

    const char* textVS = "#version 330 core\nlayout(location=0) in vec2 aPos; void main(){gl_Position=vec4(aPos,0.0,1.0);}";
    const char* textFS = "#version 330 core\nout vec4 FragColor; void main(){ FragColor=vec4(1,1,0,1); }";
    GLuint textShader = glCreateProgram();
    glAttachShader(textShader, compileShader(GL_VERTEX_SHADER, textVS));
    glAttachShader(textShader, compileShader(GL_FRAGMENT_SHADER, textFS));
    glLinkProgram(textShader);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = glfwGetTime();
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.5f, 1.0f, 0.0f));
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
        glm::mat4 mvp = proj * view * model;

        glUseProgram(cubeShader);
        glUniformMatrix4fv(glGetUniformLocation(cubeShader, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        float depth;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glReadPixels(width / 2, height / 2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

        drawDepthText(depth, textShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
