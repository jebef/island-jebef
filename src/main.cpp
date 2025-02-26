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
void RenderWater(Shader shader, unsigned int VAO, unsigned int refl_tex_id,  unsigned int refr_tex_id);
void RenderWaterGui(Shader shader, unsigned int VAO, unsigned int texture_id, unsigned int index_offset);
void RenderDebugAxes(Shader shader, unsigned int VAO);

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

    // ----------- WATER ----------- //
    float vd_water[] = {
        // Positions         
        -1.0f, 0.0f, -1.0f, // top left
         1.0f, 0.0f, -1.0f, // top right
         1.0f, 0.0f,  1.0f, // bottom right
        -1.0f, 0.0f,  1.0f  // bottom left
    };
    unsigned int i_water[] = {
        0, 1, 2, 
        0, 2, 3
    };
    // generate water plane buffers 
    unsigned int VAO_W, VBO_W, EBO_W;
    glGenVertexArrays(1, &VAO_W);
    glGenBuffers(1, &VBO_W);
    glGenBuffers(1, &EBO_W);
    // bind vertex array object 
    glBindVertexArray(VAO_W);
    // configure vertex buffer data 
    glBindBuffer(GL_ARRAY_BUFFER, VBO_W);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vd_water), vd_water, GL_STATIC_DRAW);
    // configure element buffer data 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_W);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(i_water), i_water, GL_STATIC_DRAW);
    // configure vertex array ttribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // unbind 
    glBindVertexArray(0);
    // create water shader program
    Shader water_shader = Shader("src/shaders/water.vert", "src/shaders/water.frag");
    water_shader.use();
    // set texture sampler uniforms 
    water_shader.setInt("reflection_texture", 0);
    water_shader.setInt("refraction_texture", 1);
    // set model matrix uniform
    glm::mat4 model_water = glm::mat4(1.0f);
    model_water = glm::scale(model_water, glm::vec3(20.0f));
    water_shader.setMat4("model", model_water);
    // init water frame buffers and associated attachments
    WaterFrameBuffers(water_fbos);

    // ----------- DEBUG WATER GUI ----------- //
    float vd_gui[] = {
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
    unsigned int i_gui[] = {
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vd_gui), vd_gui, GL_STATIC_DRAW);
    // configure vertex array
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // configure element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_WGUI);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(i_gui), i_gui, GL_STATIC_DRAW);
    // unbind VAO_WGUI
    glBindVertexArray(0);
    // create gui shader program 
    Shader gui_shader = Shader("src/shaders/gui-debug.vert", "src/shaders/gui-debug.frag");
    gui_shader.use();
    // set texture sampler uniform
    gui_shader.setInt("texture_id", 0);

    // ----------- DEBUG AXES ----------- //
    float vd_axes[] = {
        // x axis
        -1.0f,  0.0f,  0.0f, 
         1.0f,  0.0f,  0.0f, 
        // y axis 
         0.0f, -1.0f,  0.0f, 
         0.0f,  1.0f,  0.0f, 
         // z axis 
         0.0f,  0.0f, -1.0f, 
         0.0f,  0.0f,  1.0f
    };
    // generate buffers 
    unsigned int VAO_AX, VBO_AX;
    glGenVertexArrays(1, &VAO_AX);
    glGenBuffers(1, &VBO_AX);
    // bind vertex array object 
    glBindVertexArray(VAO_AX);
    // configure vertex buffer 
    glBindBuffer(GL_ARRAY_BUFFER, VBO_AX);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vd_axes), vd_axes, GL_STATIC_DRAW);
    // configure vertex array attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); 
    glEnableVertexAttribArray(0);
    // unbind 
    glBindVertexArray(0);
    // create debug axes shader program
    Shader axes_shader("src/shaders/axes.vert", "src/shaders/axes.frag");
    axes_shader.use();
    // set static model uniform 
    glm::mat4 model_axes = glm::mat4(1.0f);
    model_axes = glm::scale(model_axes, glm::vec3(10.0f));
    axes_shader.setMat4("model", model_axes);


    // ----------- MAIN RENDER LOOP ----------- //
    while (!glfwWindowShouldClose(g_window)) {
        // per-frame time logic
        float current_frame = static_cast<float>(glfwGetTime());
        g_delta_time = current_frame - g_last_frame;
        g_last_frame = current_frame;

        // handle user input 
        ProcessInput(g_window);

        // enable clipping 
        glEnable(GL_CLIP_DISTANCE0);

        //// RENDER SCENE TO REFLECTION BUFFER ////
        // update camera position to capture reflection angles
        glm::vec3 current_camera_pos = g_camera.position_;
        g_camera.UpdateYPosition(g_camera.position_.y - (2 * g_camera.position_.y));
        g_camera.InvertPitch();
        // define reflection clip plane and set uniform
        mvp_shader.use();
        glm::vec4 refl_clip_plane = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
        mvp_shader.setVec4("clip_plane", refl_clip_plane);
        // bind reflection framebuffer and render 
        water_fbos.BindReflectionFrameBuffer();
        glClearColor(0.0f, 0.0f, 0.6f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderScene(models, mvp_shader);
        // unbind reflection framebuffer and restore camera settings 
        water_fbos.UnbindCurrentFrameBuffer();
        g_camera.InvertPitch();
        g_camera.UpdatePosition(current_camera_pos);

        //// RENDER SCENE TO REFRACTION BUFFER ////
        // define refraction clip plane and set uniform
        mvp_shader.use();
        glm::vec4 refr_clip_plane = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
        mvp_shader.setVec4("clip_plane", refr_clip_plane);
        // bind refraction framebuffer and render 
        water_fbos.BindRefractionFrameBuffer();
        glClearColor(0.0f, 0.0f, 0.6f, 0.5f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderScene(models, mvp_shader);
        // unbind refraction framebuffer 
        water_fbos.UnbindCurrentFrameBuffer();

        // disable clipping 
        glDisable(GL_CLIP_DISTANCE0);

        // render the scene
        glClearColor(0.2f, 0.0f, 0.2f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderScene(models, mvp_shader);
        
        // render water 
        RenderWater(water_shader, VAO_W, water_fbos.GetReflectionTexture(), water_fbos.GetRefractionTexture());

        // // DEBUG - water texture guis and axes 
        // RenderWaterGui(gui_shader, VAO_WGUI, water_fbos.GetReflectionTexture(), 0);
        // RenderWaterGui(gui_shader, VAO_WGUI, water_fbos.GetRefractionTexture(), 6);
        // RenderDebugAxes(axes_shader, VAO_AX);

        // swap frame and output buffers
        glfwSwapBuffers(g_window);
        // check for I/O events 
        glfwPollEvents();
    }
    // ----------- FREE RESOURCES ----------- //
    glDeleteVertexArrays(1, &VAO_W);
    glDeleteVertexArrays(1, &VAO_WGUI);
    glDeleteVertexArrays(1, &VAO_AX);
    glDeleteBuffers(1, &VBO_W);
    glDeleteBuffers(1, &VBO_WGUI);
    glDeleteBuffers(1, &VBO_AX);
    water_fbos.CleanUp();
    // free glfw resources 
    glfwTerminate();
    return 0; 
}

/*
    Render the scene to the actvive frame buffer.
*/
void RenderScene(const std::vector<Model> models, Shader shader) {
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

void RenderWater(Shader shader, unsigned int VAO, unsigned int refl_tex_id, unsigned int refr_tex_id) {
    // activate water shader 
    shader.use();
    // set dynamic matrix uniforms 
    glm::mat4 view = g_camera.GetViewMatrix();
    shader.setMat4("view", view);
    glm::mat4 projection = glm::perspective(glm::radians(g_camera.zoom_), (float)g_screen_width / (float)g_screen_height, 0.1f, 100.0f);
    shader.setMat4("projection", projection);
    // bind reflection texture 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, refl_tex_id);
    shader.setInt("reflection_texture", 0);
    // bind refraction texture 
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, refr_tex_id);
    shader.setInt("refraction_texture", 1);
    // bind water plane and draw 
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
    // unbind 
    glBindVertexArray(0);
}

void RenderWaterGui(Shader shader, unsigned int VAO, unsigned int texture_id, unsigned int index_offset) {
    glDisable(GL_DEPTH_TEST);
    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(index_offset * sizeof(GLuint)));
    glEnable(GL_DEPTH_TEST);
}

void RenderDebugAxes(Shader shader, unsigned int VAO) {
    // bind axes shader program
    shader.use();
    // update dynamic matrix uniforms 
    glm::mat4 projection = glm::perspective(glm::radians(g_camera.zoom_), (float)g_screen_width / (float)g_screen_height, 0.1f, 100.0f);
    shader.setMat4("projection", projection);
    glm::mat4 view = g_camera.GetViewMatrix();
    shader.setMat4("view", view);
    
    // x axis
    shader.setVec3("axisColor", 1.0f, 0.0f, 0.0f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
    // y axis 
    shader.setVec3("axisColor", 0.0f, 1.0f, 0.0f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 2, 2);
    // z axis 
    shader.setVec3("axisColor", 0.0f, 0.0f, 1.0f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 4, 2);
}