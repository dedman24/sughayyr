#ifndef MODEL_MTL_H_INCLUDED
#define MODEL_MTL_H_INCLUDED

// tiny .mtl file interpreter.

// stdlib includes.
#include "stddef.h"       // size_t, NULL.
#include "stdint.h"       // integer types.
#include "string.h"       // strncmp.

typedef enum{
  MODEL_MTL__NEWMTL = 5,
  MODEL_MTL__COMMENT = 2,
  MODEL_MTL__AMBIENT_COLOUR = 6,
  MODEL_MTL__DISSOLVE = 4,
  MODEL_MTL__DIFFUSE_COLOUR = 7,
  MODEL_MTL__TRANSPARENCY = 3,
  MODEL_MTL__SPECULAR_EXPONENT = 0,
  MODEL_MTL__SPECULAR_COLOUR = 1,
  MODEL_MTL__NONE = 8
} model_mtl__tokT;

// hash function, generated with vfasttables version 1.1.0.
static model_mtl__tokT model_mtl__hash(char* const restrict str, const size_t len){
    if(len > 127) return 0;
// associated values generated for hash function.
    const uint8_t associated_values[] = {
        27,160,29,177,173,18,20,202,204,32,159,17,171,227,92,42,
        138,91,198,229,221,75,237,42,104,184,220,52,137,201,252,164,
        5,250,13,209,218,38,244,217,214,237,128,113,184,10,47,33,
        71,84,224,135,42,228,107,221,13,99,5,201,44,106,2,49,
        227,26,199,126,206,153,164,62,124,56,59,32,244,226,7,1,
        130,101,119,18,102,201,91,246,167,119,212,194,108,228,103,53,
        135,169,20,228,146,22,189,121,86,26,219,228,127,194,99,226,
        109,192,192,236,219,23,38,162,3,205,73,238,240,60,202,191
    };
    const size_t indices[] = {
        1,         0 
    };
    uint8_t hash = associated_values[len];
    for(size_t i = 0; i < sizeof(indices)/sizeof(*indices); i++){
        hash += str[associated_values[indices[i]] % len];
    }
    hash %= 8;
    return hash;
}

static model_mtl__tokT model_mtl__search(char* const restrict str, const size_t len){
    const model_mtl__tokT index = model_mtl__hash(str, len);
    char* table[8] = {
       [MODEL_MTL__NEWMTL] = "newmtl",
       [MODEL_MTL__COMMENT] = "#",
       [MODEL_MTL__AMBIENT_COLOUR] = "Ka",
       [MODEL_MTL__DISSOLVE] = "d",
       [MODEL_MTL__DIFFUSE_COLOUR] = "Kd",
       [MODEL_MTL__TRANSPARENCY] = "Tr",
       [MODEL_MTL__SPECULAR_EXPONENT] = "Ns",
       [MODEL_MTL__SPECULAR_COLOUR] = "Ks"
    };
    return strncmp(table[index], str, len) == 0? index: MODEL_MTL__NONE;
}


#endif
