//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FramebufferVk.cpp:
//    Implements the class methods for FramebufferVk.
//

#include "libANGLE/renderer/vulkan/FramebufferVk.h"

#include "common/debug.h"
#include "libANGLE/formatutils.h"

namespace rx
{

FramebufferVk::FramebufferVk(const gl::Framebuffer::Data &data) : FramebufferImpl(data)
{
}

FramebufferVk::~FramebufferVk()
{
}

gl::Error FramebufferVk::discard(size_t count, const GLenum *attachments)
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

gl::Error FramebufferVk::invalidate(size_t count, const GLenum *attachments)
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

gl::Error FramebufferVk::invalidateSub(size_t count,
                                       const GLenum *attachments,
                                       const gl::Rectangle &area)
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

gl::Error FramebufferVk::clear(const gl::Data &data, GLbitfield mask)
{
    // TODO(jmadill): Implement clear.
    return gl::Error(GL_NO_ERROR);
}

gl::Error FramebufferVk::clearBufferfv(const gl::Data &data,
                                       GLenum buffer,
                                       GLint drawbuffer,
                                       const GLfloat *values)
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

gl::Error FramebufferVk::clearBufferuiv(const gl::Data &data,
                                        GLenum buffer,
                                        GLint drawbuffer,
                                        const GLuint *values)
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

gl::Error FramebufferVk::clearBufferiv(const gl::Data &data,
                                       GLenum buffer,
                                       GLint drawbuffer,
                                       const GLint *values)
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

gl::Error FramebufferVk::clearBufferfi(const gl::Data &data,
                                       GLenum buffer,
                                       GLint drawbuffer,
                                       GLfloat depth,
                                       GLint stencil)
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

GLenum FramebufferVk::getImplementationColorReadFormat() const
{
    // TODO(jmadill): Do proper check.
    const gl::FramebufferAttachment *readAttachment = getData().getReadAttachment();
    GLenum internalFormat = readAttachment->getInternalFormat();
    const auto &internalFormatInfo = gl::GetInternalFormatInfo(internalFormat);
    return internalFormatInfo.format;
}

GLenum FramebufferVk::getImplementationColorReadType() const
{
    // TODO(jmadill): Do proper check.
    const gl::FramebufferAttachment *readAttachment = getData().getReadAttachment();
    GLenum internalFormat = readAttachment->getInternalFormat();
    const auto &internalFormatInfo = gl::GetInternalFormatInfo(internalFormat);
    return internalFormatInfo.type;
}

gl::Error FramebufferVk::readPixels(const gl::State &state,
                                    const gl::Rectangle &area,
                                    GLenum format,
                                    GLenum type,
                                    GLvoid *pixels) const
{
    // TODO(jmadill): ReadPixels
    return gl::Error(GL_NO_ERROR);
}

gl::Error FramebufferVk::blit(const gl::State &state,
                              const gl::Rectangle &sourceArea,
                              const gl::Rectangle &destArea,
                              GLbitfield mask,
                              GLenum filter,
                              const gl::Framebuffer *sourceFramebuffer)
{
    UNIMPLEMENTED();
    return gl::Error(GL_INVALID_OPERATION);
}

bool FramebufferVk::checkStatus() const
{
    UNIMPLEMENTED();
    return bool();
}

void FramebufferVk::syncState(const gl::Framebuffer::DirtyBits &dirtyBits)
{
}

}  // namespace rx
