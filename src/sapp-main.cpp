#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_time.h"

#include "camera.h"
#include "dungeon.h"
#include "dungeon_generator.h"

const int ScreenWidth = 1920;
const int ScreenHeight = 1080;
const float KeyCooldownTime = 0.15f;

typedef struct {
  int screenWidth;
  int screenHeight;
  sg_pass_action main_pass_action;
  Camera* camera;
  Dungeon* dungeon;
  bool firstMouse;
  bool showMouse;
  bool mouseJustCaptured;
  bool showWireframe;
  float lastX, lastY;
  float deltaTime, lastPress, keyCooldown;
  uint64_t lastFrameTimestamp;
} app_state_t;

static app_state_t app_state;

/*
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  int winWidth, winHeight;
  if (app_state.firstMouse) {
    glfwGetWindowSize(window, &winWidth, &winHeight);
    app_state.lastX = winWidth / 2.0f;
    app_state.lastY = winHeight / 2.0f;
    glfwSetCursorPos(window, app_state.lastX, app_state.lastY);
    app_state.firstMouse = false;
  }

  float xoffset = (float)xpos - app_state.lastX;
  float yoffset = app_state.lastY - (float)ypos;

  if (app_state.mouseJustCaptured && !app_state.showMouse) {
    glfwGetWindowSize(window, &winWidth, &winHeight);
    glfwSetCursorPos(window, winWidth / 2.0f, winHeight / 2.0f);
    xoffset = 0.0f;
    yoffset = 0.0f;
    app_state.mouseJustCaptured = false;
  }

  if (!app_state.showMouse) {
    app_state.camera->ProcessMouseMovement(xoffset, yoffset);
    glfwSetCursorPos(window, app_state.lastX, app_state.lastY);
  }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  app_state.camera->ProcessMouseScroll((float)yoffset);
}
*/

void processInput() {
  app_state.lastPress += app_state.deltaTime;
  /*
  if (glfwGetKey(state->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(state->window, GLFW_TRUE);
  }

  if (glfwGetKey(state->window, GLFW_KEY_W) == GLFW_PRESS) {
    state->camera->ProcessKeyboard(FORWARD, state->deltaTime);
  }
  if (glfwGetKey(state->window, GLFW_KEY_S) == GLFW_PRESS) {
    state->camera->ProcessKeyboard(BACKWARD, state->deltaTime);
  }
  if (glfwGetKey(state->window, GLFW_KEY_A) == GLFW_PRESS) {
    state->camera->ProcessKeyboard(LEFT, state->deltaTime);
  }
  if (glfwGetKey(state->window, GLFW_KEY_D) == GLFW_PRESS) {
    state->camera->ProcessKeyboard(RIGHT, state->deltaTime);
  }
  if (glfwGetKey(state->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    state->camera->ProcessKeyboard(UP, state->deltaTime);
  }
  if (glfwGetKey(state->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    state->camera->ProcessKeyboard(DOWN, state->deltaTime);
  }
  if (glfwGetKey(state->window, GLFW_KEY_G) == GLFW_PRESS &&
      state->lastPress >= state->keyCooldown) {
    state->showWireframe = !state->showWireframe;
    state->lastPress = 0.0f;
  }
  if (glfwGetKey(state->window, GLFW_KEY_F) == GLFW_PRESS &&
      state->lastPress >= state->keyCooldown) {
    state->showMouse = !state->showMouse;
    if (!state->showMouse) {
      state->mouseJustCaptured = true;
    }
    state->lastPress = 0.0f;
  }
  */
}

void event(const sapp_event* e) {}

void updateFrameTime() {
  uint64_t currentFrameTime = stm_now();
  app_state.deltaTime = (float)(stm_sec(
      stm_diff(currentFrameTime, app_state.lastFrameTimestamp)));
  app_state.lastFrameTimestamp = currentFrameTime;
}

void init() {
  app_state.screenWidth = ScreenWidth;
  app_state.screenHeight = ScreenHeight;

  sg_desc d = {0};
  d.context = sapp_sgcontext();
  // d.context.sample_count = 4;
  sg_setup(&d);
  assert(sg_isvalid());
  stm_setup();
  initTextureLoader();

  sg_pass_action pass = {0};
  pass.colors[0].action = SG_ACTION_CLEAR;
  pass.colors[0].value = sg_color{0.05f, 0.05f, 0.05f, 1.0f};
  app_state.main_pass_action = pass;

  app_state.camera = new Camera(glm::vec3(0.0f, 0.5f, 0.0f));
  app_state.dungeon = new Dungeon();
  app_state.dungeon->create(generate_new_dungeon(25, 25));

  app_state.keyCooldown = KeyCooldownTime;
  app_state.lastPress = 0.0f;
  app_state.firstMouse = true;
  app_state.lastX = 0;
  app_state.lastY = 0;
  app_state.mouseJustCaptured = false;
  app_state.showMouse = true;
  app_state.showWireframe = false;
  app_state.deltaTime = 0.0f;
  app_state.lastFrameTimestamp = stm_now();
}

void update() {
  updateFrameTime();
  processInput();
  texturePump();
}

void render() {
  int currWidth = sapp_width();
  int currHeight = sapp_height();
  if (currWidth != app_state.screenWidth ||
      currHeight != app_state.screenHeight) {
    app_state.screenWidth = currWidth;
    app_state.screenHeight = currHeight;
  }

  float aspectRatio =
      static_cast<float>(currWidth) / static_cast<float>(currHeight);
  glm::mat4 projection = glm::perspective(glm::radians(app_state.camera->Zoom),
                                          aspectRatio, 0.1f, 100.0f);
  glm::mat4 viewproj = projection * app_state.camera->GetViewMatrix();

  sg_begin_default_pass(&app_state.main_pass_action, currWidth, currHeight);
  app_state.dungeon->render(viewproj);
  sg_end_pass();
  sg_commit();
  // glfwSwapBuffers(state->window);
  // glfwPollEvents();
}

void cleanup() {
  delete app_state.camera;
  delete app_state.dungeon;
  destroyTextureLoader();
  sg_shutdown();
}

void frame() {
  update();
  render();
}

sapp_desc sokol_main(int argc, char* argv[]) {
  sapp_desc desc = {0};
  desc.init_cb = init;
  desc.frame_cb = frame;
  desc.cleanup_cb = cleanup;
  desc.event_cb = event;
  desc.width = ScreenWidth;
  desc.height = ScreenHeight;
  desc.gl_force_gles2 = false;
  desc.sample_count = 4;
  desc.window_title = "DungeonCrawl";
  desc.icon.sokol_default = true;
  return desc;
}