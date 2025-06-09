#pragma once

#include <SDL_opengl.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>

#include <string>
#include <vector>

#include "component.h"

class Engine {
 private:
  std::vector<Component*> components;
  SDL_Window* window;
  SDL_GLContext gl_context;
  TTF_Font* font;
  uint64_t last_frame_time;

 public:
  int char_width = 0;
  int char_height = 0;
  bool engine_running;

  Engine(SDL_Window* window, SDL_GLContext gl_context, TTF_Font* font)
      : window(window),
        gl_context(gl_context),
        font(font),
        engine_running(true),
        last_frame_time(SDL_GetTicks64()) {
  }

  // Adds a component to the components vector
  void add(Component* c);

  // Calls on_draw() for components
  // Polls for input and calls on_key_down() for components
  void run();

  void render_text(const std::string& message, int x, int y, SDL_Color color);
};
