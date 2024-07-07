#include "camera.hpp"
#include "shader.hpp"
#include <cassert>
#include <chrono>
#include <cmath>
#include <format>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <SDL.h>
#include <thread>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

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
  }
  else
    std::cout << "Texture failed to load at path: " << path << std::endl;

  stbi_image_free(data);

  return textureID;
}

int main()
{
  SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland,x11");
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    throw std::runtime_error("Failed to initialize SDL");

  SDL_GL_LoadLibrary(nullptr);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_Window* window = SDL_CreateWindow("Learn OpenGL", 0, 0, 800, 600,
                                        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);

  if (!window)
    throw std::runtime_error(SDL_GetError());

  SDL_GLContext context = SDL_GL_CreateContext(window);
  assert(context);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForOpenGL(window, context);
  ImGui_ImplOpenGL3_Init();

  gladLoadGLLoader(SDL_GL_GetProcAddress);
  std::cout << std::format("OpenGL version: {}", (const char*)(glGetString(GL_VERSION))) << std::endl;

  unsigned int VBO, cubeVAO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(cubeVAO);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // normal vectors
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // texture coordinates
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  unsigned int lightCubeVAO;
  glGenVertexArrays(1, &lightCubeVAO);
  glBindVertexArray(lightCubeVAO);

  // we only need to bind to the VBO, the container's VBO's data already contains the data.
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  Shader lightingShader("shaders/cube.vert", "shaders/cube.frag");
  Shader lightCubeShader("shaders/lightsource.vert", "shaders/lightsource.frag");

  unsigned int diffuseMap = loadTexture("assets/container2.png");
  unsigned int specularMap = loadTexture("assets/container2_specular.png");

  lightingShader.use();
  lightingShader.setInt("material.diffuse", 0);
  lightingShader.setInt("material.specular", 1);

  glEnable(GL_DEPTH_TEST);

  int width = 800;
  int height = 600;

  Camera camera;
  bool cameraFocused = true;
  bool ignoreNextMouseInput = false;

  SDL_SetRelativeMouseMode(SDL_TRUE);

  glm::vec4 lightColor(1.0f);
  glm::vec4 backgroundColor(0.15f);

  SDL_Event event;
  bool quit = false;

  uint64_t tick = 0;
  bool tickPaused = false;

  while (!quit)
  {
    using Clock = std::chrono::high_resolution_clock;
    using namespace std::chrono_literals;

    static Clock::time_point lastFrameTime = Clock::now();
    static Clock::duration accumulator;
    static constexpr std::chrono::nanoseconds timestep(1000000000 / 1000);

    Clock::time_point currentTime = Clock::now();
    Clock::duration frameTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    accumulator += frameTime;

    while (SDL_PollEvent(&event))
    {
      if (!cameraFocused)
        ImGui_ImplSDL2_ProcessEvent(&event);
      switch (event.type)
      {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
          ignoreNextMouseInput = true;
          SDL_GetWindowSizeInPixels(window, &width, &height);
          glViewport(0, 0, width, height);
        }
        break;
      case SDL_MOUSEMOTION:
        if (ignoreNextMouseInput)
          ignoreNextMouseInput = false;
        else if (cameraFocused)
          camera.processMouseMovement(event.motion.xrel, -event.motion.yrel);
        break;
      case SDL_MOUSEWHEEL:
        if (cameraFocused)
          camera.processMouseScroll(event.wheel.y);
        break;
      case SDL_KEYDOWN:
        if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE && !ImGui::GetIO().WantCaptureKeyboard)
        {
          cameraFocused = !cameraFocused;
          SDL_SetRelativeMouseMode(cameraFocused ? SDL_TRUE : SDL_FALSE);
        }
        break;
      }
    }

    if (quit)
      break;

    while (accumulator >= timestep)
    {
      if (!tickPaused)
        tick++;

      if (cameraFocused && !ImGui::GetIO().WantCaptureKeyboard)
      {
        const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
        if (keyboardState[SDL_SCANCODE_W])
          camera.processKeyboard(CameraMovement::FORWARD);
        if (keyboardState[SDL_SCANCODE_A])
          camera.processKeyboard(CameraMovement::LEFT);
        if (keyboardState[SDL_SCANCODE_S])
          camera.processKeyboard(CameraMovement::BACKWARD);
        if (keyboardState[SDL_SCANCODE_D])
          camera.processKeyboard(CameraMovement::RIGHT);
        if (keyboardState[SDL_SCANCODE_LSHIFT])
          camera.processKeyboard(CameraMovement::DOWN);
        if (keyboardState[SDL_SCANCODE_SPACE])
          camera.processKeyboard(CameraMovement::UP);
      }

      accumulator -= timestep;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), float(width) / float(height), 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    static bool pointLight = false;
    float angle = float(tick) / 1000;
    if (pointLight)
    {
      lightPos.x *= cos(angle);
      lightPos.z *= sin(angle);
    }

    // activate the shader and set uniforms
    lightingShader.use();
    // material
    lightingShader.setFloat("material.shininess", 64.0f);
    // light
    lightingShader.setVec3("light.ambient",  0.2f, 0.2f, 0.2f);
    lightingShader.setVec3("light.diffuse",  lightColor.x * 0.7f, lightColor.y * 0.7f, lightColor.z * 0.7f); // darken diffuse light a bit
    lightingShader.setVec3("light.specular", lightColor.x, lightColor.y, lightColor.z);
    // lightingShader.setFloat("light.constant",  1.0f);
    // lightingShader.setFloat("light.linear",    0.09f);
    // lightingShader.setFloat("light.quadratic", 0.032f);
    if (pointLight)
      lightingShader.setVec4("light.origin", glm::vec4(lightPos, 1.0f));
    else
      lightingShader.setVec4("light.origin", -0.2f, -1.0f, -0.3f, 0.0f);
    // camera
    lightingShader.setVec3("viewPos", camera.position);

    // world transformation
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);

    static glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    static bool rotateCube = true;
    for (unsigned int i = 0; i < 10; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        if (rotateCube)
          model = glm::rotate(model, glm::radians(angle * i), glm::vec3(1.0f, 0.3f, 0.5f));
        lightingShader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }


    // also draw the lamp object
    lightCubeShader.use();
    lightCubeShader.setVec3("lightColor",  lightColor.x, lightColor.y, lightColor.z);
    lightCubeShader.setMat4("projection", projection);
    lightCubeShader.setMat4("view", view);
    glm::mat4 model(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    lightCubeShader.setMat4("model", model);

    glBindVertexArray(lightCubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // debug GUI
    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f));
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
    static bool showDemoWindow = false;
    ImGui::Checkbox("Show demo window", &showDemoWindow);
    ImGui::SeparatorText("Camera");
    ImGui::Text("yaw: %.3f", camera.yaw);
    ImGui::Text("pitch: %.3f", camera.pitch);
    ImGui::Text("pos: %.3f,%.3f,%.3f", camera.position.x, camera.position.y, camera.position.z);
    ImGui::SeparatorText("Graphics");
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Text("Render: %.1f FPS (%.3f ms/frame)", io.Framerate, 1000.0f / io.Framerate);
    static bool useVsync = true;
    if (ImGui::Checkbox("Use vsync", &useVsync))
      SDL_GL_SetSwapInterval(useVsync ? 1 : 0);
    ImGui::SeparatorText("Simulation");
    ImGui::Text("Tick: %lu", tick);
    ImGui::Checkbox("Pause", &tickPaused);
    ImGui::Checkbox("Rotate cube", &rotateCube);
    ImGui::SeparatorText("Lighting");
    ImGui::Checkbox("Point light", &pointLight);
    ImGui::ColorEdit3("Light color", (float*)&lightColor, ImGuiColorEditFlags_NoInputs);
    ImGui::ColorEdit3("Background color", (float*)&backgroundColor, ImGuiColorEditFlags_NoInputs);
    ImGui::End();

    if (showDemoWindow)
      ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);

    // Cap framerate at max simulation speed
    std::chrono::duration thisFrameTime = Clock::now() - lastFrameTime;
    Clock::duration timeToSleep = timestep - accumulator - thisFrameTime;
    if (timeToSleep > Clock::duration::zero())
      std::this_thread::sleep_for(timeToSleep);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
