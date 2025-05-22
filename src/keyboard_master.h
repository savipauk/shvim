#pragma once

#include <unordered_set>

#include "common.h"
#include "component.h"
#include "engine.h"

class KeyboardMaster : public Component {
 private:
  enum class Mode { NORMAL, INSERT };
  Engine* engine;
  Vec2 cursor_position{0, 0};
  Vec2 move{0, 0};
  std::unordered_set<SDL_Keycode> keys_pressed;
  Mode mode;

 public:
  KeyboardMaster(Engine* engine) : engine(engine), mode(Mode::NORMAL) {}
  void on_draw() override;
  void on_key_event(const SDL_Event& e) override;
};
