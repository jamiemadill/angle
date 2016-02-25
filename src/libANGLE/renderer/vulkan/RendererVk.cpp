//
// Copyright 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// RendererVk.cpp: Implements the class methods for RendererVk.

#include "libANGLE/renderer/vulkan/RendererVk.h"

#include <EGL/eglext.h>

#include "common/debug.h"
#include "libANGLE/renderer/vulkan/CompilerVk.h"
#include "libANGLE/renderer/vulkan/FramebufferVk.h"
#include "libANGLE/renderer/vulkan/TextureVk.h"
#include "libANGLE/renderer/vulkan/VertexArrayVk.h"
#include "platform/Platform.h"

namespace rx
{

namespace
{
const char *VulkanResultString(VkResult result)
{
    switch (result)
    {
        case VK_SUCCESS:
            return "Command successfully completed.";
        case VK_NOT_READY:
            return "A fence or query has not yet completed.";
        case VK_TIMEOUT:
            return "A wait operation has not completed in the specified time.";
        case VK_EVENT_SET:
            return "An event is signaled.";
        case VK_EVENT_RESET:
            return "An event is unsignaled.";
        case VK_INCOMPLETE:
            return "A return array was too small for the result.";
        case VK_SUBOPTIMAL_KHR:
            return "A swapchain no longer matches the surface properties exactly, but can still be "
                   "used to present to the surface successfully.";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "A host memory allocation has failed.";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "A device memory allocation has failed.";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "Initialization of an object could not be completed for implementation-specific "
                   "reasons.";
        case VK_ERROR_DEVICE_LOST:
            return "The logical or physical device has been lost.";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "Mapping of a memory object has failed.";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "A requested layer is not present or could not be loaded.";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "A requested extension is not supported.";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "A requested feature is not supported.";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "The requested version of Vulkan is not supported by the driver or is otherwise "
                   "incompatible for implementation - specific reasons.";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "Too many objects of the type have already been created.";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "A requested format is not supported on this device.";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "A surface is no longer available.";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "The requested window is already connected to a VkSurfaceKHR, or to some other "
                   "non-Vulkan API.";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "A surface has changed in such a way that it is no longer compatible with the "
                   "swapchain.";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "The display used by a swapchain does not use the same presentable image "
                   "layout, or is incompatible in a way that prevents sharing an image.";
        default:
            return "Unknown vulkan error code.";
    }
}

egl::Error VulkanInitError(VkResult result, EGLint extendedErrorCode)
{
    ASSERT(result != VK_SUCCESS);
    return egl::Error(EGL_NOT_INITIALIZED, extendedErrorCode, VulkanResultString(result));
}
}  // anonymous namespace

RendererVk::RendererVk() : Renderer(), mInstance(nullptr)
{
}

RendererVk::~RendererVk()
{
    vkDestroyInstance(mInstance, nullptr);
}

#define VK_INIT_RESULT_CHECK(result, code)    \
    if (result)                               \
    {                                         \
        return VulkanInitError(result, code); \
    }

egl::Error RendererVk::initialize(const egl::AttributeMap &attribs)
{
    VkResult result = VK_SUCCESS;

    // Gather global layer properties.
    uint32_t instanceLayerCount = 0;
    result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
    VK_INIT_RESULT_CHECK(result, VULKAN_INIT_ERROR_LAYERS);

    std::vector<VkLayerProperties> instanceLayerProps(instanceLayerCount);
    result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProps.data());
    VK_INIT_RESULT_CHECK(result, VULKAN_INIT_ERROR_LAYERS);

    uint32_t instanceExtensionCount = 0;
    result = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    VK_INIT_RESULT_CHECK(result, VULKAN_INIT_ERROR_EXTENSIONS);

    std::vector<VkExtensionProperties> instanceExtensionProps(instanceExtensionCount);
    result = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount,
                                                    instanceExtensionProps.data());
    VK_INIT_RESULT_CHECK(result, VULKAN_INIT_ERROR_EXTENSIONS);

    // Compile the layer names into a set.
    std::set<std::string> layerNames;
    for (const auto &layerProp : instanceLayerProps)
    {
        layerNames.insert(layerProp.layerName);
    }

    // Similarly for the extensions.
    std::set<std::string> extensionNames;
    for (const auto &extensionProp : instanceExtensionProps)
    {
        extensionNames.insert(extensionProp.extensionName);
    }

#if !defined(NDEBUG)
    // Validation layers enabled by default in Debug.
    bool enableValidationLayers = true;
#else
    bool enableValidationLayers = false;
#endif

    // If specified in the attributes, override the default.
    if (attribs.contains(EGL_PLATFORM_ANGLE_ENABLE_VALIDATION_LAYER_ANGLE))
    {
        enableValidationLayers =
            (attribs.get(EGL_PLATFORM_ANGLE_ENABLE_VALIDATION_LAYER_ANGLE, EGL_FALSE) == EGL_TRUE);
    }

    std::vector<const char *> instanceLayerNames;

    // Use the standard validation layer ordering as in the Vulkan SDK docs.
    if (enableValidationLayers)
    {
        instanceLayerNames.push_back("VK_LAYER_LUNARG_threading");
        instanceLayerNames.push_back("VK_LAYER_LUNARG_param_checker");
        instanceLayerNames.push_back("VK_LAYER_LUNARG_device_limits");
        instanceLayerNames.push_back("VK_LAYER_LUNARG_object_tracker");
        instanceLayerNames.push_back("VK_LAYER_LUNARG_image");
        instanceLayerNames.push_back("VK_LAYER_LUNARG_mem_tracker");
        instanceLayerNames.push_back("VK_LAYER_LUNARG_draw_state");
        instanceLayerNames.push_back("VK_LAYER_LUNARG_swapchain");
        instanceLayerNames.push_back("VK_LAYER_GOOGLE_unique_objects");
    }

    // Verify the validation layers are in layer names set.
    for (const auto &instanceLayerName : instanceLayerNames)
    {
        if (layerNames.count(instanceLayerName) == 0)
        {
            ANGLEPlatformCurrent()->logWarning("Enabled Vulkan instance layer missing.");
            instanceLayerNames.clear();
            break;
        }
    }

    std::vector<const char *> instanceExtensionNames;
    instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(ANGLE_PLATFORM_WINDOWS)
    instanceExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
#error Unsupported Vulkan platform.
#endif  // defined(ANGLE_PLATFORM_WINDOWS)

    // Verify the required extensions are in the extension names set. Fail if not.
    for (const auto &extensionName : instanceExtensionNames)
    {
        if (extensionNames.count(extensionName) == 0)
        {
            return VulkanInitError(VK_ERROR_EXTENSION_NOT_PRESENT, VULKAN_INIT_ERROR_EXTENSIONS);
        }
    }

    VkApplicationInfo applicationInfo  = {};
    applicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext              = nullptr;
    applicationInfo.pApplicationName   = "ANGLE";
    applicationInfo.applicationVersion = 1;
    applicationInfo.pEngineName        = "ANGLE";
    applicationInfo.engineVersion      = 1;
    applicationInfo.apiVersion         = VK_API_VERSION;

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext                = nullptr;
    instanceInfo.flags                = 0;
    instanceInfo.pApplicationInfo     = &applicationInfo;

    // Enable requested layers and extensions.
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensionNames.size());
    instanceInfo.ppEnabledExtensionNames =
        instanceExtensionNames.empty() ? nullptr : instanceExtensionNames.data();
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayerNames.size());
    instanceInfo.ppEnabledLayerNames =
        instanceLayerNames.empty() ? nullptr : instanceLayerNames.data();

    result = vkCreateInstance(&instanceInfo, nullptr, &mInstance);
    VK_INIT_RESULT_CHECK(result, VULKAN_INIT_ERROR_CREATE_INSTANCE);

    return egl::Error(EGL_SUCCESS);
}

#undef VK_INIT_RESULT_CHECK

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
    UNIMPLEMENTED();
    return std::string();
}

std::string RendererVk::getRendererDescription() const
{
    // TODO(jmadill): Description.
    return "Vulkan";
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
    UNIMPLEMENTED();
    return static_cast<ShaderImpl *>(0);
}

ProgramImpl *RendererVk::createProgram(const gl::Program::Data &data)
{
    UNIMPLEMENTED();
    return static_cast<ProgramImpl *>(0);
}

FramebufferImpl *RendererVk::createFramebuffer(const gl::Framebuffer::Data &data)
{
    return new FramebufferVk(data);
}

TextureImpl *RendererVk::createTexture(GLenum target)
{
    return new TextureVk();
}

RenderbufferImpl *RendererVk::createRenderbuffer()
{
    UNIMPLEMENTED();
    return static_cast<RenderbufferImpl *>(0);
}

BufferImpl *RendererVk::createBuffer()
{
    UNIMPLEMENTED();
    return static_cast<BufferImpl *>(0);
}

VertexArrayImpl *RendererVk::createVertexArray(const gl::VertexArray::Data &data)
{
    return new VertexArrayVk(data);
}

QueryImpl *RendererVk::createQuery(GLenum type)
{
    UNIMPLEMENTED();
    return static_cast<QueryImpl *>(0);
}

FenceNVImpl *RendererVk::createFenceNV()
{
    UNIMPLEMENTED();
    return static_cast<FenceNVImpl *>(0);
}

FenceSyncImpl *RendererVk::createFenceSync()
{
    UNIMPLEMENTED();
    return static_cast<FenceSyncImpl *>(0);
}

TransformFeedbackImpl *RendererVk::createTransformFeedback()
{
    UNIMPLEMENTED();
    return static_cast<TransformFeedbackImpl *>(0);
}

SamplerImpl *RendererVk::createSampler()
{
    UNIMPLEMENTED();
    return static_cast<SamplerImpl *>(0);
}

void RendererVk::generateCaps(gl::Caps *outCaps,
                              gl::TextureCapsMap *outTextureCaps,
                              gl::Extensions *outExtensions,
                              gl::Limitations *outLimitations) const
{
    // TODO(jmadill): Caps.
}

}  // namespace rx
