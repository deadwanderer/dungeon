#pragma once

#include <vector>

std::vector<std::vector<uint16_t>> generate_new_dungeon(uint32_t width,
                                                        uint32_t length) {
  std::vector<std::vector<uint16_t>> result;
  result.resize(width);

  uint32_t half_width = width / 2;
  uint32_t half_length = length / 2;
  for (uint32_t x = 0; x < width; x++) {
    result[x].resize(length);
    for (uint32_t y = 0; y < length; y++) {
      if (x < 4 && y < 4) {
        result[x][y] = 1;
      } else if (x % 4 == 0 || y % 3 == 0 || x % 3 == 0) {
        result[x][y] = 1;
      } else {
        result[x][y] = 0;
      }
      // result[x][y] = 1;
    }
  }

  return result;
}