#include "text_editor.h"

#include <SDL_opengl.h>
#include <SDL_pixels.h>

#include <algorithm>

#include "common.h"

void TextEditor::update_cursor_location(const Location& loc) {
  cursor_location = loc;
}

void TextEditor::update_text() {}

void TextEditor::on_draw() {
  glColor3f(1.0f, 0.0f, 0.0f);

  glBegin(GL_LINES);

  glVertex2f(0, engine->char_height * (cursor_location.y + 1));
  glVertex2f(WINDOW_WIDTH, engine->char_height * (cursor_location.y + 1));

  glEnd();

  SDL_Color color{(Uint8)255, (Uint8)255, (Uint8)255, (Uint8)255};

  int line_n = 0;
  auto [start_line, end_line] = model.all_lines();
  for (auto it = start_line; it != end_line; ++it) {
    auto line = *it;

    // Check if the line contains any non-whitespace characters
    bool has_visible_char = std::any_of(line.begin(), line.end(), [](char c) {
      return std::isgraph(static_cast<unsigned char>(c));
    });

    if (has_visible_char) {
      engine->render_text(line, 0, engine->char_height * line_n, color);
    }

    line_n++;
  }

  if (mode == Mode::INSERT) {
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(engine->char_width * (cursor_location.x + 1),
               engine->char_height * cursor_location.y);
    glVertex2f(engine->char_width * (cursor_location.x + 1),
               engine->char_height * (cursor_location.y + 1));
    glEnd();
  } else if (mode == Mode::NORMAL) {
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);

    glBegin(GL_POLYGON);

    glVertex2f(cursor_location.x * engine->char_width,
               cursor_location.y * engine->char_height);
    glVertex2f(cursor_location.x * engine->char_width + engine->char_width,
               cursor_location.y * engine->char_height);
    glVertex2f(cursor_location.x * engine->char_width + engine->char_width,
               cursor_location.y * engine->char_height + engine->char_height);
    glVertex2f(cursor_location.x * engine->char_width,
               cursor_location.y * engine->char_height + engine->char_height);

    glEnd();
  }
}
void TextEditor::on_key_event(const SDL_Event& e) {
  auto key = e.key.keysym.sym;

  switch (e.type) {
    case SDL_KEYDOWN: {
      keys_pressed.insert(key);

      if (mode == Mode::INSERT) {
        if (key == SDLK_ESCAPE) {
          mode = Mode::NORMAL;
        }
        return; 
      }

      // Movement
      if (key == SDLK_h || key == SDLK_LEFT) model.move_cursor_left();
      if (key == SDLK_l || key == SDLK_RIGHT) model.move_cursor_right();
      if (key == SDLK_k || key == SDLK_UP) model.move_cursor_up();
      if (key == SDLK_j || key == SDLK_DOWN) model.move_cursor_down();

      if (key == SDLK_ESCAPE) {
        engine->engine_running = false;
      }

      if (key == SDLK_i) {
        mode = Mode::INSERT;
      }

      if (key == SDLK_x) {
        if (e.key.keysym.mod & KMOD_SHIFT) {
          model.delete_after();
        } else {
          model.delete_before();
        }
      }

      break;
    }

    case SDL_KEYUP: {
      keys_pressed.erase(key);
      break;
    }
  }
}
