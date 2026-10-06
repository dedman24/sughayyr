#ifndef MODEL_OBJ_H_INCLUDED
#define MODEL_OBJ_H_INCLUDED

// tiny .obj file interpreter.

#include "fileops.h"
#include "../../linking/vfastarr/vfastarr.h"

// stdlib includes.
#include "math.h"         // NAN constant.
#include "stddef.h"       // size_t, NULL.
#include "stdint.h"       // integer types.
#include "string.h"       // strncmp, various string functions.
#include "inttypes.h"     // SCNd16.

typedef enum{
  MODEL_OBJ__VERTEX = 4,
  MODEL_OBJ__TEXTURE = 6,
  MODEL_OBJ__NORMAL = 0,
  MODEL_OBJ__FACE = 5,
  MODEL_OBJ__MTLIB = 1,
  MODEL_OBJ__COMMENT = 3,
  MODEL_OBJ__USEMTL = 2,
  MODEL_OBJ__NONE = 7,
  MODEL_OBJ__ERROR_UNKNOWN_KEYWORD,
} model_obj__tokT;

// hash function, generated with vfasttables version 1.1.0.
static model_obj__tokT model_obj__hash(char* const restrict str, const size_t len){
    if(len > 127) return 0;
// associated values generated for hash function.
    const uint8_t associated_values[] = {
        85,133,117,109,94,91,193,123,161,183,187,67,25,137,220,1,
        7,60,25,137,167,248,141,56,225,44,208,141,63,233,42,82,
        33,135,143,66,202,3,89,98,20,205,164,158,212,239,157,2,
        21,122,54,165,35,135,223,28,26,124,82,143,222,151,227,49,
        136,24,184,187,207,195,227,232,126,185,146,174,187,156,51,225,
        191,15,68,222,187,162,6,144,194,108,217,50,98,93,207,237,
        57,151,161,199,180,68,184,9,189,97,89,219,139,123,146,129,
        10,157,22,54,92,137,148,45,132,221,3,212,22,40,33,116
    };
    const size_t indices[] = {
        1 
    };
    uint8_t hash = associated_values[len];
    for(size_t i = 0; i < sizeof(indices)/sizeof(*indices); i++){
        hash += associated_values[str[indices[i] % len]];
    }
    hash %= 7;
    return hash;
}

static model_obj__tokT model_obj__search(char* const restrict str, const size_t len){
    const model_obj__tokT index = model_obj__hash(str, len);
    char* table[7] = {
       [MODEL_OBJ__VERTEX] = "v",
       [MODEL_OBJ__TEXTURE] = "vt",
       [MODEL_OBJ__NORMAL] = "vn",
       [MODEL_OBJ__FACE] = "f",
       [MODEL_OBJ__MTLIB] = "mtlib",
       [MODEL_OBJ__COMMENT] = "#",
       [MODEL_OBJ__USEMTL] = "usemtl"
    };
    return strncmp(table[index], str, len) == 0? index: MODEL_OBJ__ERROR_UNKNOWN_KEYWORD;
}

typedef struct{ const model_obj__tokT tok; const size_t len; } model_obj__tok_rT;

static model_obj__tok_rT model_obj__tok(char* const restrict str){
  const char* const restrict whitespace = strchr(str, ' ');
// terminates parsing.
  if(!whitespace) return (model_obj__tok_rT){ .tok = MODEL_OBJ__NONE, .len = 0 };
  const size_t s = (whitespace - str);

  return (model_obj__tok_rT){ .tok = model_obj__search(str, s), .len = s };
}

static size_t model_obj__go_to_newline(const char* restrict line, const size_t cursor, const size_t s_obj){
  const char* const restrict eol = strchr(line, '\n');
  if(!eol) return s_obj;
  return cursor + (eol - line) + 1;
}

static void model_obj__vertex(const char* restrict line, float* restrict* const restrict vertices){
// w coordinate not supported.
  const size_t len_total = vfastarr_elcnt(*vertices, sizeof(**vertices));
  *vertices = vfastarr_extend(*vertices, 3, sizeof(**vertices));
  sscanf(line, "%f %f %f", &(*vertices)[len_total + 0], &(*vertices)[len_total + 1], &(*vertices)[len_total + 2]);
}

static void model_obj__texture(const char* restrict line, float* restrict* const restrict textures, const size_t pos_newline){  
  const size_t len_total = vfastarr_elcnt(*textures, sizeof(**textures));
  *textures = vfastarr_extend(*textures, 2, sizeof(**textures));

  const int amt_read = sscanf(line, "%f %f", &(*textures)[len_total], &(*textures)[len_total + 1]);
  if(amt_read < 2) for(int i = 0; i < amt_read; i++)
    (*textures)[len_total + 1 - i] = 1.0f;
}

static void model_obj__normal(const char* restrict line, float* restrict* const restrict normals){
  const size_t len_total = vfastarr_elcnt(*normals, sizeof(**normals));
  *normals = vfastarr_extend(*normals, 3, sizeof(**normals));
  sscanf(line, "%f %f %f", &(*normals)[len_total + 0], &(*normals)[len_total + 1], &(*normals)[len_total + 2]);
}

static unsigned short model_obj__face__format(const char* const restrict line, const size_t pos_newline){
  const char* const restrict slash0 = strchr(line, '/'), *const restrict space = strchr(line, ' ');
  if(!slash0 || slash0 - line < space - line) return MODEL__VERTEX;

  const char* const restrict slash1 = strchr(slash0 + 1, '/');
  if(!slash1 || slash1 - line > space - line) return MODEL__VERTEX | MODEL__TEXTURE;
  if(slash1 == slash0 + 1) return MODEL__VERTEX | MODEL__NORMAL;

  return MODEL__VERTEX | MODEL__TEXTURE | MODEL__NORMAL;
}

static void model_obj__face(
  float* restrict* const restrict data,
  uint16_t* restrict* const restrict faces,
  int* const restrict format,
  const char* restrict line,
  float* const restrict vertices,
  float* const restrict textures,
  float* const restrict normals,
  const size_t pos_newline
){
// figure out formatting if not set.
  if(*format == MODEL__ERROR)
    *format = model_obj__face__format(line, pos_newline);
// TODO: make these int16_t typed values.
  int16_t vertex[3], texture[3], normal[3];
  int16_t max_idx = 0;

  for(size_t i = 0; i < 3; i++){
    sscanf(line, "%" SCNd16, &vertex[i]);
    if(vertex[i] < 0) vertex[i] = vfastarr_elcnt(vertices, 3*sizeof(*vertices)) + vertex[i];
    if(vertex[i] > max_idx) max_idx = vertex[i];

    if(*format & (MODEL__TEXTURE | MODEL__NORMAL)){
      line = strchr(line, '/') + 1;
      if(*format & MODEL__TEXTURE){
        sscanf(line, "%" SCNd16, &texture[i]);
        if(texture[i] < 0) texture[i] = vfastarr_elcnt(textures, 2*sizeof(*textures)) + texture[i];
      }
      if(*format & MODEL__NORMAL){
        line = strchr(line, '/') + 1;
        sscanf(line, "%" SCNd16, &normal[i]);
        if(normal[i] < 0) normal[i] = vfastarr_elcnt(normals, 3*sizeof(*normals)) + normal[i];
      }
    }
    line = strchr(line, ' ');
  }

  *faces = vfastarr_append(*faces, &vertex, sizeof(vertex));
// if maxindex > arr size, we increase total size.
// elements that are not populated yet are marked with NaN.
// negative values have to be handled early.
  const size_t off_texture = 3;
  const size_t off_normal = off_texture + (*format >> MODEL__TEXTURE__SHAMT & 1)*2;

  const size_t size_elem_data = 3 +
    (*format >> MODEL__TEXTURE__SHAMT & 1)*2 +
    (*format >> MODEL__COLOUR__SHAMT  & 1)*3 +
    (*format >> MODEL__NORMAL__SHAMT  & 1)*3;

  {
    const size_t cnt_data = vfastarr_elcnt(*data, size_elem_data*sizeof(float));
    if(max_idx > cnt_data){
      *data = vfastarr_extend(*data, max_idx + 1, size_elem_data*sizeof(float));
      for(size_t i = cnt_data; i < max_idx + 1; i++)
        (*data)[i*size_elem_data] = NAN;
    }
  }

  for(size_t i = 0; i < 3; i++){
// if this is true, we've already populated this entry.
    if((*data)[vertex[i]*size_elem_data] == vertices[vertex[i]*3]) continue;

    if(*format & MODEL__VERTEX)
      memcpy(*data + vertex[i]*size_elem_data, vertices + vertex[i]*3, sizeof(float)*3);
    if(*format & MODEL__TEXTURE)
      memcpy(*data + vertex[i]*size_elem_data + off_texture, textures + texture[i]*2, sizeof(float)*2);
    if(*format & MODEL__NORMAL)
      memcpy(*data + vertex[i]*size_elem_data + off_normal, normals + normal[i]*3, sizeof(float)*3);
  }
}

int model_load__obj(const char* const restrict path, float* restrict* const restrict data, uint16_t* restrict* const restrict faces){
  const model__fileT obj = model_load__map(path);
  if(obj.data == NULL) return MODEL__ERROR;

  *faces = vfastarr_init(sizeof(**faces)*3);
  *data  = vfastarr_init(sizeof(**data));

  float* restrict vertices = vfastarr_init(sizeof(*vertices));
  float* restrict normals  = vfastarr_init(sizeof(*normals));
  float* restrict textures = vfastarr_init(sizeof(*textures));
  int format = MODEL__ERROR;

  for(size_t cursor = 0; cursor < obj.size;){
    const model_obj__tok_rT tok = model_obj__tok(obj.data + cursor);
    cursor += tok.len;

    const char* const restrict line = obj.data + cursor;
    const size_t pos_newline = model_obj__go_to_newline(line, cursor, obj.size);

    switch(tok.tok){
      case MODEL_OBJ__VERTEX:
        model_obj__vertex(line, &vertices);
        break;
      case MODEL_OBJ__TEXTURE:
        model_obj__texture(line, &textures, pos_newline - cursor);
        break;
      case MODEL_OBJ__NORMAL:
        model_obj__normal(line, &normals);
        break;
      case MODEL_OBJ__FACE:
        model_obj__face(data, faces, &format, line, vertices, textures, normals, pos_newline - cursor);
        break;
      case MODEL_OBJ__COMMENT:
        break;
      case MODEL_OBJ__MTLIB:
      case MODEL_OBJ__USEMTL:
        fputs("ERROR: option not currently implemented in 'model_load__obj'.\n", stderr);
        vfastarr_destroy(*data); vfastarr_destroy(*faces);
        return MODEL__ERROR;
      case MODEL_OBJ__NONE:               // exits program.
        cursor = obj.size;
        break;
      case MODEL_OBJ__ERROR_UNKNOWN_KEYWORD:
        fputs("ERROR: unknown keyword met in 'model_load__obj'.\n", stderr);
        vfastarr_destroy(*data); vfastarr_destroy(*faces);
        return MODEL__ERROR;
    }
    cursor = pos_newline;
  }
  vfastarr_destroy(vertices);
  vfastarr_destroy(textures);
  vfastarr_destroy(normals);
  model_load__unmap(obj);

  return format;
}

#endif
