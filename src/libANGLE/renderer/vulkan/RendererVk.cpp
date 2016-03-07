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

RendererVk::RendererVk() : Renderer(), mInstance(VK_NULL_HANDLE)
{
}

RendererVk::~RendererVk()
{
    vkDestroyInstance(mInstance, nullptr);
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
    result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProps.data());
    ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_INIT_LAYERS);

    uint32_t instanceExtensionCount = 0;
    result = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_INIT_EXTENSIONS);

    std::vector<VkExtensionProperties> instanceExtensionProps(instanceExtensionCount);
    result = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount,
                                                    instanceExtensionProps.data());
    ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_INIT_EXTENSIONS);

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
            return VulkanEGLError(EGL_NOT_INITIALIZED, VULKAN_ERROR_INIT_EXTENSIONS,
                                  VK_ERROR_EXTENSION_NOT_PRESENT);
        }
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
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensionNames.size());
    instanceInfo.ppEnabledExtensionNames =
        instanceExtensionNames.empty() ? nullptr : instanceExtensionNames.data();
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayerNames.size());
    instanceInfo.ppEnabledLayerNames =
        instanceLayerNames.empty() ? nullptr : instanceLayerNames.data();

    result = vkCreateInstance(&instanceInfo, nullptr, &mInstance);
    ANGLE_VK_INIT_RESULT_CHECK(result, VULKAN_ERROR_CREATE_INSTANCE);

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
