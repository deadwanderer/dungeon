#pragma once

#include "sokol_gfx.h"
#include "textureLoader.h"
#include "shaders.glsl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "dungeon_surface.h"
#include "dungeon_params.h"

class DungeonSurfaceRenderer {
 public:
  void init() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  // bottom left
        0.5f,  -0.5f, 0.0f, 0.0f, 0.0f,  // top left
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  // top right
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f,  // bottom right
    };
    uint16_t indices[] = {0, 2, 1, 0, 3, 2};

    sg_pipeline_desc pipe_desc = {0};
    pipe_desc.face_winding = SG_FACEWINDING_CW;
    pipe_desc.shader = sg_make_shader(surface_shader_desc(sg_query_backend()));
    pipe_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
    pipe_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
    pipe_desc.layout.buffers[0].stride = 5 * sizeof(float);
    pipe_desc.index_type = SG_INDEXTYPE_UINT16;
    pipe_desc.sample_count = 4;
    pipe_desc.cull_mode = SG_CULLMODE_BACK;
    pipe_desc.depth.write_enabled = true;
    pipe_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    pipe_desc.label = "dungeon-surface-pipeline";
    wall_pip = sg_make_pipeline(&pipe_desc);

    sg_buffer_desc buf_desc = {0};
    buf_desc.label = "dungeon-surface-vertices";
    buf_desc.data = SG_RANGE(vertices);
    wall_bind.vertex_buffers[0] = sg_make_buffer(&buf_desc);
    buf_desc.label = "dungeon-surface-indices";
    buf_desc.type = SG_BUFFERTYPE_INDEXBUFFER;
    buf_desc.data = SG_RANGE(indices);
    wall_bind.index_buffer = sg_make_buffer(&buf_desc);
    vs_params = {};
    for (uint32_t i = 0; i < SurfaceType_Count; i++) {
      wall_images[i] = sg_alloc_image();
      loadTexture(wall_image_paths[i], wall_images[i], SLOT_surfaceTex);
    }
  }

  void begin_render(glm::mat4 viewproj) {
    sg_apply_pipeline(wall_pip);

    vs_params.viewproj = viewproj;
  }

  void apply_textures(SurfaceType type) {
    wall_bind.fs_images[SLOT_surfaceTex] = wall_images[type];
    sg_apply_bindings(&wall_bind);
  }

  void render_surface(DungeonSurface& surf) {
    vs_params.model = surf.model;
    vs_params.color = surf.color;

    sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_surface_vs_params,
                      &SG_RANGE(vs_params));
    sg_draw(0, 6, 1);
  }

 private:
  sg_pipeline wall_pip;
  sg_bindings wall_bind;
  sg_image wall_images[SurfaceType_Count];
  surface_vs_params_t vs_params;
};