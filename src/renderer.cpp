#include "meta_render/renderer.h"
#include <stdexcept>
#include <iostream>
#include <assert.h>
#include <memory>
#include <vector>

namespace meta_renderer
{


MetaRenderer::MetaRenderer()
{}

MetaRenderer::~MetaRenderer()
{}


VulkanRenderer::VulkanRenderer()
{}

VulkanRenderer::~VulkanRenderer()
{}

bool VulkanContext::init()
{
    try {
        create_instance();

    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}

// initialize the vulkan libaray by creating an instance
void VulkanContext::create_instance()
{
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "meta_renderer";
    app_info.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instance_info {};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledLayerCount = m_enable_layers.size();
    instance_info.ppEnabledLayerNames = m_enable_layers.data();
    instance_info.enabledExtensionCount = m_enable_extensions.size();
    instance_info.ppEnabledExtensionNames = m_enable_extensions.data();

    VkResult ret = vkCreateInstance(&instance_info, nullptr, &m_instance);
    if (ret != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

void VulkanContext::check_layer_support()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    std::cout << "check validation layer: "  << std::endl;
    for (auto vk_prop : available_layers)
    {
        std::cout << "name: " << vk_prop.layerName << std::endl;
        std::cout << "desc: " << vk_prop.description << std::endl;
    }
}

void VulkanContext::check_extension_support()
{
//    uint32_t extension_count;
//    vkEnumerateDeviceExtensionProperties
}

void VulkanContext::check_physical_device()
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
    m_device_list.resize(device_count);
    vkEnumeratePhysicalDevices(m_instance, &device_count, m_device_list.data());

    for (auto dev : m_device_list)
    {
        VkPhysicalDeviceProperties dev_props;
        vkGetPhysicalDeviceProperties(dev, &dev_props);
        std::cout << dev_props.deviceName << std::endl;

    }
}

bool VulkanContext::create_logic_device()
{
    uint32_t queue_family_prop_count;
    vkGetPhysicalDeviceQueueFamilyProperties(m_device_list[0], &queue_family_prop_count, nullptr);
    m_queue_family_props.resize(queue_family_prop_count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_device_list[0], &queue_family_prop_count, m_queue_family_props.data());
    std::cout << "queue family props count: " << queue_family_prop_count << std::endl;

    for (auto qf_prop : m_queue_family_props)
    {
        std::cout << "queue count: " << qf_prop.queueCount << std::endl;
        std::cout << "queue flag: " << qf_prop.queueFlags << std::endl;
    }

    float queue_priorities[1] = {0.0};
    VkDeviceQueueCreateInfo queue_info {};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.pNext = nullptr;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = queue_priorities;
    queue_info.queueFamilyIndex = 0;

    // Creating the logical device
    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = nullptr;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    device_info.enabledExtensionCount = m_dev_extensions.size();
    device_info.ppEnabledExtensionNames = m_dev_extensions.data();
    device_info.enabledLayerCount = 0;
    device_info.ppEnabledLayerNames = nullptr;
    device_info.pEnabledFeatures = nullptr;

    VkResult res = vkCreateDevice(m_device_list[0], &device_info, nullptr, &m_device);
    return res == VK_SUCCESS;
}

bool VulkanContext::create_cmd_pool()
{
    VkCommandPoolCreateInfo cmd_pool_info = {};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = nullptr;
    cmd_pool_info.queueFamilyIndex = 0;
    cmd_pool_info.flags = 0;

    auto res = vkCreateCommandPool(m_device, &cmd_pool_info, nullptr, &m_cmd_pool);
    return res == VK_SUCCESS;
}

bool VulkanContext::create_cmd_buf()
{
    VkCommandBufferAllocateInfo cmd = {};
    cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd.pNext = nullptr;
    cmd.commandPool = m_cmd_pool;
    cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd.commandBufferCount = 1;

    auto res = vkAllocateCommandBuffers(m_device, &cmd, &m_cmd_buf);
    return res == VK_SUCCESS;
}

void VulkanContext::create_surface()
{
    VkWin32SurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.pNext = nullptr;
    create_info.hinstance = m_win_instance;
    create_info.hwnd = m_hwnd;
    auto ret = vkCreateWin32SurfaceKHR(m_instance, &create_info, nullptr, &m_surface);
    if (ret != VK_SUCCESS)
    {
        throw std::runtime_error("create surface failed");
    }
}

void VulkanContext::create_swapchain()
{
    std::cout << "Create swapchain" << std::endl;

    // get the list of VkFormats that are supported
    uint32_t format_count;
    auto ret = vkGetPhysicalDeviceSurfaceFormatsKHR(m_device_list[0], m_surface, &format_count, nullptr);
    assert(ret == VK_SUCCESS);
    std::vector<VkSurfaceFormatKHR> sur_fmts(format_count);
    ret = vkGetPhysicalDeviceSurfaceFormatsKHR(m_device_list[0], m_surface, &format_count, sur_fmts.data());
    assert(ret == VK_SUCCESS);

    for (auto fmt : sur_fmts)
    {
        std::cout << fmt.format << std::endl;
    }
    m_format = sur_fmts.at(0).format;


    VkSurfaceCapabilitiesKHR surf_cap;
    ret = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device_list[0], m_surface, &surf_cap);
    assert(ret == VK_SUCCESS);

    uint32_t present_mode_count;
    ret = vkGetPhysicalDeviceSurfacePresentModesKHR(m_device_list[0], m_surface, &present_mode_count, nullptr);
    assert(ret == VK_SUCCESS);
    std::vector<VkPresentModeKHR> present_mode_list(present_mode_count);
    ret = vkGetPhysicalDeviceSurfacePresentModesKHR(m_device_list[0], m_surface, &present_mode_count, present_mode_list.data());
    assert(ret == VK_SUCCESS);
    for (auto mode : present_mode_list)
    {
        std::cout << mode << std::endl;
    }

    std::cout << "cap width: " << surf_cap.currentExtent.width << std::endl;
    std::cout << "cap height: " << surf_cap.currentExtent.height << std::endl;
    VkExtent2D swapchain_extent;
    swapchain_extent = surf_cap.currentExtent;

    VkPresentModeKHR swapchain_present_mode = VK_PRESENT_MODE_FIFO_KHR;

    uint32_t desire_number_of_swapchain_images = surf_cap.minImageCount;
    std::cout << "surface min image count: " << desire_number_of_swapchain_images << std::endl;

    VkSurfaceTransformFlagBitsKHR pre_trans;
    if (surf_cap.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        pre_trans = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        pre_trans = surf_cap.currentTransform;
    }
    std::cout << "cur trans: " << surf_cap.currentTransform << std::endl;
    std::cout << "pre trans: " << pre_trans << std::endl;

    // find a supported composite alpha mode - one of these is guaranteed to be set
    VkCompositeAlphaFlagBitsKHR composite_aplha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR composite_aplha_flags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (uint32_t i = 0; i < 4; i++)
    {
        if (surf_cap.supportedCompositeAlpha & composite_aplha_flags[i])
        {
            composite_aplha = composite_aplha_flags[i];
            break;
        }
    }

    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.pNext = nullptr;
    create_info.surface = m_surface;
    create_info.minImageCount = desire_number_of_swapchain_images;
    create_info.imageFormat = m_format;
    create_info.imageExtent.width = swapchain_extent.width;
    create_info.imageExtent.height = swapchain_extent.height;
    create_info.preTransform = pre_trans;
    create_info.compositeAlpha = composite_aplha;
    create_info.imageArrayLayers = 1;
    create_info.presentMode = swapchain_present_mode;
    create_info.oldSwapchain = VK_NULL_HANDLE;
    create_info.clipped = true;
    create_info.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;

    ret = vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swapchain);
    assert(ret == VK_SUCCESS);

    ret = vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_swapchain_image_count, nullptr);
    assert(ret == VK_SUCCESS);
    std::cout << "swapchain image count: " << m_swapchain_image_count << std::endl;

    m_swapchain_image_list.resize(m_swapchain_image_count);
    ret = vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_swapchain_image_count, m_swapchain_image_list.data());
    assert(ret == VK_SUCCESS);

    m_swapchain_image_view_list.resize(m_swapchain_image_count);
    for (uint32_t i = 0; i < m_swapchain_image_count; i++)
    {
        VkImageViewCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.pNext = nullptr;
        create_info.flags = 0;
        create_info.image = m_swapchain_image_list[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = m_format;
        create_info.components.r = VK_COMPONENT_SWIZZLE_R;
        create_info.components.g = VK_COMPONENT_SWIZZLE_G;
        create_info.components.b = VK_COMPONENT_SWIZZLE_B;
        create_info.components.a = VK_COMPONENT_SWIZZLE_A;
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        ret = vkCreateImageView(m_device, &create_info, nullptr, &m_swapchain_image_view_list.at(i));
        assert(ret == VK_SUCCESS);
    }
}

void VulkanContext::create_depth_buffer()
{
    VkImageCreateInfo create_info = {};
    VkFormat depth_format = VK_FORMAT_D16_UNORM;

    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(m_device_list[0], depth_format, &props);
    if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        create_info.tiling = VK_IMAGE_TILING_LINEAR;
    } else if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    }
    else
    {
        std::cout << "VK_FORMAT_D16_UNORM Unsupported.\n";
        exit(-1);
    }

    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.imageType = VK_IMAGE_TYPE_2D;
    create_info.format = depth_format;
    create_info.extent.width = m_width;
    create_info.extent.height = m_height;
    create_info.extent.depth = 1;
    create_info.mipLevels = 1;
    create_info.arrayLayers = 1;
    create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.flags = 0;

    auto ret = vkCreateImage(m_device, &create_info, nullptr, &m_depth_image);
    assert(ret == VK_SUCCESS);

    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(m_device, m_depth_image, &mem_reqs);

    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext = nullptr;
    mem_alloc.allocationSize = mem_reqs.size;
    mem_alloc.memoryTypeIndex = 0;

    ret = vkAllocateMemory(m_device, &mem_alloc, nullptr, &m_depth_image_mem);
    assert(ret == VK_SUCCESS);

    ret = vkBindImageMemory(m_device, m_depth_image, m_depth_image_mem, 0);
    assert(ret == VK_SUCCESS);

    VkImageViewCreateInfo view_ci = {};
    view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_ci.pNext = nullptr;
    view_ci.image = m_depth_image;
    view_ci.format = depth_format;
    view_ci.components.r = VK_COMPONENT_SWIZZLE_R;
    view_ci.components.g = VK_COMPONENT_SWIZZLE_G;
    view_ci.components.b = VK_COMPONENT_SWIZZLE_B;
    view_ci.components.a = VK_COMPONENT_SWIZZLE_A;
    view_ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_ci.subresourceRange.baseMipLevel = 0;
    view_ci.subresourceRange.levelCount = 1;
    view_ci.subresourceRange.baseArrayLayer = 0;
    view_ci.subresourceRange.layerCount = 1;
    view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_ci.flags = 0;

    ret = vkCreateImageView(m_device, &view_ci, nullptr, &m_depth_image_view);
    assert(ret == VK_SUCCESS);
}

void VulkanContext::create_uniform_buffer()
{
    VkBufferCreateInfo buf_ci = {};
    buf_ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_ci.pNext = nullptr;
    buf_ci.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buf_ci.size = sizeof()
}

void VulkanContext::set_window(HINSTANCE instance, HWND hwnd, int width, int height)
{
    m_win_instance = instance;
    m_hwnd = hwnd;
    m_width = width;
    m_height = height;
}
void VulkanContext::set_enable_layers(const std::vector<const char *> &layers)
{
    m_enable_layers = layers;
}

void VulkanContext::set_enable_extensions(const std::vector<const char *> &extensions)
{
    m_enable_extensions = extensions;
}

void VulkanContext::set_dev_enable_extensions(const std::vector<const char *> &extensions)
{
    m_dev_extensions = extensions;
}

void VulkanRenderer::init()
{

}


















}
