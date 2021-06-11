#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum SurfaceType {
  SurfaceType_Left = 0,
  SurfaceType_Right,
  SurfaceType_Front,
  SurfaceType_Back,
  SurfaceType_Top,
  SurfaceType_Bottom,
  SurfaceType_Count
};

const float DUNGEON_TILE_WIDTH = 2.0f;
const float DUNGEON_TILE_WIDTH_OFFSET = DUNGEON_TILE_WIDTH / 2.0f;
const float DUNGEON_TILE_LENGTH = 2.0f;
const float DUNGEON_TILE_LENGTH_OFFSET = DUNGEON_TILE_LENGTH / 2.0f;
const float DUNGEON_TILE_HEIGHT = 2.5f;
const float DUNGEON_TILE_HEIGHT_OFFSET = DUNGEON_TILE_HEIGHT / 2.0f;

const glm::mat4 DUNGEON_WALL_SCALE_MAT =
    glm::scale(glm::mat4(1.0f),
               glm::vec3(DUNGEON_TILE_WIDTH, DUNGEON_TILE_HEIGHT, 0.0f));
const glm::mat4 DUNGEON_FLOOR_SCALE_MAT =
    glm::scale(glm::mat4(1.0f),
               glm::vec3(DUNGEON_TILE_WIDTH, DUNGEON_TILE_LENGTH, 0.0f));

const glm::mat4 DUNGEON_LEFT_ROT_MAT = glm::rotate(glm::mat4(1.0f),
                                                   glm::radians(90.0f),
                                                   glm::vec3(0.0f, 1.0f, 0.0f));
const glm::mat4 DUNGEON_RIGHT_ROT_MAT =
    glm::rotate(glm::mat4(1.0f),
                glm::radians(-90.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));
const glm::mat4 DUNGEON_FRONT_ROT_MAT =
    glm::rotate(glm::mat4(1.0f),
                glm::radians(0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));
const glm::mat4 DUNGEON_BACK_ROT_MAT = glm::rotate(glm::mat4(1.0f),
                                                   glm::radians(180.0f),
                                                   glm::vec3(0.0f, 1.0f, 0.0f));
const glm::mat4 DUNGEON_TOP_ROT_MAT = glm::rotate(glm::mat4(1.0f),
                                                  glm::radians(90.0f),
                                                  glm::vec3(1.0f, 0.0f, 0.0f));
const glm::mat4 DUNGEON_BOTTOM_ROT_MAT =
    glm::rotate(glm::mat4(1.0f),
                glm::radians(-90.0f),
                glm::vec3(1.0f, 0.0f, 0.0f));

const glm::vec3 DUNGEON_BOTTOM_COLOR = glm::vec3(0.0f, 0.0f, 0.0f);  // red
const glm::vec3 DUNGEON_TOP_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);     // green
const glm::vec3 DUNGEON_LEFT_COLOR =
    glm::vec3(155.0f / 255.0f, 66.0f / 255.0f, 42.0f / 255.0f);  // blue
const glm::vec3 DUNGEON_RIGHT_COLOR =
    glm::vec3(66.0f / 255.0f, 155.0f / 255.0f, 42.0f / 255.0f);  // yellow
const glm::vec3 DUNGEON_FRONT_COLOR =
    glm::vec3(66.0f / 255.0f, 42.0f / 255.0f, 155.0f / 255.0f);  // purple
const glm::vec3 DUNGEON_BACK_COLOR =
    glm::vec3(155.0f / 255.0f, 42.0f / 255.0f, 66.0f / 255.0f);  // cyan?

const char* wall_image_paths[SurfaceType_Count] = {
    "bricks2.jpg",   "bricks2.jpg",         "brickwall.jpg",
    "brickwall.jpg", "toy_box_diffuse.png", "wood.png",
};

// const char* wall_bump_image_