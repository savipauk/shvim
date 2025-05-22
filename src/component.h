#pragma once

#include <SDL_events.h>

class Component {
 public:
  virtual ~Component() {}
  virtual void on_draw() = 0;
  virtual void on_key_event(const SDL_Event& e) {}
};
