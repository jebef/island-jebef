#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "utils/core.h"
#include "utils/shader.h"
#include "utils/camera.h"
#include "utils/stb_image.h"
#include "utils/model.h"
#include "utils/water_frame_buffers.h"

/*
    1. Setup window
    2. Register callback functions 
    3. Compile shaders
    4. Load models 
    5. Set uniforms 
    6. Render 


*/

void RenderScene(const std::vector<Model> models, Shader shader);
void RenderSceneToTexture(WaterFrameBuffers water_fbos);
void RenderWater(Shader shader, unsigned int VAO);
void RenderWaterGui(Shader shader, unsigned int VAO, unsigned int texture_id, unsigned int index_offset);

int main() 
{
    // init glfw and create a window 
    GLFWwindow* g_window = CreateWindow(3, 3, GLFW_OPENGL_CORE_PROFILE, g_screen_width, g_screen_height, "ISLAND DEMO");
    if (g_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(g_window);

    float width_scale, height_scale;
    glfwGetWindowContentScale(g_window, &width_scale, &height_scale);
    g_screen_width_p = static_cast<unsigned int>(g_screen_width * width_scale);
    g_screen_height_p = static_cast<unsigned int>(g_screen_height * height_scale);

    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // register glfw callback functions here 
    glfwSetFramebufferSizeCallback(g_window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(g_window, MouseCallback);
    glfwSetScrollCallback(g_window, ScrollCallback);

    // load opengl function pointers with glad 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to retrieve OpenGL function pointers with GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state here 
    glEnable(GL_DEPTH_TEST);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // construct shader programs
    Shader mvp_shader = Shader("src/shaders/mvp.vert", "src/shaders/mvp.frag");

    // load in model data
    Model palm_tree("src/resources/models/palm_tree/palm-tree.obj");
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
    model = glm::translate(model, glm::vec3(-1.0f,0.0f,0.0f));
    palm_tree.SetModelMatrix(model);

    Model island("src/resources/models/island/island.obj");
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(2.0f));
    model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));
    island.SetModelMatrix(model);

    const std::vector<Model> models = { island, palm_tree };
    
    // ----------- SET SHADER UNIFORMS ----------- // 
    //// TERRAIN
    mvp_shader.use();
    // material properties
    mvp_shader.setFloat("material.shininess", 16.0f);
    // lighting properties
    bool dir_only = true;
    mvp_shader.setBool("dir_only", dir_only);
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

    //// WATER
    // water will be represented as a 2D plane 
    // WATER IS DEFINED TO BE AT y = 0.0f
    float water_vertex_data[] = {
        // positions            // normals
        -1.0f, 0.0f, -1.0f,     0.0f, 1.0f, 0.0f,
         1.0f, 0.0f, -1.0f,     0.0f, 1.0f, 0.0f,
         1.0f, 0.0f,  1.0f,     0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,     0.0f, 1.0f, 0.0f,
         1.0f, 0.0f,  1.0f,     0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f,  1.0f,     0.0f, 1.0f, 0.0f
    };
    // generate buffers 
    unsigned int VAO_W, VBO_W;
    glGenVertexArrays(1, &VAO_W);
    glGenBuffers(1, &VBO_W);
    glBindVertexArray(VAO_W);
    // configure vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO_W);
    glBufferData(GL_ARRAY_BUFFER, sizeof(water_vertex_data), water_vertex_data, GL_STATIC_DRAW);
    // configure vertex array
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // unbind VAO_W 
    glBindVertexArray(0);
    // create water shader program
    Shader water_shader = Shader("src/shaders/mvp-debug.vert", "src/shaders/water-debug.frag");
    // init water frame buffers and associated attachments
    WaterFrameBuffers(water_fbos);

    //// WATER GUI 
    float gui_vertex_data[] = {
        // reflection gui
        -1.0f, 1.0f, 0.0f,  0.0f, 1.0f, // top left
        -0.5f, 1.0f, 0.0f,  1.0f, 1.0f, // top right
        -0.5f, 0.5f, 0.0f,  1.0f, 0.0f, // bottom right
        -1.0f, 0.5f, 0.0f,  0.0f, 0.0f, // bottom left
        // refraction gui
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, // top left
         1.0f, -0.5f, 0.0f,  1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right
         0.5f, -1.0f, 0.0f,  0.0f, 0.0f // bottom left
    };
    unsigned int gui_indices[] = {
        // reflection gui 
        0, 1, 2, // first triangle
        0, 2, 3, // second triangle
        // refraction gui 
        4, 5, 6, // first triangle
        4, 6, 7 // second triangle
    };
    // generate buffers
    unsigned int VAO_WGUI, VBO_WGUI, EBO_WGUI;
    glGenVertexArrays(1, &VAO_WGUI);
    glGenBuffers(1, &VBO_WGUI);
    glGenBuffers(1, &EBO_WGUI);
    glBindVertexArray(VAO_WGUI);
    // configure vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO_WGUI);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gui_vertex_data), gui_vertex_data, GL_STATIC_DRAW);
    // configure vertex array
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // configure element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_WGUI);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gui_indices), gui_indices, GL_STATIC_DRAW);
    // unbind VAO_WGUI
    glBindVertexArray(0);
    // create gui shader program 
    Shader gui_shader = Shader("src/shaders/gui-debug.vert", "src/shaders/gui-debug.frag");
    gui_shader.use();
    // set texture sampler uniform
    gui_shader.setInt("texture_id", 0);

    // ----------- MAIN RENDER LOOP ----------- //
    while (!glfwWindowShouldClose(g_window)) {
        // per-frame time logic
        float current_frame = static_cast<float>(glfwGetTime());
        g_delta_time = current_frame - g_last_frame;
        g_last_frame = current_frame;

        // handle user input 
        ProcessInput(g_window);

        // render scene to reflection buffer
        // save current camera position 
        glm::vec3 current_camera_pos = g_camera.position_;
        // update camera position to capture reflection angles
        g_camera.UpdateYPosition(g_camera.position_.y - (2 * g_camera.position_.y));
        g_camera.InvertPitch();
        // bind reflection frame buffer and render 
        water_fbos.BindReflectionFrameBuffer();
        // define reflection clip plane and set uniform
        glm::vec4 refl_clip_plane = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
        mvp_shader.setVec4("clip_plane", refl_clip_plane);
        RenderScene(models, mvp_shader);
        water_fbos.UnbindCurrentFrameBuffer();
        // restore camera settings 
        g_camera.InvertPitch();
        g_camera.UpdatePosition(current_camera_pos);

        // render scene to refraction buffer
        water_fbos.BindRefractionFrameBuffer();
        RenderScene(models, mvp_shader);
        water_fbos.UnbindCurrentFrameBuffer();
        
        // render scene to default frame buffer
        mvp_shader.setBool("clip", false);
        mvp_shader.setVec4("clip_plane", refl_clip_plane);
        RenderScene(models, mvp_shader);
        
        // render water 
        RenderWater(water_shader, VAO_W);

        // render scene to water gui - DEBUG 
        RenderWaterGui(gui_shader, VAO_WGUI, water_fbos.GetReflectionTexture(), 0);
        RenderWaterGui(gui_shader, VAO_WGUI, water_fbos.GetRefractionTexture(), 6);

        // swap frame and output buffers
        glfwSwapBuffers(g_window);
        // check for I/O events 
        glfwPollEvents();
    }

    // FREE WATER RESOURCES
    water_fbos.CleanUp();

    // free glfw resources 
    glfwTerminate();
    return 0; 
}

/*
    Render the scene to the actvive frame buffer.
*/
void RenderScene(const std::vector<Model> models, Shader shader) {
    glEnable(GL_DEPTH_TEST);
    shader.use();

    // update camera/view uniform
    shader.setVec3("camera_pos", g_camera.position_);

    // --- update global uniforms --- //
    // view matrix (world -> camera/view)
    glm::mat4 view = g_camera.GetViewMatrix();
    shader.setMat4("view", view);

    // projection matrix (camera/view -> clip)
    // glm::perspective(fov, aspect_ratio, z_near, z_far)
    // creates a frustum ^
    glm::mat4 projection = glm::perspective(glm::radians(g_camera.zoom_), (float)g_screen_width / (float)g_screen_height, 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    // clear the color and depth buffers 
    glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw models 
    for (const Model& model : models) {
        // --- set model and normal uniforms --- // 
        // model matrix (local -> world)
        shader.setMat4("model", model.model_matrix);
        // normal matrix - translates vec3 normals to world space 
        glm::mat3 normal = glm::mat3(glm::transpose(glm::inverse(model.model_matrix)));
        shader.setMat3("normal", normal);
        // draw 
        model.Draw(shader);
    }
}

void RenderWater(Shader shader, unsigned int VAO) {
    // activate water shader 
    shader.use();
    // set matrix uniforms for water shader 
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(20.0f));
    shader.setMat4("model", model);
    glm::mat4 view = g_camera.GetViewMatrix();
    shader.setMat4("view", view);
    glm::mat4 projection = glm::perspective(glm::radians(g_camera.zoom_), (float)g_screen_width / (float)g_screen_height, 0.1f, 100.0f);
    shader.setMat4("projection", projection);
    // bind water VAO and draw 
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void RenderWaterGui(Shader shader, unsigned int VAO, unsigned int texture_id, unsigned int index_offset) {
    glDisable(GL_DEPTH_TEST);
    shader.use();
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(index_offset * sizeof(GLuint)));
    glEnable(GL_DEPTH_TEST);
}