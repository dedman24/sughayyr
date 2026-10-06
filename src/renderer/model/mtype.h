#ifndef MODEL_TYPE_H_INCLUDED
#define MODEL_TYPE_H_INCLUDED

#include "../textures/textures.h"
#include "../../linking/vfastarr/vfastarr.h"

// stdlib includes.
#include "stdint.h"         // uint16_t.
#include "stdlib.h"         // malloc, free.

enum{
  MODEL__VERTEX__SHAMT  = 0,
  MODEL__TEXTURE__SHAMT = 1,
  MODEL__COLOUR__SHAMT  = 2,
  MODEL__NORMAL__SHAMT  = 3,
  MODEL__ERROR__SHAMT   = 4
};

typedef struct{
  enum{
// what model actually includes.
// 'vertices' is a packed array formatted as:
//    vertexdata * 3 | texture (if present) * 2 | colour (if present) * 3 | normal (if present) * 3.
// formatting holds what data is actually included.
    MODEL__VERTEX  = 1,
    MODEL__TEXTURE = 2,
    MODEL__COLOUR  = 4,
    MODEL__NORMAL  = 8,
    MODEL__ERROR   = 16
  } formatting;
// opengl stuff.
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  GLuint id_texture;
} modelT;

#define MODEL__NULL ((modelT){ .formatting = MODEL__ERROR, .vertices = NULL, .faces = NULL, })

#endif

#ifdef RENDERER_IMPLEMENTATION

static modelT* model_init(modelT* restrict model, const int format){
  if(!model) model = malloc(sizeof(*model));

  model->formatting = format;
// initialises OpenGL stuff.
  glGenVertexArrays(1, &model->VAO);
  glGenBuffers(1, &model->VBO);
  glGenBuffers(1, &model->EBO);

  return model;
}

void model_destroy(modelT* const restrict model){
// TODO: learn how to free texture.
  glDeleteBuffers(1, &model->VBO);
  glDeleteBuffers(1, &model->EBO);
  glDeleteVertexArrays(1, &model->VAO);

  free(model);
}

#endif
