//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// RendererVk.cpp:
//    Implements the class methods for RendererVk.
//

#include "libANGLE/renderer/vulkan/RendererVk.h"

#include <EGL/eglext.h>

#include "common/debug.h"
#include "libANGLE/renderer/vulkan/CompilerVk.h"
#include "libANGLE/renderer/vulkan/FramebufferVk.h"
#include "libANGLE/renderer/vulkan/TextureVk.h"
#include "libANGLE/renderer/vulkan/VertexArrayVk.h"
#include "libANGLE/renderer/vulkan/renderervk_utils.h"
#include "platform/Platform.h"

#include "libANGLE/renderer/vulkan/BufferVk.h"
#include "libANGLE/renderer/vulkan/CompilerVk.h"
#include "libANGLE/renderer/vulkan/DeviceVk.h"
#include "libANGLE/renderer/vulkan/FenceNVVk.h"
#include "libANGLE/renderer/vulkan/FenceSyncVk.h"
#include "libANGLE/renderer/vulkan/FramebufferVk.h"
#include "libANGLE/renderer/vulkan/ImageVk.h"
#include "libANGLE/renderer/vulkan/ProgramVk.h"
#include "libANGLE/renderer/vulkan/QueryVk.h"
#include "libANGLE/renderer/vulkan/RenderbufferVk.h"
#include "libANGLE/renderer/vulkan/SamplerVk.h"
#include "libANGLE/renderer/vulkan/ShaderVk.h"
#include "libANGLE/renderer/vulkan/TextureVk.h"
#include "libANGLE/renderer/vulkan/TransformFeedbackVk.h"
#include "libANGLE/renderer/vulkan/VertexArrayVk.h"

namespace rx
{

namespace
{

bool VerifyLayerNameList(const std::vector<VkLayerProperties> &layerProps,
                         const std::vector<const char *> &enabledLayerNames)
{
    // Compile the layer names into a set.
    std::set<std::string> layerNames;
    for (const auto &layerProp : layerProps)
    {
        layerNames.insert(layerProp.layerName);
    }

    // Verify the validation layers are in layer names set.
    for (const auto &instanceLayerName : enabledLayerNames)
    {
        if (layerNames.count(instanceLayerName) == 0)
        {
            return false;
        }
    }

    return true;
}

bool VerifyExtensionNameList(const std::vector<VkExtensionProperties> &extensionProps,
                             const std::vector<const char *> &enabledExtensionNames)
{
    // Compile the extensions names into a set.
    std::set<std::string> extensionNames;
    for (const auto &extensionProp : extensionProps)
    {
        extensionNames.insert(extensionProp.extensionName);
    }

    for (const auto &extensionName : enabledExtensionNames)
    {
        if (extensionNames.count(extensionName) == 0)
        {
            return false;
        }
    }

    return true;
}

VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags,
                                        VkDebugReportObjectTypeEXT objectType,
                                        uint64_t object,
                                        size_t location,
                                        int32_t messageCode,
                                        const char *layerPrefix,
                                        const char *message,
                                        void *userData)
{
    if ((flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0)
    {
        ANGLEPlatformCurrent()->logError(message);
    }
    else if ((flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0)
    {
        ANGLEPlatformCurrent()->logWarning(message);
    }
    else
    {
        ANGLEPlatformCurrent()->logInfo(message);
    }

    return VK_FALSE;
}

}  // anonymous namespace

RendererVk::RendererVk()
    : Renderer(),
      mInstance(VK_NULL_HANDLE),
      mPhysicalDevice(VK_NULL_HANDLE),
      mQueue(VK_NULL_HANDLE),
      mCurrentQueueFamilyIndex(std::numeric_limits<uint32_t>::max()),
      mDevice(VK_NULL_HANDLE),
      mEnableValidationLayers(false),
      mDebugReportCallback(VK_NULL_HANDLE)
{
}

RendererVk::~RendererVk()
{
    // TODO(jmadill): Submit fix for Loader which should allow destroying NULL
    if (mDebugReportCallback)
    {
        ASSERT(mInstance);
        auto destroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
            vkGetInstanceProcAddr(mInstance, "vkDestroyDebugReportCallbackEXT"));
        ASSERT(destroyDebugReportCallback);
        destroyDebugReportCallback(mInstance, mDebugReportCallback, nullptr);
    }

    if (mDevice)
    {
        vkDestroyDevice(mDevice, nullptr);
    }

    if (mInstance)
    {
        vkDestroyInstance(mInstance, nullptr);
    }
}

#define ANGLE_VK_INIT_RESULT_CHECK(result, code) \
    ANGLE_VK_EGL_RESULT_CHECK(result, EGL_NOT_INITIALIZED, code)

egl::Error RendererVk::initialize(const egl::AttributeMap &attribs)
{
    VkResult result = VK_SUCCESS;

    // Gather global layer properties.
    uint32_t instanceLayerCount = 0;
    result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
    ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_INIT_LAYERS);

    std::vector<VkLayerProperties> instanceLayerProps(instanceLayerCount);
    if (instanceLayerCount > 0)
    {
        result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProps.data());
        ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_INIT_LAYERS);
    }

    uint32_t instanceExtensionCount = 0;
    result = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_INIT_EXTENSIONS);

    std::vector<VkExtensionProperties> instanceExtensionProps(instanceExtensionCount);
    if (instanceExtensionCount > 0)
    {
        result = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount,
                                                        instanceExtensionProps.data());
        ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_INIT_EXTENSIONS);
    }

// Validation layers enabled by default in Debug, disabled in Release.
#if !defined(NDEBUG)
    mEnableValidationLayers = true;
#endif

    // If specified in the attributes, override the default.
    if (attribs.contains(EGL_PLATFORM_ANGLE_ENABLE_VALIDATION_LAYER_ANGLE))
    {
        mEnableValidationLayers =
            (attribs.get(EGL_PLATFORM_ANGLE_ENABLE_VALIDATION_LAYER_ANGLE, EGL_FALSE) == EGL_TRUE);
    }

    std::vector<const char *> enabledInstanceLayers;

    if (mEnableValidationLayers)
    {
        getValidationLayers(&enabledInstanceLayers);

        // Verify the validation layers are in layer names set.
        if (!VerifyLayerNameList(instanceLayerProps, enabledInstanceLayers))
        {
            ANGLEPlatformCurrent()->logWarning("A standard Vulkan validation layer is missing.");
            mEnableValidationLayers = false;
            enabledInstanceLayers.clear();
        }
    }

    std::vector<const char *> enabledInstanceExtensions;
    enabledInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(ANGLE_PLATFORM_WINDOWS)
    enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
#error Unsupported Vulkan platform.
#endif  // defined(ANGLE_PLATFORM_WINDOWS)

    // TODO(jmadill): Should be able to continue initialization if debug report ext missing.
    if (mEnableValidationLayers)
    {
        enabledInstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }

    // Verify the required extensions are in the extension names set. Fail if not.
    if (!VerifyExtensionNameList(instanceExtensionProps, enabledInstanceExtensions))
    {
        return VulkanEGLError(EGL_NOT_INITIALIZED, VULKAN_ERROR_INIT_EXTENSIONS,
                              VK_ERROR_EXTENSION_NOT_PRESENT);
    }

    VkApplicationInfo applicationInfo  = {};
    applicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext              = nullptr;
    applicationInfo.pApplicationName   = "ANGLE";
    applicationInfo.applicationVersion = 1;
    applicationInfo.pEngineName        = "ANGLE";
    applicationInfo.engineVersion      = 1;
    applicationInfo.apiVersion         = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext                = nullptr;
    instanceInfo.flags                = 0;
    instanceInfo.pApplicationInfo     = &applicationInfo;

    // Enable requested layers and extensions.
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(enabledInstanceExtensions.size());
    instanceInfo.ppEnabledExtensionNames =
        enabledInstanceExtensions.empty() ? nullptr : enabledInstanceExtensions.data();
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(enabledInstanceLayers.size());
    instanceInfo.ppEnabledLayerNames =
        enabledInstanceLayers.empty() ? nullptr : enabledInstanceLayers.data();

    result = vkCreateInstance(&instanceInfo, nullptr, &mInstance);
    ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_CREATE_INSTANCE);

    if (mEnableValidationLayers)
    {
        VkDebugReportCallbackCreateInfoEXT debugReportInfo = {};

        debugReportInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        debugReportInfo.pNext = nullptr;
        debugReportInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
                                VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                                VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
        debugReportInfo.pfnCallback = &DebugReportCallback;
        debugReportInfo.pUserData   = this;

        auto createDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
            vkGetInstanceProcAddr(mInstance, "vkCreateDebugReportCallbackEXT"));
        ASSERT(createDebugReportCallback);
        result =
            createDebugReportCallback(mInstance, &debugReportInfo, nullptr, &mDebugReportCallback);
        ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_INIT_EXTENSIONS);
    }

    uint32_t physicalDeviceCount = 0;
    result = vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr);
    ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_QUERY_PHYSICAL_DEVICE);

    if (physicalDeviceCount == 0)
    {
        return egl::Error(EGL_NOT_INITIALIZED, VULKAN_ERROR_QUERY_PHYSICAL_DEVICE,
                          "No Physical Devices found");
    }

    // TODO(jmadill): Handle multiple physical devices. For now, use the first device.
    physicalDeviceCount = 1;
    result = vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, &mPhysicalDevice);
    ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_QUERY_PHYSICAL_DEVICE);

    vkGetPhysicalDeviceProperties(mPhysicalDevice, &mPhysicalDeviceProperties);

    // Ensure we can find a graphics queue family.
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueCount, nullptr);

    if (queueCount == 0)
    {
        return egl::Error(EGL_NOT_INITIALIZED, VULKAN_ERROR_INIT_QUEUE,
                          "No Queues found on Physical Device");
    }

    mQueueFamilyProperties.resize(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueCount,
                                             mQueueFamilyProperties.data());

    size_t graphicsQueueFamilyCount   = false;
    uint32_t firstGraphicsQueueFamily = 0;
    for (uint32_t queueIndex = 0; queueIndex < queueCount; ++queueIndex)
    {
        const auto &queueInfo = mQueueFamilyProperties[queueIndex];
        if ((queueInfo.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            ASSERT(queueInfo.queueCount > 0);
            graphicsQueueFamilyCount++;
            if (firstGraphicsQueueFamily == 0)
            {
                firstGraphicsQueueFamily = queueIndex;
            }
            break;
        }
    }

    if (graphicsQueueFamilyCount == 0)
    {
        return egl::Error(EGL_NOT_INITIALIZED, VULKAN_ERROR_INIT_QUEUE,
                          "No Graphics Queues found on Physical Device");
    }

    // If only one queue family, go ahead and initialize the device. If there is more than one
    // queue, we'll have to wait until we see a WindowSurface to know which supports present.
    if (graphicsQueueFamilyCount == 1)
    {
        initializeDevice(firstGraphicsQueueFamily);
    }

    return egl::Error(EGL_SUCCESS);
}

void RendererVk::getValidationLayers(std::vector<const char *> *layersOut) const
{
    // Use the standard validation layer ordering as in the Vulkan SDK docs.
    if (mEnableValidationLayers)
    {
        //layersOut->push_back("VK_LAYER_LUNARG_threading");
        //layersOut->push_back("VK_LAYER_LUNARG_param_checker");
        //layersOut->push_back("VK_LAYER_LUNARG_device_limits");
        //layersOut->push_back("VK_LAYER_LUNARG_object_tracker");
        //layersOut->push_back("VK_LAYER_LUNARG_image");
        //layersOut->push_back("VK_LAYER_LUNARG_mem_tracker");
        //layersOut->push_back("VK_LAYER_LUNARG_draw_state");
        //layersOut->push_back("VK_LAYER_LUNARG_swapchain");
        layersOut->push_back("VK_LAYER_GOOGLE_unique_objects");
    }
}

egl::Error RendererVk::initializeDevice(uint32_t queueFamilyIndex)
{
    VkResult result = VK_SUCCESS;

    uint32_t deviceLayerCount = 0;
    result = vkEnumerateDeviceLayerProperties(mPhysicalDevice, &deviceLayerCount, nullptr);
    ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_INIT_LAYERS);

    std::vector<VkLayerProperties> deviceLayerProps(deviceLayerCount);
    if (deviceLayerCount > 0)
    {
        result = vkEnumerateDeviceLayerProperties(mPhysicalDevice, &deviceLayerCount,
                                                  deviceLayerProps.data());
        ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_INIT_LAYERS);
    }

    uint32_t deviceExtensionCount = 0;
    result = vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &deviceExtensionCount,
                                                  nullptr);
    ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_INIT_EXTENSIONS);

    std::vector<VkExtensionProperties> deviceExtensionProps(deviceExtensionCount);
    if (deviceExtensionCount > 0)
    {
        result = vkEnumerateDeviceExtensionProperties(
            mPhysicalDevice, nullptr, &deviceExtensionCount, deviceExtensionProps.data());
        ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_INIT_EXTENSIONS);
    }

    std::vector<const char *> enabledDeviceLayers;
    if (mEnableValidationLayers)
    {
        getValidationLayers(&enabledDeviceLayers);

        if (!VerifyLayerNameList(deviceLayerProps, enabledDeviceLayers))
        {
            ANGLEPlatformCurrent()->logWarning("A standard Vulkan validation layer is missing.");
            enabledDeviceLayers.clear();
        }
    }

    std::vector<const char *> enabledDeviceExtensions;
    enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    if (!VerifyExtensionNameList(deviceExtensionProps, enabledDeviceExtensions))
    {
        return VulkanEGLError(EGL_NOT_INITIALIZED, VULKAN_ERROR_INIT_EXTENSIONS,
                              VK_ERROR_EXTENSION_NOT_PRESENT);
    }

    VkDeviceQueueCreateInfo queueCreateInfo = {};

    float zeroPriority = 0.0f;

    queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext            = nullptr;
    queueCreateInfo.flags            = 0;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount       = 1;
    queueCreateInfo.pQueuePriorities = &zeroPriority;

    // Initialize the device
    VkDeviceCreateInfo createInfo = {};

    createInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext                = nullptr;
    createInfo.flags                = 0;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos    = &queueCreateInfo;
    createInfo.enabledLayerCount = static_cast<uint32_t>(enabledDeviceLayers.size());
    createInfo.ppEnabledLayerNames =
        enabledDeviceLayers.empty() ? nullptr : enabledDeviceLayers.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames =
        enabledDeviceExtensions.empty() ? nullptr : enabledDeviceExtensions.data();
    createInfo.pEnabledFeatures = nullptr;  // TODO(jmadill): features

    result = vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice);
    ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_CREATE_DEVICE);

    mCurrentQueueFamilyIndex = queueFamilyIndex;

    return egl::Error(EGL_SUCCESS);
}

egl::Error RendererVk::selectGraphicsQueue()
{
    uint32_t queueCount = static_cast<uint32_t>(mQueueFamilyProperties.size());
    for (uint32_t queueIndex = 0; queueIndex < queueCount; ++queueIndex)
    {
        const auto &queueInfo = mQueueFamilyProperties[queueIndex];
        if ((queueInfo.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            egl::Error error = initializeDevice(queueIndex);
            if (error.isError())
            {
                return egl::Error(EGL_BAD_SURFACE, error.getID(), error.getMessage());
            }
            return egl::Error(EGL_SUCCESS);
        }
    }

    UNREACHABLE();
    return egl::Error(EGL_BAD_SURFACE, "Unreachable code internal error.");
}

egl::ErrorOrResult<bool> RendererVk::selectPresentQueueForSurface(VkSurfaceKHR surface)
{
    VkResult result = VK_SUCCESS;

    // We've already initialized a device, and can't re-create it unless it's never been used.
    // TODO(jmadill): Handle the re-creation case if necessary.
    if (mDevice != VK_NULL_HANDLE)
    {
        ASSERT(mCurrentQueueFamilyIndex != std::numeric_limits<uint32_t>::max());

        // Check if the current device supports present on this surface.
        VkBool32 supportsPresent = VK_FALSE;
        result = vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, mCurrentQueueFamilyIndex,
                                                      surface, &supportsPresent);
        ANGLE_VK_EGL_RESULT_CHECK(result, EGL_BAD_SURFACE, VULKAN_ERROR_INIT_SURFACE);

        return (supportsPresent == VK_TRUE);
    }

    // Find a graphics and present queue.
    Optional<uint32_t> newPresentQueue;
    uint32_t queueCount = static_cast<uint32_t>(mQueueFamilyProperties.size());
    for (uint32_t queueIndex = 0; queueIndex < queueCount; ++queueIndex)
    {
        const auto &queueInfo = mQueueFamilyProperties[queueIndex];
        if ((queueInfo.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            VkBool32 supportsPresent = VK_FALSE;
            result = vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, queueIndex, surface,
                                                          &supportsPresent);
            ANGLE_VK_EGL_RESULT_CHECK(result, EGL_BAD_SURFACE, VULKAN_ERROR_INIT_SURFACE);

            if (supportsPresent == VK_TRUE)
            {
                newPresentQueue = queueIndex;
                break;
            }
        }
    }

    if (!newPresentQueue.valid())
    {
        // We failed to find a queue that supports present.
        return false;
    }

    egl::Error error = initializeDevice(newPresentQueue.value());
    if (error.isError())
    {
        return egl::Error(EGL_BAD_SURFACE, error.getID(), error.getMessage());
    }

    return true;
}

gl::Error RendererVk::flush()
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

gl::Error RendererVk::finish()
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

gl::Error RendererVk::drawArrays(const gl::Data &data, GLenum mode, GLint first, GLsizei count)
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

gl::Error RendererVk::drawArraysInstanced(const gl::Data &data,
                                          GLenum mode,
                                          GLint first,
                                          GLsizei count,
                                          GLsizei instanceCount)
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

gl::Error RendererVk::drawElements(const gl::Data &data,
                                   GLenum mode,
                                   GLsizei count,
                                   GLenum type,
                                   const GLvoid *indices,
                                   const gl::IndexRange &indexRange)
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

gl::Error RendererVk::drawElementsInstanced(const gl::Data &data,
                                            GLenum mode,
                                            GLsizei count,
                                            GLenum type,
                                            const GLvoid *indices,
                                            GLsizei instances,
                                            const gl::IndexRange &indexRange)
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

gl::Error RendererVk::drawRangeElements(const gl::Data &data,
                                        GLenum mode,
                                        GLuint start,
                                        GLuint end,
                                        GLsizei count,
                                        GLenum type,
                                        const GLvoid *indices,
                                        const gl::IndexRange &indexRange)
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

void RendererVk::notifyDeviceLost()
{
    UNIMPLEMENTED();
}

bool RendererVk::isDeviceLost() const
{
    UNIMPLEMENTED();
    return bool();
}

bool RendererVk::testDeviceLost()
{
    UNIMPLEMENTED();
    return bool();
}

bool RendererVk::testDeviceResettable()
{
    UNIMPLEMENTED();
    return bool();
}

std::string RendererVk::getVendorString() const
{
    switch (mPhysicalDeviceProperties.vendorID)
    {
        case VENDOR_ID_AMD:
            return "Advanced Micro Devices";
        case VENDOR_ID_NVIDIA:
            return "NVIDIA";
        case VENDOR_ID_INTEL:
            return "Intel";
        default:
        {
            // TODO(jmadill): More vendor IDs.
            std::stringstream strstr;
            strstr << "Vendor ID: " << mPhysicalDeviceProperties.vendorID;
            return strstr.str();
        }
    }
}

std::string RendererVk::getRendererDescription() const
{
    std::stringstream strstr;

    uint32_t apiVersion = mPhysicalDeviceProperties.apiVersion;

    strstr << "Vulkan ";
    strstr << VK_VERSION_MAJOR(apiVersion) << ".";
    strstr << VK_VERSION_MINOR(apiVersion) << ".";
    strstr << VK_VERSION_PATCH(apiVersion);

    strstr << "(" << mPhysicalDeviceProperties.deviceName << ")";

    return strstr.str();
}

void RendererVk::insertEventMarker(GLsizei length, const char *marker)
{
    UNIMPLEMENTED();
}

void RendererVk::pushGroupMarker(GLsizei length, const char *marker)
{
    UNIMPLEMENTED();
}

void RendererVk::popGroupMarker()
{
    UNIMPLEMENTED();
}

void RendererVk::syncState(const gl::State & /*state*/, const gl::State::DirtyBits & /*dirtyBits*/)
{
}

GLint RendererVk::getGPUDisjoint()
{
    UNIMPLEMENTED();
    return GLint();
}

GLint64 RendererVk::getTimestamp()
{
    UNIMPLEMENTED();
    return GLint64();
}

void RendererVk::onMakeCurrent(const gl::Data &data)
{
}

CompilerImpl *RendererVk::createCompiler()
{
    return new CompilerVk();
}

ShaderImpl *RendererVk::createShader(const gl::Shader::Data &data)
{
    return new ShaderVk(data);
}

ProgramImpl *RendererVk::createProgram(const gl::Program::Data &data)
{
    return new ProgramVk(data);
}

FramebufferImpl *RendererVk::createFramebuffer(const gl::Framebuffer::Data &data)
{
    return new FramebufferVk(data);
}

TextureImpl *RendererVk::createTexture(GLenum /*target*/)
{
    return new TextureVk();
}

RenderbufferImpl *RendererVk::createRenderbuffer()
{
    return new RenderbufferVk();
}

BufferImpl *RendererVk::createBuffer()
{
    return new BufferVk();
}

VertexArrayImpl *RendererVk::createVertexArray(const gl::VertexArray::Data &data)
{
    return new VertexArrayVk(data);
}

QueryImpl *RendererVk::createQuery(GLenum type)
{
    return new QueryVk(type);
}

FenceNVImpl *RendererVk::createFenceNV()
{
    return new FenceNVVk();
}

FenceSyncImpl *RendererVk::createFenceSync()
{
    return new FenceSyncVk();
}

TransformFeedbackImpl *RendererVk::createTransformFeedback()
{
    return new TransformFeedbackVk();
}

SamplerImpl *RendererVk::createSampler()
{
    return new SamplerVk();
}

void RendererVk::generateCaps(gl::Caps *outCaps,
                              gl::TextureCapsMap *outTextureCaps,
                              gl::Extensions *outExtensions,
                              gl::Limitations *outLimitations) const
{
    // TODO(jmadill): Caps.
}

}  // namespace rx
