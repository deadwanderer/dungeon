#include "glad.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "sokol_gfx.h"
// #pragma warning(push)
// #pragma warning(disable : 4005)
#include "sokol_time.h"

#ifdef APIENTRY
  #undef APIENTRY
#endif
#define GLFW_INCLUDE_NONE
#include "glfw/glfw3.h"
// #pragma warning(pop)

#include "camera.h"
#include "dungeon.h"
#include "dungeon_generator.h"

const int ScreenWidth = 1920;
const int ScreenHeight = 1080;
const float KeyCooldownTime = 0.15f;

typedef struct {
  int screenWidth;
  int screenHeight;
  GLFWwindow* window;
  sg_pass_action main_pass_action;
  Camera* camera;
  Dungeon* dungeon;
  bool firstMouse;
  bool showMouse;
  bool mouseJustCaptured;
  bool showWireframe;
  float lastX, lastY;
  float deltaTime, lastPress, keyCooldown;
  double lastFrameTimestamp;
} app_state_t;

static app_state_t app_state;

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

void processInput(app_state_t* state) {
  state->lastPress += state->deltaTime;
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
}

void updateFrameTime(app_state_t* state) {
  double currentFrameTime = glfwGetTime();
  state->deltaTime = (float)(currentFrameTime - state->lastFrameTimestamp);
  state->lastFrameTimestamp = currentFrameTime;
}

void init(app_state_t* appState) {
  glfwInit();
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  appState->window =
      glfwCreateWindow(ScreenWidth, ScreenHeight, "Dungeon", 0, 0);
  appState->screenWidth = ScreenWidth;
  appState->screenHeight = ScreenHeight;
  glfwSetWindowPos(appState->window, 250, 150);
  glfwMakeContextCurrent(appState->window);

  glfwSetCursorPosCallback(appState->window, mouse_callback);
  glfwSetScrollCallback(appState->window, scroll_callback);
  glfwSwapInterval(1);
  // gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  sg_desc d = {0};
  d.context.sample_count = 4;
  sg_setup(&d);
  assert(sg_isvalid());
  stm_setup();

  sg_pass_action pass = {0};
  pass.colors[0].action = SG_ACTION_CLEAR;
  pass.colors[0].value = sg_color{0.05f, 0.05f, 0.05f, 1.0f};
  appState->main_pass_action = pass;

  appState->camera = new Camera(glm::vec3(2.5f, 1.5f, 3.5f));
  appState->dungeon = new Dungeon();
  appState->dungeon->create(generate_new_dungeon(5, 5));

  appState->keyCooldown = KeyCooldownTime;
  appState->lastPress = 0.0f;
  appState->firstMouse = true;
  appState->lastX = 0;
  appState->lastY = 0;
  appState->mouseJustCaptured = false;
  appState->showMouse = true;
  appState->showWireframe = false;
  appState->deltaTime = 0.0f;
  appState->lastFrameTimestamp = glfwGetTime();
}

void update(app_state_t* state) {
  updateFrameTime(state);
  processInput(state);
}

void render(app_state_t* state) {
  int currWidth, currHeight;
  glfwGetFramebufferSize(state->window, &currWidth, &currHeight);
  if (currWidth != state->screenWidth || currHeight != state->screenHeight) {
    state->screenWidth = currWidth;
    state->screenHeight = currHeight;
  }

  float aspectRatio =
      static_cast<float>(currWidth) / static_cast<float>(currHeight);
  glm::mat4 projection = glm::perspective(glm::radians(state->camera->Zoom),
                                          aspectRatio, 0.1f, 100.0f);
  glm::mat4 viewproj = projection * state->camera->GetViewMatrix();

  sg_begin_default_pass(&state->main_pass_action, currWidth, currHeight);
  state->dungeon->render(viewproj);
  sg_end_pass();
  sg_commit();
  glfwSwapBuffers(state->window);
  glfwPollEvents();
}

void cleanup(app_state_t* state) {
  delete state->camera;
  delete state->dungeon;
  sg_shutdown();
  glfwDestroyWindow(state->window);
  glfwTerminate();
}

void main() {
  init(&app_state);
  while (!glfwWindowShouldClose(app_state.window)) {
    update(&app_state);
    render(&app_state);
  }

  cleanup(&app_state);
}