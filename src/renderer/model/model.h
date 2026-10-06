// parses .obj files.

#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

// debug info:

// formats of models in shaders:
//  (location = 0) -> vertex position in xyz format.
//  (location = 1) -> texture coordinates in uv format/texture colours in rgb format/vertex normals in xyz format (if the other 2 do not exist).
//  (location = 2) -> vertex normals in xyz format.
#include "mtype.h"

// defined in 'mtype.h'.
void model_destroy(modelT* const restrict model);

// defined in 'obj.h'.
int model_load__obj(const char* const restrict path, float* restrict* const restrict vertices, uint16_t* restrict* const restrict faces);

// TODO: one day, add fast multithreaded loading of multiple models at the same time.
// loading .obj files is slow, nothing can be done about it.

modelT* model_load(const char* const restrict path, const char* const restrict path_texture);
modelT* model_load__many(const char* const restrict* const restrict paths);

#endif

#ifdef RENDERER_IMPLEMENTATION

#include "mtl.h"
#include "obj.h"

// handles model loading, initialisation in OpenGL.
modelT* model_load(const char* const restrict path, const char* const restrict path_texture){
  float* restrict data;
  uint16_t* restrict faces;
  const int formatting = model_load__obj(path, &data, &faces);
  if(formatting == MODEL__ERROR) return NULL;
  modelT* const restrict model = model_init(NULL, formatting);

// binds & generates buffers.
  glBindVertexArray(model->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, model->VBO);
  glBufferData(GL_ARRAY_BUFFER, vfastarr_size(data), data, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vfastarr_size(faces), faces, GL_STATIC_DRAW);

  vfastarr_destroy(data);
  vfastarr_destroy(faces);
// attribute stuff.
  const size_t stride = (
    3 + 
    (model->formatting >> MODEL__TEXTURE__SHAMT & 1)*2 +
    (model->formatting >> MODEL__COLOUR__SHAMT & 1)*3 +
    (model->formatting >> MODEL__NORMAL__SHAMT & 1)*3
  )*sizeof(float);

  const size_t s_second = 3*sizeof(float);
  const size_t s_third = 3*sizeof(float) +
    (model->formatting & MODEL__TEXTURE)*2*sizeof(float) +
    (model->formatting & MODEL__COLOUR)*3*sizeof(float); 
  size_t loc_attribarray = 0;

  // vertices.
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
  glEnableVertexAttribArray(loc_attribarray++);
  // textures or colours.
  if(model->formatting & MODEL__TEXTURE){
    glVertexAttribPointer(loc_attribarray, 2, GL_FLOAT, GL_FALSE, stride, (void*)s_second);
    glEnableVertexAttribArray(loc_attribarray++);
  }
  else if(model->formatting & MODEL__COLOUR){
    glVertexAttribPointer(loc_attribarray, 3, GL_FLOAT, GL_FALSE, stride, (void*)s_second);
    glEnableVertexAttribArray(loc_attribarray++);
  }
  // normals.
  if(model->formatting & MODEL__NORMAL){
    glVertexAttribPointer(loc_attribarray, 2, GL_FLOAT, GL_FALSE, stride, (void*)s_third);
    glEnableVertexAttribArray(loc_attribarray++);
  }

  if(path_texture){
    glGenTextures(1, &model->id_texture);
    glBindTexture(GL_TEXTURE_2D, model->id_texture);
// TODO: possibly change based on texture stuff.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    textureT texture;
    texture_load(&texture, path_texture, false);
    const GLint ch_cnt = texture.ch == 3? GL_RGB: GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, ch_cnt, texture.width, texture.height, 0, ch_cnt, GL_UNSIGNED_BYTE, texture.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    texture_destroy(&texture);
  }

// cleanup avoids corruption.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return model;
}

#endif
