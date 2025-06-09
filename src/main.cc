#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <SDL_video.h>

#include <iostream>

#include "common.h"
#include "engine.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "text_editor.h"

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    return 1;
  }

  if (TTF_Init() == -1) {
    std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError()
              << std::endl;
    SDL_Quit();
    return 1;
  }

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
    TTF_Quit();
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

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  TTF_Font* font =
      TTF_OpenFont("../third_party/fonts/RobotoMono-Regular.ttf", 24);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load font: %s",
                 TTF_GetError());
    SDL_Quit();
    TTF_Quit();
  }

  Engine engine(window, gl_context, font);
  engine.run();

  TTF_Quit();
  SDL_Quit();

  return 0;
}
