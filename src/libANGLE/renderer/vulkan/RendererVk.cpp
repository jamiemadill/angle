//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// RendererVk.cpp:
//    Implements the class methods for RendererVk.
//

#include "libANGLE/renderer/vulkan/RendererVk.h"

#include "common/debug.h"

namespace rx
{

RendererVk::RendererVk() : Renderer(), mInstance(nullptr)
{
}

RendererVk::~RendererVk()
{
    vkDestroyInstance(mInstance, nullptr);
}

egl::Error RendererVk::initialize()
{
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

    // TODO(jmadill): Layers and extensions.
    instanceInfo.enabledExtensionCount   = 0;
    instanceInfo.ppEnabledExtensionNames = nullptr;
    instanceInfo.enabledLayerCount       = 0;
    instanceInfo.ppEnabledLayerNames     = nullptr;

    VkResult result = VK_SUCCESS;
    result = vkCreateInstance(&instanceInfo, nullptr, &mInstance);
    if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
    {
        return egl::Error(EGL_NOT_INITIALIZED, VULKAN_INIT_INCOMPATIBLE_DRIVER,
                          "Could not find a compatible Vulkan driver.");
    }

    return egl::Error(EGL_SUCCESS);
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
    UNIMPLEMENTED();
    return std::string();
}

std::string RendererVk::getRendererDescription() const
{
    UNIMPLEMENTED();
    return std::string();
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

void RendererVk::syncState(const gl::State &state, const gl::State::DirtyBits &dirtyBits)
{
    UNIMPLEMENTED();
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
    UNIMPLEMENTED();
}

CompilerImpl *RendererVk::createCompiler()
{
    UNIMPLEMENTED();
    return static_cast<CompilerImpl *>(0);
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
    UNIMPLEMENTED();
    return static_cast<FramebufferImpl *>(0);
}

TextureImpl *RendererVk::createTexture(GLenum target)
{
    UNIMPLEMENTED();
    return static_cast<TextureImpl *>(0);
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
    UNIMPLEMENTED();
    return static_cast<VertexArrayImpl *>(0);
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
    UNIMPLEMENTED();
}

}  // namespace rx
