#pragma once

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define TARGET_FPS 60

struct Vec2 {
  int x;
  int y;
};

class Location {
 public:
  int x;
  int y;

  Location(int line = 0, int column = 0) : x(line), y(column) {}

  bool operator==(const Location& other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Location& other) const {
    return !(*this == other);
  }
};

class LocationRange {
 public:
  Location start;
  Location end;

  LocationRange(const Location& start, const Location& end)
      : start(start), end(end) {}
};

class CursorObserver {
 public:
  virtual ~CursorObserver() = default;
  virtual void update_cursor_location(const Location& loc) = 0;
};

class TextObserver {
 public:
  virtual ~TextObserver() = default;
  virtual void update_text() = 0;
};
