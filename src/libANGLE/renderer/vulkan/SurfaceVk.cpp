//
// Copyright 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// SurfaceVk.cpp: Implements the class methods for SurfaceVk.

#include "libANGLE/renderer/vulkan/SurfaceVk.h"

#include "common/debug.h"
#include "libANGLE/renderer/vulkan/RendererVk.h"

namespace rx
{

OffscreenSurfaceVk::OffscreenSurfaceVk(RendererVk *renderer, EGLint width, EGLint height)
    : mRenderer(renderer), mWidth(width), mHeight(height)
{
}

OffscreenSurfaceVk::~OffscreenSurfaceVk()
{
}

egl::Error OffscreenSurfaceVk::initialize()
{
    return egl::Error(EGL_SUCCESS);
}

FramebufferImpl *OffscreenSurfaceVk::createDefaultFramebuffer(const gl::Framebuffer::Data &data)
{
    return mRenderer->createFramebuffer(data);
}

egl::Error OffscreenSurfaceVk::swap()
{
    return egl::Error(EGL_SUCCESS);
}

egl::Error OffscreenSurfaceVk::postSubBuffer(EGLint /*x*/,
                                             EGLint /*y*/,
                                             EGLint /*width*/,
                                             EGLint /*height*/)
{
    return egl::Error(EGL_SUCCESS);
}

egl::Error OffscreenSurfaceVk::querySurfacePointerANGLE(EGLint /*attribute*/, void ** /*value*/)
{
    UNREACHABLE();
    return egl::Error(EGL_BAD_CURRENT_SURFACE);
}

egl::Error OffscreenSurfaceVk::bindTexImage(gl::Texture * /*texture*/, EGLint /*buffer*/)
{
    return egl::Error(EGL_SUCCESS);
}

egl::Error OffscreenSurfaceVk::releaseTexImage(EGLint /*buffer*/)
{
    return egl::Error(EGL_SUCCESS);
}

void OffscreenSurfaceVk::setSwapInterval(EGLint /*interval*/)
{
}

EGLint OffscreenSurfaceVk::getWidth() const
{
    return mWidth;
}

EGLint OffscreenSurfaceVk::getHeight() const
{
    return mHeight;
}

EGLint OffscreenSurfaceVk::isPostSubBufferSupported() const
{
    return EGL_FALSE;
}

EGLint OffscreenSurfaceVk::getSwapBehavior() const
{
    return EGL_BUFFER_PRESERVED;
}

gl::Error OffscreenSurfaceVk::getAttachmentRenderTarget(
    const gl::FramebufferAttachment::Target & /*target*/,
    FramebufferAttachmentRenderTarget ** /*rtOut*/)
{
    UNREACHABLE();
    return gl::Error(GL_INVALID_OPERATION);
}

WindowSurfaceVk::WindowSurfaceVk(RendererVk *renderer, EGLNativeWindowType window)
    : mRenderer(renderer)
{
}

WindowSurfaceVk::~WindowSurfaceVk()
{
}

egl::Error WindowSurfaceVk::initialize()
{
    // TODO(jmadill)
    return egl::Error(EGL_SUCCESS);
}

FramebufferImpl *WindowSurfaceVk::createDefaultFramebuffer(const gl::Framebuffer::Data &data)
{
    return mRenderer->createFramebuffer(data);
}

egl::Error WindowSurfaceVk::swap()
{
    // TODO(jmadill)
    return egl::Error(EGL_SUCCESS);
}

egl::Error WindowSurfaceVk::postSubBuffer(EGLint x, EGLint y, EGLint width, EGLint height)
{
    // TODO(jmadill)
    return egl::Error(EGL_SUCCESS);
}

egl::Error WindowSurfaceVk::querySurfacePointerANGLE(EGLint attribute, void **value)
{
    UNREACHABLE();
    return egl::Error(EGL_BAD_CURRENT_SURFACE);
}

egl::Error WindowSurfaceVk::bindTexImage(gl::Texture *texture, EGLint buffer)
{
    return egl::Error(EGL_SUCCESS);
}

egl::Error WindowSurfaceVk::releaseTexImage(EGLint buffer)
{
    return egl::Error(EGL_SUCCESS);
}

void WindowSurfaceVk::setSwapInterval(EGLint interval)
{
}

EGLint WindowSurfaceVk::getWidth() const
{
    // TODO(jmadill)
    return 0;
}

EGLint WindowSurfaceVk::getHeight() const
{
    // TODO(jmadill)
    return 0;
}

EGLint WindowSurfaceVk::isPostSubBufferSupported() const
{
    // TODO(jmadill)
    return EGL_FALSE;
}

EGLint WindowSurfaceVk::getSwapBehavior() const
{
    // TODO(jmadill)
    return EGL_BUFFER_DESTROYED;
}

gl::Error WindowSurfaceVk::getAttachmentRenderTarget(
    const gl::FramebufferAttachment::Target &target,
    FramebufferAttachmentRenderTarget **rtOut)
{
    UNREACHABLE();
    return gl::Error(GL_INVALID_OPERATION);
}

}  // namespace rx
