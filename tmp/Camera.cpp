#include <Camera.h>
#include <Renderer.h>

Camera::Camera(bool active) {
    if (active) {
        Renderer::get_instance()->set_active_camera(
            std::shared_ptr<Camera>(this));
    }
}



glm::mat4 Camera::get_view(int min_z_index, int max_z_index) {
    const auto pos = get_global_position();
    const bx::Vec3 at = {pos[0], pos[1], float(max_z_index+1)};
    const bx::Vec3 eye = {pos[0], pos[1], float(min_z_index-1)};
    glm::mat4 view;
    bx::mtxLookAt(&view[0][0], eye, at);
    return view;
}



glm::mat4 Camera::get_projection(int max_z_index) {
    max_z_index = std::abs(max_z_index);
    glm::mat4 proj;
    bx::mtxOrtho(&proj[0][0], 0.f, float(500), 0.f, float(500), 0.f, max_z_index+1, 0.f,
                 true);
    return proj;
}