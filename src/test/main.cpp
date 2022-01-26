#include "meta_render/renderer.h"
#include <memory>
#include <iostream>
#include <assert.h>
#include "Windows.h"
#include <vulkan/vulkan_win32.h>
#include "windows_window.h"

using namespace meta_renderer;

int main(int argc, char **argv)
{
    std::cout << "hello";

    WindowsWindow w(0, 0, 740, 580);

    VulkanContext vk_ctx;
    vk_ctx.check_layer_support();
//    vk_ctx.init();
    std::vector<const char *> layers;
    std::string validation_layer = "VK_LAYER_KHRONOS_validation";
    layers.push_back(validation_layer.c_str());

    std::vector<const char *> extensions;
    std::string surface_extension = VK_KHR_SURFACE_EXTENSION_NAME;
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    std::string win32_surface_extension = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

    std::vector<const char *> dev_extensions;
    dev_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    vk_ctx.set_enable_layers(layers);
    vk_ctx.set_enable_extensions(extensions);
    vk_ctx.set_dev_enable_extensions(dev_extensions);

    vk_ctx.create_instance();
    vk_ctx.check_physical_device();
    vk_ctx.create_logic_device();

    assert(vk_ctx.create_cmd_pool());
    assert(vk_ctx.create_cmd_buf());

    vk_ctx.set_window(w.get_hInstance(), w.get_hwnd(), w.get_width(), w.get_height());
    vk_ctx.create_surface();

    vk_ctx.create_swapchain();
    vk_ctx.create_depth_buffer();




    w.show();
    w.exec();

    return 0;
}








