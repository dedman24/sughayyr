#ifndef SHADERS_H_INCLUDED
#define SHADERS_H_INCLUDED

// this file handles shader loading & compilation by itself.

// compiles fragment & vertex shaders together.
// returns shader program.
GLuint shader_compile(const char* const restrict sh_vert, const char* const restrict sh_frag);

#endif

#ifdef RENDERER_IMPLEMENTATION

#include "../model/fileops.h"       // mapping stuff.

// stdlib includes.
#include "stdio.h"                  // FILE handling ops.
#include "stdlib.h"                 // malloc, free.

// compiles single shader, returns 0 if fails, shader if it succeeds.
static GLuint shader_compile__single(const char* const restrict path, const GLenum shaderType){
  model__fileT sh_code = model_load__map(path);
  if(!sh_code.data){
    fprintf(stderr, "ERROR: could not open file '%s'.\n", path);
    return 0;
  }

  const GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, (const char* const*)&sh_code.data, NULL);
  glCompileShader(shader);
  model_load__unmap(sh_code);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success){
    char err[512];
    glGetShaderInfoLog(shader, 512, NULL, err);
    fprintf(
      stderr,
      "ERROR: %s shader compilation failed."
      "GLSL compiler returned:\n%512s\n",
      shaderType == GL_VERTEX_SHADER? "vertex": "fragment", err
    );
    return 0;
  }
  return shader;
}

GLuint shader_compile(const char* const restrict path_vert, const char* const restrict path_frag){
  const GLuint sh_vert = shader_compile__single(path_vert, GL_VERTEX_SHADER);
  const GLuint sh_frag = shader_compile__single(path_frag, GL_FRAGMENT_SHADER);

  if(!sh_vert || !sh_frag) return 0;
// creates shader program
  GLuint program = glCreateProgram();
  glAttachShader(program, sh_vert);
  glAttachShader(program, sh_frag);
  glLinkProgram(program);

  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if(!success) {
    char err[512];
    glGetProgramInfoLog(program, 512, NULL, err);
    fprintf(stderr, "ERROR: shader program creation failed. GLSL compiler returned:\n%512s.\n", err);
    return 1;
  }
  glDeleteShader(sh_vert);
  glDeleteShader(sh_frag);

  return program;
}

#endif
