#include "keyboard_master.h"

#include <SDL_opengl.h>
#include <SDL_pixels.h>

#include "common.h"

void KeyboardMaster::on_draw() {
  glColor3f(1.0f, 0.0f, 0.0f);

  glBegin(GL_LINES);

  glVertex2f(0, 40);
  glVertex2f(WINDOW_WIDTH, 40);

  glEnd();

  glBegin(GL_LINES);

  glVertex2f(20, 0);
  glVertex2f(20, 40);

  glEnd();

  SDL_Color color{(Uint8)255, (Uint8)255, (Uint8)255, (Uint8)255};
  std::string text = "Hello SDL_ttf!";
  int text_x = 30;
  int text_y = 10;

  text_y = 50;

  engine->render_text(text, text_x, text_y, color);

  engine->render_text("Testing text rendering...", 30, 80, {200, 200, 50, 255});
}

void KeyboardMaster::on_key_down(const SDL_KeyboardEvent& key) {
  if (key.keysym.sym == SDLK_RETURN) {
    engine->engine_running = false;
  }
}
