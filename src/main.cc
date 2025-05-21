#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_video.h>

#include <iostream>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define TARGET_FPS 60

bool game_running = true;

void process_input() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    ImGui_ImplSDL2_ProcessEvent(&e);
    switch (e.type) {
      case SDL_QUIT:
        game_running = false;
        break;
    }
  }
}

void update() {}

bool render() {
  // Background
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Make (0,0) top left
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Error checking
  GLenum err = glGetError();
  switch (err) {
    case GL_NO_ERROR:
      return true;

    case GL_INVALID_OPERATION:
    case GL_OUT_OF_MEMORY:
      std::cout << "OpenGL Error: " << std::to_string(err) << "\n";
      std::cout << "Critical OpenGL Error. Stopping rendering.\n";
      return false;

    default:
      std::cout << "OpenGL Error: " << std::to_string(err) << "\n";
      return true;
  }
}

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) return 1;

  SDL_Window* window = SDL_CreateWindow(
      "shvim", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
      WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
    return 1;
  }

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr) {
    std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << "\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_GL_MakeCurrent(window, gl_context);

  // Enable vsync
  SDL_GL_SetSwapInterval(1);

  const char* glsl_version = "#version 130";

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard |
                    ImGuiConfigFlags_NavEnableSetMousePos;

  io.FontGlobalScale = 1.5f;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  while (game_running) {
    process_input();

    ImGuiIO& io = ImGui::GetIO();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    update();
    bool noerr = render();
    if (!noerr) {
      return 1;
    }

    // Render ImGui
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap the window
    SDL_GL_SwapWindow(window);

    // Frame limiting
    SDL_Delay(1000 * (1.0 / TARGET_FPS));

    // Error checking
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
      std::cout << "OpenGL Error: " << std::to_string(err) << "\n";
    }
  }

  return 0;
}
