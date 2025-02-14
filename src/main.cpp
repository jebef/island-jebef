#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "utils/core.h"
#include "utils/shader.h"
#include "utils/camera.h"
#include "utils/stb_image.h"
#include "utils/model.h"

unsigned int loadTexture(const char *path);

int main() 
{
    // init glfw and create a window 
    GLFWwindow* window = CreateWindow(3, 3, GLFW_OPENGL_CORE_PROFILE, g_screen_width, g_screen_height, "ISLAND DEMO");
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // register glfw callback functions here 
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    // load opengl function pointers with glad 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to retrieve OpenGL function pointers with GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state here 
    glEnable(GL_DEPTH_TEST);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // construct shader program
    // Shader core_shader = Shader("src/shaders/core.vert","src/shaders/core.frag");
    Shader mvp_shader = Shader("src/shaders/mvp.vert", "src/shaders/mvp.frag");
    Shader water_shader = Shader("src/shaders/water-debug.vert", "src/shaders/water-debug.frag");

    // load in model data
    Model palm_tree("src/resources/models/palm_tree/palm-tree.obj");

    // water will be represented as a 2D plane 
    float water_vertex_data[] = {
        // positions            // normals
        -1.0f, 0.0f, -1.0f,     0.0f, 1.0f, 0.0f,
         1.0f, 0.0f, -1.0f,     0.0f, 1.0f, 0.0f,
         1.0f, 0.0f,  1.0f,     0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,     0.0f, 1.0f, 0.0f,
         1.0f, 0.0f,  1.0f,     0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f,  1.0f,     0.0f, 1.0f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(water_vertex_data), water_vertex_data, GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);




    // float cube_vertex_data[] = {
    //     // positions          // normals           // texture coords
    //     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
    //      0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    //      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    //     -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    //     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    //     -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    //      0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    //     -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    //     -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    //     -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    //     -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    //     -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //     -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //     -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    //     -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    //      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    //      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //      0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    //      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    //     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    //      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    //      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    //      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    //     -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    //     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    //     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    //      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    //     -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    //     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    // };

    // // configure test cube memory buffers 
    // unsigned int VAO, VBO;
    // glGenBuffers(1, &VBO);
    // glGenVertexArrays(1, &VAO);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertex_data), cube_vertex_data, GL_STATIC_DRAW);

    // glBindVertexArray(VAO);
    // // position 
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // // normal
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    // // texture coordinates
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    // glEnableVertexAttribArray(2);

    // // configure light cube memory buffers, don't need another VBO - same vertex data
    // unsigned int VAO_l;
    // glGenVertexArrays(1, &VAO_l);
    
    // // bind buffers
    // glBindVertexArray(VAO_l);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO); // necessary?

    // // position 
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);

    // ----------- MAIN RENDER LOOP ----------- //
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float current_frame = static_cast<float>(glfwGetTime());
        g_delta_time = current_frame - g_last_frame;
        g_last_frame = current_frame;

        // handle user input 
        ProcessInput(window);

        // ----------- RENDER  ----------- //
        // clear/set background color
        glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render landscape
        water_shader.use();
        // set uniforms for water shader 

        glm::mat4 water_model = glm::mat4(1.0f);
        water_model = glm::scale(water_model, glm::vec3(5.0, 5.0, 5.0));
        water_shader.setMat4("model", water_model);

        glm::mat4 view = g_camera.GetViewMatrix();
        water_shader.setMat4("view", view);

        glm::mat4 projection = glm::perspective(glm::radians(g_camera.zoom_), (float)g_screen_width / (float)g_screen_height, 0.1f, 100.0f);
        water_shader.setMat4("projection", projection);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // activate shader program
        mvp_shader.use();
        // camera/view property
        mvp_shader.setVec3("camera_pos", g_camera.position_);
        // material properties
        mvp_shader.setFloat("material.shininess", 32.0f);
        // lighting properties
        // directional light 
        glm::vec3 dl_direction = glm::vec3(0.0f, -1.0f, 0.0f);
        glm::vec3 dl_ambient = glm::vec3(0.1f, 0.1f, 0.1f);
        glm::vec3 dl_diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        glm::vec3 dl_specular = glm::vec3(1.0f, 1.0f, 1.0f);
        // point lights
        glm::vec3 pl_position = glm::vec3(3.0f, 3.0f, -3.0f);
        glm::vec3 pl_ambient = glm::vec3(0.1f, 0.1f, 0.1f);
        glm::vec3 pl_diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        glm::vec3 pl_specular = glm::vec3(1.0f, 1.0f, 1.0f);
        float pl_constant = 1.0f;
        float pl_linear = 0.09f;
        float pl_quadratic = 0.032f;
        // spot lights 
        glm::vec3 sl_position = glm::vec3(1.0f, 1.0f, 0.0f);
        glm::vec3 sl_direction = glm::vec3(1.0f, 0.5f, 0.0f);
        glm::vec3 sl_ambient = glm::vec3(0.1f, 0.1f, 0.1f);
        glm::vec3 sl_diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        glm::vec3 sl_specular = glm::vec3(1.0f, 1.0f, 1.0f);
        float sl_constant = 1.0f;
        float sl_linear = 0.09f;
        float sl_quadratic = 0.032f;
        float sl_inner_cut_off = glm::cos(glm::radians(12.5f));
        float sl_outer_cut_off = glm::cos(glm::radians(15.0f));


        mvp_shader.setVec3("directional_light.direction", dl_direction);
        mvp_shader.setVec3("directional_light.ambient", dl_ambient);
        mvp_shader.setVec3("directional_light.diffuse", dl_diffuse);
        mvp_shader.setVec3("directional_light.specular", dl_specular);

        mvp_shader.setVec3("point_light[0].position", pl_position);
        mvp_shader.setVec3("point_light[0].ambient", pl_ambient);
        mvp_shader.setVec3("point_light[0].diffuse", pl_diffuse);
        mvp_shader.setVec3("point_light[0].specular", pl_specular);
        mvp_shader.setFloat("point_light[0].constant", pl_constant);
        mvp_shader.setFloat("point_light[0].linear", pl_linear);
        mvp_shader.setFloat("point_light[0].quadratic", pl_quadratic);

        mvp_shader.setVec3("spot_light[0].position", sl_position);
        mvp_shader.setVec3("spot_light[0].direction", sl_direction);
        mvp_shader.setVec3("spot_light[0].ambient", sl_ambient);
        mvp_shader.setVec3("spot_light[0].diffuse", sl_diffuse);
        mvp_shader.setVec3("spot_light[0].specular", sl_specular);
        mvp_shader.setFloat("spot_light[0].constant", sl_constant);
        mvp_shader.setFloat("spot_light[0].linear", sl_linear);
        mvp_shader.setFloat("spot_light[0].quadratic", sl_quadratic);
        mvp_shader.setFloat("spot_light[0].inner_cut_off", sl_inner_cut_off);
        mvp_shader.setFloat("spot_light[0].outer_cut_off", sl_outer_cut_off);

        // model matrix (local -> world)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
        mvp_shader.setMat4("model", model);

        // view matrix (world -> camera/view)
        //glm::mat4 view = g_camera.GetViewMatrix();
        mvp_shader.setMat4("view", view);

        // projection matrix (camera/view -> clip)
        // glm::perspective(fov, aspect_ratio, z_near, z_far)
        // creates a frustum ^
        //glm::mat4 projection = glm::perspective(glm::radians(g_camera.zoom_), (float)g_screen_width / (float)g_screen_height, 0.1f, 100.0f);
        mvp_shader.setMat4("projection", projection);

        // normal matrix
        // translates vec3 normals to world space ^
        glm::mat3 normal = glm::mat3(glm::transpose(glm::inverse(model)));
        mvp_shader.setMat3("normal", normal);

        // // bind diffuse map
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, diffuse_map);
        // // bind specular map
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, specular_map);

        // // draw model 
        // glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // draw model 
        palm_tree.Draw(mvp_shader);

        // swap frame and output buffers
        glfwSwapBuffers(window);
        // check for I/O events 
        glfwPollEvents();
    }
    
    // // de-allocate all resources
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteVertexArrays(1, &VAO_l);
    // glDeleteBuffers(1, &VBO);

    // free glfw resources 
    glfwTerminate();
    return 0; 
}

// GET RID OF THIS  
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}