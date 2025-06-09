#pragma once

#include <unordered_set>

#include "common.h"
#include "component.h"
#include "engine.h"
#include "text_editor_model.h"

class TextEditor : public Component,
                   public CursorObserver,
                   public TextObserver {
 private:
  enum class Mode { NORMAL, INSERT, VISUAL };
  Engine* engine;
  std::unordered_set<SDL_Keycode> keys_pressed;
  TextEditorModel model;
  Mode mode;
  Location cursor_location;

 public:
  TextEditor(Engine* engine, std::string initial_text,
             LocationRange cursor_range)
      : engine(engine),
        mode(Mode::NORMAL),
        model(TextEditorModel(initial_text, cursor_range)) {
    model.add_cursor_observer(this);
    cursor_location = model.get_cursor_location();
  }

  ~TextEditor() {
    model.remove_cursor_observer(this);
  }

  void update_cursor_location(const Location& loc) override;
  void update_text() override;
  void on_draw() override;
  void on_key_event(const SDL_Event& e) override;
};
