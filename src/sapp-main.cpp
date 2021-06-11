#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_time.h"

#include "dungeon_camera.h"
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
  bool freeLook;
  float lastX, lastY;
  float deltaTime, lastPress, keyCooldown;
  uint64_t lastFrameTimestamp;
} app_state_t;

static app_state_t app_state;

void event(const sapp_event* e) {
  switch (e->type) {
    case SAPP_EVENTTYPE_KEY_DOWN: {
      switch (e->key_code) {
        case SAPP_KEYCODE_W: {
          app_state.camera->ProcessKeyboardMovement(MOVE_FORWARD,
                                                    app_state.deltaTime);
        } break;
        case SAPP_KEYCODE_S: {
          app_state.camera->ProcessKeyboardMovement(MOVE_BACKWARD,
                                                    app_state.deltaTime);
        } break;
        case SAPP_KEYCODE_A: {
          app_state.camera->ProcessKeyboardMovement(MOVE_LEFT,
                                                    app_state.deltaTime);
        } break;
        case SAPP_KEYCODE_D: {
          app_state.camera->ProcessKeyboardMovement(MOVE_RIGHT,
                                                    app_state.deltaTime);
        } break;
        case SAPP_KEYCODE_Q: {
          if (app_state.lastPress > app_state.keyCooldown) {
            app_state.camera->ProcessKeyboardRotation(TURN_LEFT,
                                                      app_state.deltaTime);
          }
        } break;
        case SAPP_KEYCODE_E: {
          if (app_state.lastPress > app_state.keyCooldown) {
            app_state.camera->ProcessKeyboardRotation(TURN_RIGHT,
                                                      app_state.deltaTime);
          }
        } break;
        case SAPP_KEYCODE_ESCAPE: {
          sapp_request_quit();
        } break;
      }
    } break;
    case SAPP_EVENTTYPE_MOUSE_DOWN: {
      if (e->mouse_button == SAPP_MOUSEBUTTON_RIGHT) {
        app_state.freeLook = true;
      }
    }
    case SAPP_EVENTTYPE_MOUSE_UP: {
      if (e->mouse_button == SAPP_MOUSEBUTTON_RIGHT) {
        app_state.freeLook = false;
      }
    }
  }
}

void updateFrameTime() {
  uint64_t currentFrameTime = stm_now();
  app_state.deltaTime = (float)(stm_sec(
      stm_diff(currentFrameTime, app_state.lastFrameTimestamp)));
  app_state.lastFrameTimestamp = currentFrameTime;

  app_state.lastPress += app_state.deltaTime;
}

void init() {
  app_state.screenWidth = ScreenWidth;
  app_state.screenHeight = ScreenHeight;

  sg_desc d = {0};
  d.context = sapp_sgcontext();
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
  app_state.freeLook = false;
  app_state.lastFrameTimestamp = stm_now();
}

void update() {
  updateFrameTime();
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