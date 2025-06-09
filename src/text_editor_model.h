#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "common.h"

class TextEditorModel {
 private:
  std::vector<std::string> lines;
  std::vector<CursorObserver*> cursor_observers;
  std::vector<TextObserver*> text_observers;
  Location cursor_location{0, 0};

  void notify_cursor_observers();
  void notify_text_observers();

 public:
  using Iterator = std::vector<std::string>::const_iterator;

  std::pair<Location, Location> selection_range;
  LocationRange cursor_range;

  TextEditorModel(const std::string& text, LocationRange cursor_range)
      : cursor_range(cursor_range) {
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
      lines.push_back(line);
    }

    if (!text.empty() && text.back() == '\n') {
      lines.push_back("");
    }

    this->cursor_range.end.x = (int)lines[0].size() - 1;
    this->cursor_range.end.y = (int)lines.size() - 1;
  }

  void move_cursor_left();
  void move_cursor_right();
  void move_cursor_up();
  void move_cursor_down();

  const Location& get_cursor_location() const;

  std::pair<Iterator, Iterator> all_lines() const;
  std::pair<Iterator, Iterator> lines_range(int index1, int index2) const;

  void add_cursor_observer(CursorObserver* observer);
  void remove_cursor_observer(CursorObserver* observer);

  void delete_before();
  void delete_after();
  void delete_range(LocationRange range);
  LocationRange get_selection_range();
  void set_selection_range(LocationRange range);

  void add_text_observer(TextObserver* observer);
  void remove_text_observer(TextObserver* observer);
};
