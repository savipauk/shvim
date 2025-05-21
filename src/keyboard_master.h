#pragma once

#include "component.h"
#include "engine.h"

class KeyboardMaster : public Component {
 private:
  Engine* engine;

 public:
  KeyboardMaster(Engine* engine) : engine(engine) {}
  void on_draw() override;
  void on_key_down(const SDL_KeyboardEvent& key) override;
};
