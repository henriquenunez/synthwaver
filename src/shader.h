#ifndef SHADER_LIB
#define SHADER_LIB

#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _shader shader_t;
typedef enum _shader_err
{
    SH_OK,
    SH_NOT_OK
} shader_err;

shader_t* load_shader(const char*, const char*, shader_err*);
shader_err unload_shader(shader_t*);
shader_err use_shader(shader_t*);
shader_err set_uniform_float(shader_t*, float val, const char* uniform);
shader_err set_uniform_float3(shader_t*, float val_1, float val_2, float val_3, const char* uniform);
int shader_get_id(shader_t*);

struct _shader
{
    int shader_program;
};

shader_err unload_shader(shader_t* a_shader)
{
    //TODO delete shader on opengl?
    free(a_shader);

    return SH_OK;
}

//TODO: introduce a list of args.
shader_t* load_shader(const char* vtx_shd, const char* frag_shd, shader_err* errc)
{
    shader_t* ret = (shader_t*) malloc(sizeof(shader_t));
    char info_log[512];
    FILE* vtx_file;
    FILE* frag_file;
    size_t shader_size;
    shader_err _errc = SH_OK;

    vtx_file = fopen(vtx_shd, "rb");
    frag_file = fopen(frag_shd, "rb");

    if (vtx_file == NULL)
        printf("Deu ruim no veritce\n");

    if (frag_file == NULL)
        printf("Deu ruim no fragm\n");

    //Size of shader
    fseek(vtx_file, 0, SEEK_END);
    shader_size = ftell(vtx_file);

    char* vtx_buffer = (char*) calloc(shader_size+1, 1);
    fseek(vtx_file, 0, SEEK_SET);
    fread(vtx_buffer, 1, shader_size, vtx_file);

    //Size of shader
    fseek(frag_file, 0, SEEK_END);
    shader_size = ftell(frag_file);

    char* frag_buffer = (char*) calloc(shader_size+1, 1);
    fseek(frag_file, 0, SEEK_SET);
    fread(frag_buffer, 1, shader_size, frag_file);

    //Now, compiling shaders.
    int vtx_shader_id, frag_shader_id, shader_success;
    vtx_shader_id = glCreateShader(GL_VERTEX_SHADER);
    frag_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(vtx_shader_id, 1, &vtx_buffer, NULL);
    glCompileShader(vtx_shader_id);
    glGetShaderiv(vtx_shader_id, GL_COMPILE_STATUS,  &shader_success);
    if(!shader_success)
    {
        glGetShaderInfoLog(vtx_shader_id, 512, NULL, info_log);
        printf("VERTEX SHADER COMPILATION FAILED\n%s\n", info_log);
        _errc = SH_NOT_OK;
    }

    glShaderSource(frag_shader_id, 1, &frag_buffer, NULL);
    glCompileShader(frag_shader_id);
    glGetShaderiv(frag_shader_id, GL_COMPILE_STATUS,  &shader_success);
    if(!shader_success)
    {
        glGetShaderInfoLog(frag_shader_id, 512, NULL, info_log);
        printf("FRAGMENT SHADER COMPILATION FAILED\n%s\n", info_log);
        _errc = SH_NOT_OK;
    }

    //Now, link to program.
    ret->shader_program = glCreateProgram();
    glAttachShader(ret->shader_program, vtx_shader_id);
    glAttachShader(ret->shader_program, frag_shader_id);
    glLinkProgram(ret->shader_program); 

    glGetProgramiv(ret->shader_program, GL_LINK_STATUS, &shader_success);
    if(!shader_success)
    {
        glGetProgramInfoLog(ret->shader_program, 512, NULL, info_log);
        printf("PROGRAM LINKING FAILED\n%s\n", info_log);
        _errc = SH_NOT_OK;
    }

    //Deleting unused shaders.
    glDeleteShader(vtx_shader_id);
    glDeleteShader(frag_shader_id);

    free(frag_buffer);
    free(vtx_buffer);

    if (errc != NULL) *errc = _errc;

    return ret;
}

shader_err use_shader(shader_t* this_shader)
{
    glUseProgram(this_shader->shader_program);

    return SH_OK;
}

shader_err set_uniform_float(shader_t* this_shader, float val, const char* uniform)
{
    int uniform_loc = glGetUniformLocation(this_shader->shader_program, uniform);
    glUniform1f(uniform_loc, val);

    return SH_OK;
}

shader_err set_uniform_float3(shader_t* this_shader, float val_1, float val_2, float val_3, const char* uniform)
{
    int uniform_loc = glGetUniformLocation(this_shader->shader_program, uniform);
    glUniform3f(uniform_loc, val_1, val_2, val_3);

    return SH_OK;
}

int shader_get_id(shader_t* this_shader)
{
    return this_shader->shader_program;
}


#endif
