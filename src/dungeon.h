#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "dungeon_surface.h"
#include "dungeon_surface_renderer.h"
#include "dungeon_params.h"

static bool tile_has_left_neighbor(
    const std::vector<std::vector<uint16_t>>& layout,
    uint32_t tileX,
    uint32_t tileY) {
  if (tileX <= 0)
    return false;
  return layout[tileX - 1][tileY] == 1;
}

static bool tile_has_right_neighbor(
    const std::vector<std::vector<uint16_t>>& layout,
    uint32_t tileX,
    uint32_t tileY) {
  if (tileX >= layout.size() - 1)
    return false;
  return layout[tileX + 1][tileY] == 1;
}

static bool tile_has_front_neighbor(
    const std::vector<std::vector<uint16_t>>& layout,
    uint32_t tileX,
    uint32_t tileY) {
  if (tileY <= 0)
    return false;
  return layout[tileX][tileY - 1] == 1;
}

static bool tile_has_back_neighbor(
    const std::vector<std::vector<uint16_t>>& layout,
    uint32_t tileX,
    uint32_t tileY) {
  if (tileY >= layout[tileX].size() - 1)
    return false;
  return layout[tileX][tileY + 1] == 1;
}

static DungeonSurface create_left_surface(uint32_t x, uint32_t z) {
  DungeonSurface surf;
  glm::vec3 pos = glm::vec3(x * DUNGEON_TILE_WIDTH - DUNGEON_TILE_WIDTH_OFFSET,
                            0.0f, z * DUNGEON_TILE_LENGTH);
  glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
  surf.model = trans * DUNGEON_LEFT_ROT_MAT * DUNGEON_WALL_SCALE_MAT;
  surf.color = DUNGEON_LEFT_COLOR;
  return surf;
}

static DungeonSurface create_right_surface(uint32_t x, uint32_t z) {
  DungeonSurface surf;
  glm::vec3 pos = glm::vec3(x * DUNGEON_TILE_WIDTH + DUNGEON_TILE_WIDTH_OFFSET,
                            0.0f, z * DUNGEON_TILE_LENGTH);
  glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
  surf.model = trans * DUNGEON_RIGHT_ROT_MAT * DUNGEON_WALL_SCALE_MAT;
  surf.color = DUNGEON_RIGHT_COLOR;
  return surf;
}

static DungeonSurface create_front_surface(uint32_t x, uint32_t z) {
  DungeonSurface surf;
  glm::vec3 pos =
      glm::vec3(x * DUNGEON_TILE_WIDTH, 0.0f,
                z * DUNGEON_TILE_LENGTH - DUNGEON_TILE_LENGTH_OFFSET);
  glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
  surf.model = trans * DUNGEON_FRONT_ROT_MAT * DUNGEON_WALL_SCALE_MAT;
  surf.color = DUNGEON_FRONT_COLOR;
  return surf;
}

static DungeonSurface create_back_surface(uint32_t x, uint32_t z) {
  DungeonSurface surf;
  glm::vec3 pos =
      glm::vec3(x * DUNGEON_TILE_WIDTH, 0.0f,
                z * DUNGEON_TILE_LENGTH + DUNGEON_TILE_LENGTH_OFFSET);
  glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
  surf.model = trans * DUNGEON_BACK_ROT_MAT * DUNGEON_WALL_SCALE_MAT;
  surf.color = DUNGEON_BACK_COLOR;
  return surf;
}

static DungeonSurface create_top_surface(uint32_t x, uint32_t z) {
  DungeonSurface surf;
  glm::vec3 pos = glm::vec3(x * DUNGEON_TILE_WIDTH, DUNGEON_TILE_HEIGHT_OFFSET,
                            z * DUNGEON_TILE_LENGTH);
  glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
  surf.model = trans * DUNGEON_TOP_ROT_MAT * DUNGEON_FLOOR_SCALE_MAT;
  surf.color = DUNGEON_TOP_COLOR;
  return surf;
}

static DungeonSurface create_bottom_surface(uint32_t x, uint32_t z) {
  DungeonSurface surf;
  glm::vec3 pos = glm::vec3(x * DUNGEON_TILE_WIDTH, -DUNGEON_TILE_HEIGHT_OFFSET,
                            z * DUNGEON_TILE_LENGTH);
  glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
  surf.model = trans * DUNGEON_BOTTOM_ROT_MAT * DUNGEON_FLOOR_SCALE_MAT;
  surf.color = DUNGEON_BOTTOM_COLOR;
  return surf;
}

class Dungeon {
 public:
  Dungeon() { renderer = new DungeonSurfaceRenderer(); }

  void create(const std::vector<std::vector<uint16_t>> _layout) {
    renderer->init();
    dungeonWidth = (uint32_t)_layout.size();
    dungeonLength = (uint32_t)_layout[0].size();

    for (uint32_t x = 0; x < dungeonWidth; x++) {
      for (uint32_t z = 0; z < dungeonLength; z++) {
        if (_layout[x][z] == 0)
          continue;
        else {
          if (!tile_has_left_neighbor(_layout, x, z)) {
            left_surfaces.push_back(create_left_surface(x, z));
          }
          if (!tile_has_right_neighbor(_layout, x, z)) {
            right_surfaces.push_back(create_right_surface(x, z));
          }
          if (!tile_has_front_neighbor(_layout, x, z)) {
            front_surfaces.push_back(create_front_surface(x, z));
          }
          if (!tile_has_back_neighbor(_layout, x, z)) {
            back_surfaces.push_back(create_back_surface(x, z));
          }
          top_surfaces.push_back(create_top_surface(x, z));
          bottom_surfaces.push_back(create_bottom_surface(x, z));
        }
      }
    }
  }
  void render(const glm::mat4 viewproj) {
    renderer->begin_render(viewproj);

    renderer->apply_textures(SurfaceType_Left);
    for (auto& surf : left_surfaces) {
      renderer->render_surface(surf);
    }

    renderer->apply_textures(SurfaceType_Right);
    for (auto& surf : right_surfaces) {
      renderer->render_surface(surf);
    }

    renderer->apply_textures(SurfaceType_Front);
    for (auto& surf : front_surfaces) {
      renderer->render_surface(surf);
    }

    renderer->apply_textures(SurfaceType_Back);
    for (auto& surf : back_surfaces) {
      renderer->render_surface(surf);
    }

    renderer->apply_textures(SurfaceType_Top);
    for (auto& surf : top_surfaces) {
      renderer->render_surface(surf);
    }

    renderer->apply_textures(SurfaceType_Bottom);
    for (auto& surf : bottom_surfaces) {
      renderer->render_surface(surf);
    }
  }

 private:
  uint32_t dungeonWidth, dungeonLength;
  std::vector<DungeonSurface> left_surfaces;
  std::vector<DungeonSurface> right_surfaces;
  std::vector<DungeonSurface> front_surfaces;
  std::vector<DungeonSurface> back_surfaces;
  std::vector<DungeonSurface> bottom_surfaces;
  std::vector<DungeonSurface> top_surfaces;
  DungeonSurfaceRenderer* renderer;
};
