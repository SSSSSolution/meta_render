#ifndef _META_RENDER_RENDERER_H_
#define _META_RENDER_RENDERER_H_


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <windows.h>

#include <vector>
#include <string>

namespace meta_renderer
{

class MetaRenderer
{
public:
    MetaRenderer();
    ~MetaRenderer();

    virtual void init() = 0;

private:

};


class VulkanContext
{
public:
    bool init();
    void set_enable_layers(const std::vector<const char *> &layers);
    void set_enable_extensions(const std::vector<const char *> &extensions);
    void set_dev_enable_extensions(const std::vector<const char *> &extensions);

    void set_window(HINSTANCE instance, HWND hwnd, int width, int height);

    void create_instance();
    void check_layer_support();
    void check_extension_support();
    void check_physical_device();
    bool create_logic_device();

    bool create_cmd_pool();
    bool create_cmd_buf();

    void create_surface();
    void create_swapchain();

    void create_depth_buffer();
    void create_uniform_buffer();

private:
    VkInstance m_instance;
    std::vector<const char *> m_enable_layers;
    std::vector<const char *> m_enable_extensions;
    std::vector<const char *> m_dev_extensions;
    std::vector<VkPhysicalDevice> m_device_list;
    std::vector<VkQueueFamilyProperties> m_queue_family_props;
    VkDevice m_device;

    // surface
    HINSTANCE m_win_instance;
    HWND m_hwnd;
    int m_width;
    int m_height;

    VkSurfaceKHR m_surface;
    VkFormat m_format;
    VkSwapchainKHR m_swapchain;
    uint32_t m_swapchain_image_count;

    std::vector<VkImage> m_swapchain_image_list;
    std::vector<VkImageView> m_swapchain_image_view_list;

    VkImage m_depth_image;
    VkImageView m_depth_image_view;
    VkDeviceMemory m_depth_image_mem;

    VkCommandPool m_cmd_pool;
    VkCommandBuffer m_cmd_buf;
};

class VulkanRenderer : public MetaRenderer
{
public:
    VulkanRenderer();
    ~VulkanRenderer();

    virtual void init() override;

private:
    VulkanContext m_vk_context;


};

}




















#endif
