#pragma once
#include <bgfx/bgfx.h>

struct PosColorVertex {
    float x;
    float y;
    float z;
    uint32_t abgr;
    static const bgfx::VertexLayout ms_layout;

private:
    static const bgfx::VertexLayout gen_layout() {
        bgfx::VertexLayout layout;
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
        return layout;
    }
};



struct PosVertex {
    float x;
    float y;
    float z;
    static const bgfx::VertexLayout ms_layout;

private:
    static const bgfx::VertexLayout gen_layout() {
        bgfx::VertexLayout layout;
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .end();
        return layout;
    }
};
