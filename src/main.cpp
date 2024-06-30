#include "camera.hpp"
#include "material.hpp"
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
#include <map>
#include <thread>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

  std::map<std::string, Material> materials = {
    {"emerald", Material(0.0215f, 0.1745f, 0.0215f, 0.07568f,  0.61424f,  0.07568f,  0.633f,  0.727811f, 0.633f,  0.6f)},
    {"jade", Material(0.135f,  0.2225f, 0.1575f, 0.54f, 0.89f, 0.63f, 0.316228f, 0.316228f, 0.316228f, 0.1f)},
    {"obsidian", Material(0.05375f,  0.05f, 0.06625f,  0.18275f,  0.17f, 0.22525f,  0.332741f, 0.328634f, 0.346435f, 0.3f)},
    {"pearl", Material(0.25f, 0.20725f,  0.20725f,  1.0f,  0.829f,  0.829f,  0.296648f, 0.296648f, 0.296648f, 0.088f)},
    {"ruby", Material(0.1745f, 0.01175f,  0.01175f,  0.61424f,  0.04136f,  0.04136f,  0.727811f, 0.626959f, 0.626959f, 0.6f)},
    {"turquoise", Material(0.1f,  0.18725f,  0.1745f, 0.396f,  0.74151f,  0.69102f,  0.297254f, 0.30829f,  0.306678f, 0.1f)},
    {"brass", Material(0.329412f, 0.223529f, 0.027451f, 0.780392f, 0.568627f, 0.113725f, 0.992157f, 0.941176f, 0.807843f, 0.21794872f)},
    {"bronze", Material(0.2125f, 0.1275f, 0.054f,  0.714f,  0.4284f, 0.18144f,  0.393548f, 0.271906f, 0.166721f, 0.2f)},
    {"chrome", Material(0.25f, 0.25f, 0.25f, 0.4f,  0.4f,  0.4f,  0.774597f, 0.774597f, 0.774597f, 0.6f)},
    {"copper", Material(0.19125f,  0.0735f, 0.0225f, 0.7038f, 0.27048f,  0.0828f, 0.256777f, 0.137622f, 0.086014f, 0.1f)},
    {"gold", Material(0.24725f,  0.1995f, 0.0745f, 0.75164f,  0.60648f,  0.22648f,  0.628281f, 0.555802f, 0.366065f, 0.4f)},
    {"silver", Material(0.19225f,  0.19225f,  0.19225f,  0.50754f,  0.50754f,  0.50754f,  0.508273f, 0.508273f, 0.508273f, 0.4f)},
    {"blackPlastic", Material(0.0f,  0.0f,  0.0f,  0.01f, 0.01f, 0.01f, 0.50f, 0.50f, 0.50f, .25f)},
    {"cyanPlastic", Material(0.0f,  0.1f,  0.06f, 0.0f,  0.50980392f, 0.50980392f, 0.50196078f, 0.50196078f, 0.50196078f, .25f)},
    {"greenPlastic", Material(0.0f,  0.0f,  0.0f,  0.1f,  0.35f, 0.1f,  0.45f, 0.55f, 0.45f, .25f)},
    {"redPlastic", Material(0.0f,  0.0f,  0.0f,  0.5f,  0.0f,  0.0f,  0.7f,  0.6f,  0.6f,  .25f)},
    {"whitePlastic", Material(0.0f,  0.0f,  0.0f,  0.55f, 0.55f, 0.55f, 0.70f, 0.70f, 0.70f, .25f)},
    {"yellowPlastic", Material(0.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.0f,  0.60f, 0.60f, 0.50f, .25f)},
    {"blackRubber", Material(0.02f, 0.02f, 0.02f, 0.01f, 0.01f, 0.01f, 0.4f,  0.4f,  0.4f,  .078125f)},
    {"cyanRubber", Material(0.0f,  0.05f, 0.05f, 0.4f,  0.5f,  0.5f,  0.04f, 0.7f,  0.7f,  .078125f)},
    {"greenRubber", Material(0.0f,  0.05f, 0.0f,  0.4f,  0.5f,  0.4f,  0.04f, 0.7f,  0.04f, .078125f)},
    {"redRubber", Material(0.05f, 0.0f,  0.0f,  0.5f,  0.4f,  0.4f,  0.7f,  0.04f, 0.04f, .078125f)},
    {"whiteRubber", Material(0.05f, 0.05f, 0.05f, 0.5f,  0.5f,  0.5f,  0.7f,  0.7f,  0.7f,  .078125f)},
    {"yellowRubber", Material(0.05f, 0.05f, 0.0f,  0.5f,  0.5f,  0.4f,  0.7f,  0.7f,  0.04f, .078125f)},
  };

  float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
  };

  unsigned int VBO, cubeVAO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(cubeVAO);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // normal vectors
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  unsigned int lightCubeVAO;
  glGenVertexArrays(1, &lightCubeVAO);
  glBindVertexArray(lightCubeVAO);

  // we only need to bind to the VBO, the container's VBO's data already contains the data.
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  Shader lightingShader("shaders/cube.vert", "shaders/cube.frag");
  Shader lightCubeShader("shaders/lightsource.vert", "shaders/lightsource.frag");

  glEnable(GL_DEPTH_TEST);

  int width = 800;
  int height = 600;

  Camera camera;
  bool cameraFocused = true;
  bool ignoreNextMouseInput = false;

  SDL_SetRelativeMouseMode(SDL_TRUE);

  ImVec4 lightColor(1.0f, 1.0f, 1.0f, 1.0f);

  SDL_Event event;
  bool quit = false;

  Material& selectedMaterial = materials["greenRubber"];

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
        else if (cameraFocused && !ImGui::GetIO().WantCaptureMouse)
          camera.processMouseMovement(event.motion.xrel, -event.motion.yrel);
        break;
      case SDL_MOUSEWHEEL:
        if (cameraFocused && !ImGui::GetIO().WantCaptureMouse)
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

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), float(width) / float(height), 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    static bool rotateLight = false;
    float angle = float(SDL_GetTicks()) / 1000;
    if (rotateLight)
    {
      lightPos.x *= cos(angle);
      lightPos.z *= sin(angle);
    }

    // activate the shader and set uniforms
    lightingShader.use();
    // material
    lightingShader.setVec3("material.ambient", selectedMaterial.ambient);
    lightingShader.setVec3("material.diffuse", selectedMaterial.diffuse);
    lightingShader.setVec3("material.specular", selectedMaterial.specular);
    lightingShader.setFloat("material.shininess", selectedMaterial.shininess * 32.0f);
    // light
    lightingShader.setVec3("light.ambient",  1.0f, 1.0f, 1.0f);
    lightingShader.setVec3("light.diffuse",  lightColor.x * 0.5f, lightColor.y * 0.5f, lightColor.z * 0.5f); // darken diffuse light a bit
    lightingShader.setVec3("light.specular", lightColor.x, lightColor.y, lightColor.z);
    lightingShader.setVec3("light.position", lightPos);
    // camera
    lightingShader.setVec3("viewPos", camera.position);

    // world transformation
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);
    glm::mat4 model(1.0f);
    static bool rotateCube = true;
    if (rotateCube)
      model = glm::rotate(model, glm::radians(20.0f * angle), glm::vec3(1.0f, 0.3f, 0.5f));
    lightingShader.setMat4("model", model);

    // render the cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // also draw the lamp object
    lightCubeShader.use();
    lightCubeShader.setVec3("lightColor",  lightColor.x, lightColor.y, lightColor.z);
    lightCubeShader.setMat4("projection", projection);
    lightCubeShader.setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    lightCubeShader.setMat4("model", model);

    glBindVertexArray(lightCubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    // debug GUI
    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f));
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
    ImGui::Text("yaw: %.3f", camera.yaw);
    ImGui::Text("pitch: %.3f", camera.pitch);
    ImGui::Text("pos: %.3f,%.3f,%.3f", camera.position.x, camera.position.y, camera.position.z);
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Text("Render: %.1f FPS (%.3f ms/frame)", io.Framerate, 1000.0f / io.Framerate);
    static bool useVsync = true;
    if (ImGui::Checkbox("Use vsync", &useVsync))
      SDL_GL_SetSwapInterval(useVsync ? 1 : 0);
    ImGui::Checkbox("Rotate cube", &rotateCube);
    // ImGui::Text("Cube color:");
    // ImGui::SameLine();
    // ImGui::ColorEdit3("Cube color", (float*)&cubeColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    if (ImGui::Button("Cube material..."))
      ImGui::OpenPopup("Select cube material");
    if (ImGui::BeginPopup("Select cube material"))
    {
      for (auto& [name, material] : materials)
        if (ImGui::Selectable(name.c_str()))
          selectedMaterial = material;
      ImGui::EndPopup();
    }

    ImGui::Text("Light color:");
    ImGui::SameLine();
    ImGui::ColorEdit3("Light color", (float*)&lightColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    ImGui::Checkbox("Move light", &rotateLight);
    static bool showDemoWindow = false;
    ImGui::Checkbox("Show demo window", &showDemoWindow);
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
