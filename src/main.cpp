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

unsigned int loadTexture(const char* path, int format)
{
  unsigned int texture;
  glGenTextures(1, &texture);

  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (!data)
    throw std::runtime_error("Failed to load texture");

  glBindTexture(GL_TEXTURE_2D, texture);

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);

  return texture;
}

int main()
{
  // SDL_SetHint(SDL_HINT_EVENT_LOGGING, "2");
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

  float vertices[] = {
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
       0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
  };

  unsigned int vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);


  Shader shader("shaders/boxface.vert", "shaders/boxface.frag");
  shader.use();
  shader.setInt("texture1", 0);
  shader.setInt("texture2", 1);

  glm::vec3 cubePositions[] = {
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

  unsigned int texture1 = loadTexture("assets/container.jpg", GL_RGB);
  unsigned int texture2 = loadTexture("assets/awesomeface.png", GL_RGBA);

  glEnable(GL_DEPTH_TEST);

  int width = 800;
  int height = 600;

  Camera camera;
  bool cameraFocused = true;
  bool ignoreNextMouseInput = false;

  SDL_SetRelativeMouseMode(SDL_TRUE);

  SDL_Event event;
  bool quit = false;
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

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(camera.zoom), float(width) / float(height), 0.1f, 100.0f);

    shader.setMat4("view", camera.getViewMatrix());
    shader.setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glBindVertexArray(vao);
    for (size_t i = 0; i < 10; i++)
    {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      if (i % 3 == 1)
        angle = angle * float(SDL_GetTicks()) / 1000;
      model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      shader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);

    ImGui::Text("yaw: %.3f", camera.yaw);
    ImGui::Text("pitch: %.3f", camera.pitch);
    ImGui::Text("pos: %.3f,%.3f,%.3f", camera.position.x, camera.position.y, camera.position.z);
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Text("Render: %.1f FPS (%.3f ms/frame)", io.Framerate, 1000.0f / io.Framerate);
    static bool useVsync = true;
    if (ImGui::Checkbox("Use vsync", &useVsync))
      SDL_GL_SetSwapInterval(useVsync ? 1 : 0);

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
