#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// TODO
#include <functional>
#include <iostream>

enum disk_color { DISK_LIGHT, DISK_DARK };

class disk_state {
private:
  std::vector<disk_color> _colors;

public:
  disk_state(size_t light_count)
    : _colors(light_count * 2, DISK_LIGHT) {

      assert(light_count > 0);

      for (size_t i = 0; i < _colors.size(); i += 2) {
        _colors[i] = DISK_DARK;
      }
  }

  bool operator== (const disk_state& rhs) const {
    return std::equal(_colors.begin(), _colors.end(), rhs._colors.begin());
  }

  size_t total_count() const {
    return _colors.size();
  }

  size_t light_count() const {
    return total_count() / 2;
  }

  size_t dark_count() const {
    return light_count();
  }

  bool is_index(size_t i) const {
    return (i < total_count());
  }

  disk_color get(size_t index) const {
    assert(is_index(index));
    return _colors[index];
  }

  void swap(size_t left_index) {
    assert(is_index(left_index));
    auto right_index = left_index + 1;
    assert(is_index(right_index));
    std::swap(_colors[left_index], _colors[right_index]);
  }

  std::string to_string() const {
    std::stringstream ss;
    bool first = true;
    for (auto color : _colors) {
      if (!first) {
        ss << " ";
      }

      if (color == DISK_LIGHT) {
        ss << "L";
      } else {
        ss << "D";
      }

      first = false;
    }
    return ss.str();
  }

  // Return true when this disk_state is in alternating format. That means
  // that the first disk at index 0 is dark, the second disk at index 1
  // is light, and so on for the entire row of disks.
  bool is_initialized() const {
    disk_color check = DISK_DARK;
    for(unsigned long i = 0; i < _colors.size(); i++)
    {
      switch(_colors[i])
      {
        case DISK_DARK:
          if(_colors[i] == check)
          {
            check = DISK_LIGHT;
            continue;
          }
          return false;
        case DISK_LIGHT:
          if(_colors[i] == check)
          {
            check = DISK_DARK;
            continue;
          }
          return false;
        default:
          std::cerr << "Unknown disk color\n";
      }
    }
    return true;
  }

  // Return true when this disk_state is fully sorted, with all dark disks on 
  // the left (low indices) and all dark disks on the right (high indices).
  bool is_sorted() const {
    unsigned long half = _colors.size() / 2;
    for(unsigned long i = 0; i < half; i++)
    {
      if(_colors[i] == DISK_DARK) return false;
    }

    for(unsigned long i = half; i < _colors.size(); i++)
    {
      if(_colors[i] == DISK_LIGHT) return false;
    }

    return true;  
  }
};

// Data structure for the output of the alternating disks problem. That
// includes both the final disk_state, as well as a count of the number
// of swaps performed.
class sorted_disks {
private:
  disk_state _after;
  unsigned _swap_count;

public:

  sorted_disks(const disk_state& after, unsigned swap_count)
    : _after(after), _swap_count(swap_count) { }

  sorted_disks(disk_state&& after, unsigned swap_count)
    : _after(after), _swap_count(swap_count) { }

  const disk_state& after() const {
    return _after;
  }

  unsigned swap_count() const {
    return _swap_count;
  }
};

// Algorithm that sorts disks using the lawnmower algorithm.
sorted_disks sort_lawnmower(const disk_state& before) {
  disk_state after = before;

  const disk_color black = DISK_DARK;
  const disk_color white = DISK_LIGHT;
  int swap_count = 0;
  int size = after.total_count();
  
  for(int i = 0; i < size / 2; i++)
  {
    // going right
    for(int j = 0; j < size - 1; j++)
    {
      // if curr is black and the next is not
      if(after.get(j) == black && after.get(j) != after.get(j + 1))
      {
        after.swap(j);
        swap_count++;
      }
    }

    // going left
    for(unsigned long j = size - 1; j > 0; j--)
    {
      // if curr is black and the next is not
      if(after.get(j) == white && after.get(j) != after.get(j - 1))
      {
        after.swap(j - 1);
        swap_count++;
      }
    }
  }

  return sorted_disks(after, swap_count);
}


// Algorithm that sorts disks using the alternate algorithm.
sorted_disks sort_alternate(const disk_state& before) {
  disk_state after = before;
  int swap_count = 0;
  const disk_color black = DISK_DARK;
  const disk_color white = DISK_LIGHT;
  const size_t size = after.total_count();

  // n + 1 runs
  for(size_t i = 0; i < after.dark_count() + 1; i++)
  {
    // for each pair right of the last run start
    for(size_t j = i; j < size - 1; j += 2)
    {
      if (after.get(j) == black && after.get(j + 1) == white)
      {
        after.swap(j);
        swap_count += 1;
      }
    }
  }

  return sorted_disks(after, swap_count);
}