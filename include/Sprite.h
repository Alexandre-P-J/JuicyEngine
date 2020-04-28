#pragma once
#include "Node.h"
#include "Renderer.h"

class Sprite : public Node {
    static bgfx::VertexBufferHandle default_vertices;
    static bgfx::IndexBufferHandle default_indexes;
    static bgfx::ProgramHandle default_shader_program;

    void init_default_handlers();

public:
    Sprite();
    virtual void draw();
};