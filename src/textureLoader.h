#ifndef _TEXTURE_LOADER_H_
#define _TEXTURE_LOADER_H_

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "sokol_fetch.h"

#define MAX_FILE_SIZE (10 * 1024 * 1024)
#define NUM_CHANNELS (4)
#define NUM_LANES (8)
uint8_t fileBuffer[NUM_CHANNELS][NUM_LANES][MAX_FILE_SIZE];

typedef struct {
  sfetch_handle_t handle;
  sg_bindings* binding;
  int slotId;
  int requestId;
} request_t;

#define NUM_REQUESTS (32)
request_t requests[NUM_REQUESTS];
static int requestsMade = 0;

static void fetch_callback(const sfetch_response_t* response);

static void initTextureLoader(void) {
  for (int i = 0; i < NUM_REQUESTS; i++) {
    requests[i].binding = 0;
  }
  sfetch_desc_t fetchDesc = {0};
  fetchDesc.num_channels = NUM_CHANNELS;
  fetchDesc.num_lanes = NUM_LANES;
  sfetch_setup(&fetchDesc);
}

static void destroyTextureLoader(void) {
  sfetch_shutdown();
}

static void loadTexture(const char* fileName, sg_bindings* bindLoc, int slot) {
  if (requestsMade >= NUM_REQUESTS) {
    return;
  }
  requests[requestsMade].slotId = slot;
  requests[requestsMade].requestId = requestsMade;
  requests[requestsMade].binding = bindLoc;

  sfetch_request_t fetchRequest = {0};
  fetchRequest.path = fileName;
  fetchRequest.callback = fetch_callback;
  requests[requestsMade++].handle = sfetch_send(&fetchRequest);
}

static void texturePump(void) {
  sfetch_dowork();
}

static void fetch_callback(const sfetch_response_t* response) {
  if (response->dispatched) {
    void* ptr = fileBuffer[response->channel][response->lane];
    sfetch_bind_buffer(response->handle, ptr, MAX_FILE_SIZE);
  }
  if (response->fetched) {
    int index = -1;
    for (int i = 0; i < requestsMade; i++) {
      if (requests[i].handle.id == response->handle.id) {
        index = i;
        break;
      }
    }
    if (index >= 0) {
      int texWidth, texHeight, numChannels;
      const int desiredChannels = 4;
      stbi_uc* pixels = stbi_load_from_memory(
          (const stbi_uc*)response->buffer_ptr, (int)response->fetched_size,
          &texWidth, &texHeight, &numChannels, desiredChannels);
      if (pixels) {
        sg_image_desc imageDesc = {0};
        imageDesc.width = texWidth;
        imageDesc.height = texHeight;
        imageDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
        imageDesc.min_filter = SG_FILTER_LINEAR;
        imageDesc.mag_filter = SG_FILTER_LINEAR;
        imageDesc.content.subimage[0][0].ptr = pixels;
        imageDesc.content.subimage[0][0].size = texWidth * texHeight * 4;
        sg_init_image(
            requests[index].binding->fs_images[requests[index].slotId],
            &imageDesc);
        stbi_image_free(pixels);
      }
    }
  }
}

#endif  // _TEXTURE_LOADER_H_