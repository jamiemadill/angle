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

RendererVk::RendererVk() : Renderer()
{
}

RendererVk::~RendererVk()
{
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
    UNIMPLEMENTED();
}

}  // namespace rx
