#include "text_editor.h"

#include <SDL_keycode.h>
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
    int x_offset = 1;
    glVertex2f(engine->char_width * (cursor_location.x + 1) + x_offset,
               engine->char_height * cursor_location.y);
    glVertex2f(engine->char_width * (cursor_location.x + 1) + x_offset,
               engine->char_height * (cursor_location.y + 1));
    glEnd();
  } else if (mode == Mode::VISUAL) {
    LocationRange range = model.get_selection_range();
    if (range.end < range.start) {
      std::swap(range.start, range.end);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.5f, 0.5f, 0.5f, 0.3f);

    for (int y = range.start.y; y <= range.end.y; ++y) {
      int start_x = (y == range.start.y) ? range.start.x : 0;
      int end_x = (y == range.end.y) ? range.end.x : model.line_at(y)->size();

      for (int x = start_x; x <= end_x; ++x) {
        float x_width = x * engine->char_width;
        float y_width = y * engine->char_height;

        glBegin(GL_QUADS);
        glVertex2f(x_width, y_width);
        glVertex2f(x_width + engine->char_width, y_width);
        glVertex2f(x_width + engine->char_width, y_width + engine->char_height);
        glVertex2f(x_width, y_width + engine->char_height);
        glEnd();
      }
    }
  }

  if (mode == Mode::NORMAL || mode == Mode::VISUAL) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.0f, 1.0f, 1.0f, 0.75f);

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

      // Movement that's allowed in insert mode
      if (key == SDLK_LEFT) model.move_cursor_left();
      if (key == SDLK_RIGHT) model.move_cursor_right();
      if (key == SDLK_UP) model.move_cursor_up();
      if (key == SDLK_DOWN) model.move_cursor_down();

      if (mode == Mode::INSERT) {
        if (key == SDLK_ESCAPE) {
          mode = Mode::NORMAL;
          model.update_cursor_range_modifier(0);
          if (cursor_location.x == -1) {
            model.move_cursor_right();
          }
          return;
        }

        if (key == SDLK_BACKSPACE) {
          if (cursor_location.x > 0) {
            model.delete_before();
            model.move_cursor_left();
          } else if (cursor_location.y > 0) {
            model.delete_newline_before_current_location();
          }
          return;
        }

        if (key == SDLK_DELETE) {
          if (cursor_location.x < model.get_current_range().end.x) {
          }
          model.delete_after();
          return;
        }

        return;
      }

      // Movement that's not allowed in insert mode
      if (key == SDLK_h) model.move_cursor_left();
      if (key == SDLK_l) model.move_cursor_right();
      if (key == SDLK_k) model.move_cursor_up();
      if (key == SDLK_j) model.move_cursor_down();

      if (mode == Mode::VISUAL) {
        if (key == SDLK_ESCAPE) {
          mode = Mode::NORMAL;
          model.update_cursor_range_modifier(0);
          return;
        }
        LocationRange range = model.get_selection_range();
        if (key == SDLK_d) {
          mode = Mode::NORMAL;
          model.update_cursor_range_modifier(0);
          range.end.x += 1;
          model.delete_range(range);
          return;
        }
        range.end = cursor_location;
        model.set_selection_range(range);
      }

      if (key == SDLK_ESCAPE) {
        engine->engine_running = false;
        return;
      }

      if (key == SDLK_i) {
        mode = Mode::INSERT;
        model.update_cursor_range_modifier(-1);
        model.move_cursor_left();
        return;
      }

      if (key == SDLK_a) {
        mode = Mode::INSERT;
        model.update_cursor_range_modifier(-1);
        return;
      }

      if (key == SDLK_v) {
        mode = Mode::VISUAL;
        model.update_cursor_range_modifier(1);
        LocationRange range(cursor_location, cursor_location);
        model.set_selection_range(range);
        return;
      }

      if (key == SDLK_x) {
        if (e.key.keysym.mod & KMOD_SHIFT) {
          model.move_cursor_left();
          model.delete_after();
        } else {
          model.delete_before();
        }
        return;
      }

      break;
    }

    case SDL_KEYUP: {
      keys_pressed.erase(key);
      break;
    }
  }
}
