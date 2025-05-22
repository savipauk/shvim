#include "keyboard_master.h"

#include <SDL_opengl.h>
#include <SDL_pixels.h>

#include <algorithm>
#include <iostream>

#include "common.h"

#define CHAR_WIDTH 20
#define CHAR_HEIGHT 40
#define MAX_CHARS 800 / 20
#define MAX_ROWS 600 / 40

void KeyboardMaster::on_draw() {
  glColor3f(1.0f, 0.0f, 0.0f);

  glBegin(GL_LINES);

  glVertex2f(0, CHAR_HEIGHT * (cursor_position.y + 1));
  glVertex2f(WINDOW_WIDTH, CHAR_HEIGHT * (cursor_position.y + 1));

  glEnd();

  SDL_Color color{(Uint8)255, (Uint8)255, (Uint8)255, (Uint8)255};
  std::string text = "Hello SDL_ttf!";
  int text_x = 30;
  int text_y = 10;

  text_y = 50;

  engine->render_text(text, text_x, text_y, color);
  engine->render_text("Testing text rendering...", 30, 80, {200, 200, 50, 255});

  if (mode == Mode::INSERT) {
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(CHAR_WIDTH * (cursor_position.x + 1),
               CHAR_HEIGHT * cursor_position.y);
    glVertex2f(CHAR_WIDTH * (cursor_position.x + 1),
               CHAR_HEIGHT * (cursor_position.y + 1));
    glEnd();
  } else if (mode == Mode::NORMAL) {
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);

    glBegin(GL_POLYGON);

    glVertex2f(cursor_position.x * CHAR_WIDTH, cursor_position.y * CHAR_HEIGHT);
    glVertex2f(cursor_position.x * CHAR_WIDTH + CHAR_WIDTH,
               cursor_position.y * CHAR_HEIGHT);
    glVertex2f(cursor_position.x * CHAR_WIDTH + CHAR_WIDTH,
               cursor_position.y * CHAR_HEIGHT + CHAR_HEIGHT);
    glVertex2f(cursor_position.x * CHAR_WIDTH,
               cursor_position.y * CHAR_HEIGHT + CHAR_HEIGHT);

    glEnd();
  }

  std::cout << cursor_position.x << " " << cursor_position.y << "\n";
}

void KeyboardMaster::on_key_event(const SDL_Event& e) {
  move = {0, 0};

  switch (e.type) {
    case SDL_KEYDOWN: {
      keys_pressed.insert(e.key.keysym.sym);
      break;
    }
    case SDL_KEYUP: {
      keys_pressed.erase(e.key.keysym.sym);
      break;
    }
  }

  if (mode == Mode::INSERT) {
    if (keys_pressed.count(SDLK_ESCAPE)) {
      mode = Mode::NORMAL;
    }
    return;
  }

  if (keys_pressed.count(SDLK_RETURN)) {
    engine->engine_running = false;
  }

  if (keys_pressed.count(SDLK_i)) {
    mode = Mode::INSERT;
  }

  move.x -= keys_pressed.count(SDLK_h) || keys_pressed.count(SDLK_LEFT);
  move.x += keys_pressed.count(SDLK_l) || keys_pressed.count(SDLK_RIGHT);
  move.y -= keys_pressed.count(SDLK_k) || keys_pressed.count(SDLK_UP);
  move.y += keys_pressed.count(SDLK_j) || keys_pressed.count(SDLK_DOWN);

  cursor_position.x += move.x;
  cursor_position.x = std::clamp(cursor_position.x, 0, MAX_CHARS - 1);

  cursor_position.y += move.y;
  cursor_position.y = std::clamp(cursor_position.y, 0, MAX_ROWS - 1);
}
