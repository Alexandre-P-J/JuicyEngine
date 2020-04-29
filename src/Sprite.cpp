#include <ResourceManager.h>
#include <Sprite.h>
#include <VertexLayouts.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <vector>



bgfx::VertexBufferHandle Sprite::default_vertices = BGFX_INVALID_HANDLE;
bgfx::IndexBufferHandle Sprite::default_indexes = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle Sprite::default_shader_program = BGFX_INVALID_HANDLE;



Sprite::Sprite() {
    if (!(isValid(default_vertices) && isValid(default_indexes) &&
          isValid(default_shader_program))) {
        init_default_handlers();
    }
}


void Sprite::init_default_handlers() {
    static PosColorVertex vertices[] = {{25.0f, 25.0f, 0.0f, 0xff0000ff},
                                        {25.0f, -25.0f, 0.0f, 0xff00ffff},
                                        {-25.0f, -25.0f, 0.0f, 0xffff0000},
                                        {-25.0f, 25.0f, 0.0f, 0xff00ff00}};
    default_vertices = bgfx::createVertexBuffer(
        bgfx::makeRef(vertices, sizeof(vertices)), PosColorVertex::ms_layout);

    static const uint16_t indices[] = {0, 3, 2, 2, 1, 0};
    default_indexes =
        bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));
    auto sp = ResourceManager::get_instance()->loadShaderProgram(
        "basic/v_simple.bin", "basic/f_simple.bin");
    if (sp) {
        default_shader_program = sp.value();
    } else
        spdlog::critical("Default sprite shader couldn't be loaded!");
}



void Sprite::draw() {
    auto render = Renderer::get_instance();
    //glm::mat4 transform = glm::identity<glm::mat4>();
    //transform = glm::translate(transform, glm::vec3(75.0f, 75.0f, 0.0f));
    render->set_current_transform(transform, -100);
    render->set_current_buffers(default_vertices, default_indexes);
    render->submit_current(default_shader_program);
}