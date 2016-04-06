//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// DisplayVk.cpp:
//    Implements the class methods for DisplayVk.
//

#include "libANGLE/renderer/vulkan/DisplayVk.h"

#include "common/debug.h"
#include "libANGLE/Context.h"
#include "libANGLE/Display.h"
#include "libANGLE/renderer/vulkan/RendererVk.h"
#include "libANGLE/renderer/vulkan/SurfaceVk.h"

namespace rx
{

DisplayVk::DisplayVk() : DisplayImpl()
{
}

DisplayVk::~DisplayVk()
{
}

egl::Error DisplayVk::initialize(egl::Display *display)
{
    ASSERT(!mRenderer && display != nullptr);
    mRenderer.reset(new RendererVk());
    return mRenderer->initialize(display->getAttributeMap());
}

void DisplayVk::terminate()
{
    mRenderer.reset(nullptr);
}

SurfaceImpl *DisplayVk::createWindowSurface(const egl::Config *configuration,
                                            EGLNativeWindowType window,
                                            const egl::AttributeMap &attribs)
{
    return new WindowSurfaceVk(mRenderer.get(), window);
}

SurfaceImpl *DisplayVk::createPbufferSurface(const egl::Config *configuration,
                                             const egl::AttributeMap &attribs)
{
    ASSERT(mRenderer);

    EGLint width  = attribs.getAsInt(EGL_WIDTH, 0);
    EGLint height = attribs.getAsInt(EGL_HEIGHT, 0);

    return new OffscreenSurfaceVk(mRenderer.get(), width, height);
}

SurfaceImpl *DisplayVk::createPbufferFromClientBuffer(const egl::Config *configuration,
                                                      EGLClientBuffer shareHandle,
                                                      const egl::AttributeMap &attribs)
{
    UNIMPLEMENTED();
    return static_cast<SurfaceImpl *>(0);
}

SurfaceImpl *DisplayVk::createPixmapSurface(const egl::Config *configuration,
                                            NativePixmapType nativePixmap,
                                            const egl::AttributeMap &attribs)
{
    UNIMPLEMENTED();
    return static_cast<SurfaceImpl *>(0);
}

ImageImpl *DisplayVk::createImage(EGLenum target,
                                  egl::ImageSibling *buffer,
                                  const egl::AttributeMap &attribs)
{
    UNIMPLEMENTED();
    return static_cast<ImageImpl *>(0);
}

gl::Context *DisplayVk::createContext(const egl::Config *config,
                                      const gl::Context *shareContext,
                                      const egl::AttributeMap &attribs)
{
    ASSERT(mRenderer);
    return new gl::Context(config, shareContext, mRenderer.get(), attribs);
}

StreamImpl *DisplayVk::createStream(const egl::AttributeMap &attribs)
{
    UNIMPLEMENTED();
    return static_cast<StreamImpl *>(0);
}

egl::Error DisplayVk::makeCurrent(egl::Surface *drawSurface,
                                  egl::Surface *readSurface,
                                  gl::Context *context)
{
    return egl::Error(EGL_SUCCESS);
}

egl::ConfigSet DisplayVk::generateConfigs() const
{
    // TODO(jmadill): Multiple configs, and proper checking of config attribs.
    egl::Config singleton;
    singleton.renderTargetFormat    = GL_RGBA8;
    singleton.depthStencilFormat    = GL_DEPTH24_STENCIL8;
    singleton.bufferSize            = 32;
    singleton.redSize               = 8;
    singleton.greenSize             = 8;
    singleton.blueSize              = 8;
    singleton.alphaSize             = 8;
    singleton.alphaMaskSize         = 0;
    singleton.bindToTextureRGB      = false;
    singleton.bindToTextureRGBA     = false;
    singleton.colorBufferType       = EGL_RGB_BUFFER;
    singleton.configCaveat          = EGL_NONE;
    singleton.conformant            = 0;
    singleton.depthSize             = 24;
    singleton.level                 = 0;
    singleton.matchNativePixmap     = EGL_NONE;
    singleton.maxPBufferWidth       = 0;
    singleton.maxPBufferHeight      = 0;
    singleton.maxPBufferPixels      = 0;
    singleton.maxSwapInterval       = 1;
    singleton.minSwapInterval       = 1;
    singleton.nativeRenderable      = EGL_TRUE;
    singleton.nativeVisualID        = 0;
    singleton.nativeVisualType      = EGL_NONE;
    singleton.renderableType        = EGL_OPENGL_ES2_BIT;
    singleton.sampleBuffers         = 0;
    singleton.samples               = 0;
    singleton.surfaceType           = EGL_WINDOW_BIT;
    singleton.optimalOrientation    = 0;
    singleton.transparentType       = EGL_NONE;
    singleton.transparentRedValue   = 0;
    singleton.transparentGreenValue = 0;
    singleton.transparentBlueValue  = 0;

    egl::ConfigSet configSet;
    configSet.add(singleton);
    return configSet;
}

bool DisplayVk::isDeviceLost() const
{
    // TODO(jmadill): Figure out how to do device lost in Vulkan.
    return false;
}

bool DisplayVk::testDeviceLost()
{
    // TODO(jmadill): Figure out how to do device lost in Vulkan.
    return false;
}

egl::Error DisplayVk::restoreLostDevice()
{
    UNIMPLEMENTED();
    return egl::Error(EGL_BAD_ACCESS);
}

bool DisplayVk::isValidNativeWindow(EGLNativeWindowType window) const
{
    // TODO(jmadill): Cross-platform this.
    return (IsWindow(window) == TRUE);
}

std::string DisplayVk::getVendorString() const
{
    // TODO(jmadill): Determine GPU vendor from Renderer.
    return std::string("Google Inc.");
}

egl::Error DisplayVk::getDevice(DeviceImpl **device)
{
    UNIMPLEMENTED();
    return egl::Error(EGL_BAD_ACCESS);
}

egl::Error DisplayVk::waitClient() const
{
    UNIMPLEMENTED();
    return egl::Error(EGL_BAD_ACCESS);
}

egl::Error DisplayVk::waitNative(EGLint engine,
                                 egl::Surface *drawSurface,
                                 egl::Surface *readSurface) const
{
    UNIMPLEMENTED();
    return egl::Error(EGL_BAD_ACCESS);
}

void DisplayVk::generateExtensions(egl::DisplayExtensions *outExtensions) const
{
}

void DisplayVk::generateCaps(egl::Caps *outCaps) const
{
    outCaps->textureNPOT = true;
}

}  // namespace rx
