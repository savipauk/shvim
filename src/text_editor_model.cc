#include "text_editor_model.h"

#include <algorithm>
#include <iostream>

#include "common.h"

using Iterator = TextEditorModel::Iterator;

void TextEditorModel::notify_cursor_observers() {
  for (auto* observer : cursor_observers) {
    observer->update_cursor_location(cursor_location);
  }
}

void TextEditorModel::notify_text_observers() {
  for (auto* observer : text_observers) {
    observer->update_text();
  }
}

void TextEditorModel::move_cursor_left() {
  if (cursor_location.x > cursor_range.start.x) {
    cursor_location.x--;
    notify_cursor_observers();
  }
}

void TextEditorModel::move_cursor_right() {
  if (cursor_location.x < cursor_range.end.x) {
    cursor_location.x++;
    cursor_range.end.x = (int)lines[cursor_location.y].size() - 1;
    notify_cursor_observers();
  }
}

void TextEditorModel::move_cursor_up() {
  if (cursor_location.y > cursor_range.start.y) {
    cursor_location.y--;
    cursor_range.end.x = (int)lines[cursor_location.y].size() - 1;
    cursor_location.x =
        std::min(cursor_location.x, (int)(lines[cursor_location.y].size()) - 1);
    if (cursor_location.x == -1) {
      cursor_location.x = cursor_range.start.x;
    }
    notify_cursor_observers();
  }
}

void TextEditorModel::move_cursor_down() {
  if (cursor_location.y < cursor_range.end.y) {
    cursor_location.y++;
    cursor_range.end.x = (int)lines[cursor_location.y].size() - 1;
    cursor_location.x =
        std::min(cursor_location.x, (int)(lines[cursor_location.y].size()) - 1);
    if (cursor_location.x == -1) {
      cursor_location.x = cursor_range.start.x;
    }
    notify_cursor_observers();
  }
}

const Location& TextEditorModel::get_cursor_location() const {
  return cursor_location;
}

std::pair<Iterator, Iterator> TextEditorModel::all_lines() const {
  return {lines.cbegin(), lines.cend()};
}

std::pair<Iterator, Iterator> TextEditorModel::lines_range(int index1,
                                                           int index2) const {
  return {lines.cbegin() + index1, lines.cbegin() + index2};
}

Iterator TextEditorModel::line_at(int index) const {
  return (lines.cbegin() + index);
}

void TextEditorModel::add_cursor_observer(CursorObserver* observer) {
  cursor_observers.push_back(observer);
}

void TextEditorModel::remove_cursor_observer(CursorObserver* observer) {
  cursor_observers.erase(
      std::remove(cursor_observers.begin(), cursor_observers.end(), observer),
      cursor_observers.end());
}

void TextEditorModel::delete_before() {
  if (cursor_location.x >= (int)lines[cursor_location.y].size() - 1) {
    lines[cursor_location.y].erase(cursor_location.x, 1);
    move_cursor_left();
  } else {
    lines[cursor_location.y].erase(cursor_location.x, 1);
  }

  cursor_range.end.x = (int)lines[cursor_location.y].size() - 1;
  notify_text_observers();
}

void TextEditorModel::delete_after() {
  lines[cursor_location.y].erase(cursor_location.x, 1);
  move_cursor_left();
  cursor_range.end.x = (int)lines[cursor_location.y].size() - 1;

  notify_text_observers();
}

void TextEditorModel::delete_range(LocationRange range) {
  if (range.start.y == range.end.y) {
    std::string& line = lines[range.start.y];
    line.erase(range.start.x, range.end.x - range.start.x);
    std::cout << "line size " << lines.size() << "\n";
    notify_text_observers();
    return;
  }

  if (range.end < range.start) {
    std::swap(range.start, range.end);
  }

  std::string start_line_head = lines[range.start.y].substr(0, range.start.x);
  std::string end_line_tail = lines[range.end.y].substr(range.end.x);

  lines[range.start.y] = start_line_head + end_line_tail;

  auto first_line_to_erase = lines.begin() + range.start.y + 1;
  auto last_line_to_erase = lines.begin() + range.end.y + 1;

  lines.erase(first_line_to_erase, last_line_to_erase);

  cursor_range.end.x = (int)lines[lines.size() - 1].size() - 1;
  cursor_range.end.y = (int)lines.size() - 1;
  notify_text_observers();

  cursor_location = range.start;
  notify_cursor_observers();
}

LocationRange TextEditorModel::get_selection_range() {
  return selection_range;
}

void TextEditorModel::set_selection_range(LocationRange range) {
  selection_range = range;
}

void TextEditorModel::add_text_observer(TextObserver* observer) {
  text_observers.push_back(observer);
}

void TextEditorModel::remove_text_observer(TextObserver* observer) {
  text_observers.erase(
      std::remove(text_observers.begin(), text_observers.end(), observer),
      text_observers.end());
}
