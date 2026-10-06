#ifndef TEXTURES_H_INCLUDED
#define TEXTURES_H_INCLUDED

// stdlib includes.
#include "stdbool.h"      // bool type.

typedef struct{ 
  unsigned char* data;
  int width;
  int height;
  int ch;
} textureT;
void texture_destroy(textureT* texture);
void texture_load(textureT* const restrict texture, const char* const restrict path, const bool suppresserrors);

#ifdef RENDERER_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// stdlib includes.
#include "stdio.h"                  // file handling ops.
#include "stdlib.h"                 // malloc.
#include "string.h"                 // memcpy.

void texture_destroy(textureT* texture){
// currently, stbi_image_free is just free() which means we can reuse it to free all data.
  stbi_image_free(texture->data);
}

void texture_load(textureT* const restrict texture, const char* const restrict path, const bool suppresserrors){
  texture->data = stbi_load(path, &texture->width, &texture->height, &texture->ch, 0);
  if(!texture->data){
    if(!suppresserrors) fprintf(stderr, "ERROR: could not find texture pointed by path '%s'.\n", path);
  // missing texture data.
    const unsigned char missing[] = {
      0xdd, 0x00, 0xff, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0xdd, 0x00, 0xff
    };

    texture->data = malloc(sizeof(missing));
    memcpy(texture->data, missing, sizeof(missing));
  }
}


#endif

#endif