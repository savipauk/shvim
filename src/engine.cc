#include "engine.h"

#include <iostream>
#include <string>

#include "common.h"
#include "component.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "text_editor.h"

void Engine::add(Component* c) {
  components.push_back(c);
}

void Engine::run() {
  SDL_Event e;

  int minx, maxx, miny, maxy, advance;
  if (TTF_GlyphMetrics(font, 'A', &minx, &maxx, &miny, &maxy, &advance) == 0) {
    char_width = advance;
  }
  char_height = TTF_FontHeight(font);
  std::string initial_text = "mjau mjau\nwuf wuf\nmu mu\n";
  LocationRange cursor_range(
      Location{0, 0},
      Location{WINDOW_WIDTH / char_width - 1, WINDOW_HEIGHT / char_height - 1});
  TextEditor text_editor(this, initial_text, cursor_range);
  add(&text_editor);

  while (engine_running) {
    while (SDL_PollEvent(&e)) {
      ImGui_ImplSDL2_ProcessEvent(&e);

      if (e.type == SDL_QUIT) {
        engine_running = false;
      }

      if (e.type == SDL_TEXTINPUT) {
        continue;
      }

      for (auto c : components) {
        c->on_key_event(e);
      }
    }

    ImGuiIO& io = ImGui::GetIO();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Make (0,0) top left
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (auto c : components) {
      c->on_draw();
    }

    // Error checking
    GLenum err = glGetError();
    switch (err) {
      case GL_NO_ERROR:
        break;

      case GL_INVALID_OPERATION:
      case GL_OUT_OF_MEMORY:
        std::cout << "OpenGL Error: " << std::to_string(err) << "\n";
        std::cout << "Critical OpenGL Error. Stopping rendering.\n";
        engine_running = false;

      default:
        std::cout << "OpenGL Error: " << std::to_string(err) << "\n";
        engine_running = false;
    }

    // Render ImGui
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap the window
    SDL_GL_SwapWindow(window);

    // Frame limiting
    SDL_Delay(1000 * (1.0 / TARGET_FPS));

    // Error checking
    err = glGetError();
    if (err != GL_NO_ERROR) {
      std::cout << "OpenGL Error: " << std::to_string(err) << "\n";
    }
  }
}

void Engine::render_text(const std::string& message, int x, int y,
                         SDL_Color color) {
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Cannot render text: font not loaded.");
    return;
  }

  SDL_Surface* old_surface =
      TTF_RenderText_Blended(font, message.c_str(), color);
  if (!old_surface) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to create text surface: %s", TTF_GetError());
    return;
  }

  SDL_Surface* text_surface =
      SDL_ConvertSurfaceFormat(old_surface, SDL_PIXELFORMAT_RGBA32, 0);
  if (!text_surface) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Failed to convert text surface: %s", TTF_GetError());
    return;
  }
  SDL_FreeSurface(old_surface);

  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  // Setup filtering parameters for display
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Set pixel unpack alignment
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  GLenum text_format = GL_RGBA;
  // Upload pixel data to texture
  glTexImage2D(GL_TEXTURE_2D, 0, text_format, text_surface->w, text_surface->h,
               0, text_format, GL_UNSIGNED_BYTE, text_surface->pixels);

  // Render the textured quad
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);  // Enable blending for transparency
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // Use white to not tint the texture

  float draw_x = static_cast<float>(x);
  float draw_y = static_cast<float>(y);
  float tex_Width = static_cast<float>(text_surface->w);
  float tex_Height = static_cast<float>(text_surface->h);

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(draw_x, draw_y);  // Top-left
  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(draw_x + tex_Width, draw_y);  // Top-right
  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(draw_x + tex_Width, draw_y + tex_Height);  // Bottom-right
  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(draw_x, draw_y + tex_Height);  // Bottom-left
  glEnd();

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

  SDL_FreeSurface(text_surface);
  glDeleteTextures(1, &texture_id);
}
