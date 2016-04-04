//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// DisplayVk.h:
//    Defines the class interface for DisplayVk.
//

#ifndef LIBANGLE_RENDERER_VULKAN_DISPLAYVK_H_
#define LIBANGLE_RENDERER_VULKAN_DISPLAYVK_H_

#include "libANGLE/renderer/DisplayImpl.h"

namespace rx
{

class DisplayVk : public DisplayImpl
{
  public:
    DisplayVk();
    ~DisplayVk() override;

    egl::Error initialize(egl::Display *display) override;
    void terminate() override;

    SurfaceImpl *createWindowSurface(const egl::Config *configuration,
                                     EGLNativeWindowType window,
                                     const egl::AttributeMap &attribs) override;
    SurfaceImpl *createPbufferSurface(const egl::Config *configuration,
                                      const egl::AttributeMap &attribs) override;
    SurfaceImpl *createPbufferFromClientBuffer(const egl::Config *configuration,
                                               EGLClientBuffer shareHandle,
                                               const egl::AttributeMap &attribs) override;
    SurfaceImpl *createPixmapSurface(const egl::Config *configuration,
                                     NativePixmapType nativePixmap,
                                     const egl::AttributeMap &attribs) override;

    ImageImpl *createImage(EGLenum target,
                           egl::ImageSibling *buffer,
                           const egl::AttributeMap &attribs) override;

    gl::Context *createContext(const egl::Config *config,
                               const gl::Context *shareContext,
                               const egl::AttributeMap &attribs) override;

    StreamImpl *createStream(const egl::AttributeMap &attribs) override;

    egl::Error makeCurrent(egl::Surface *drawSurface,
                           egl::Surface *readSurface,
                           gl::Context *context) override;

    egl::ConfigSet generateConfigs() const override;

    bool isDeviceLost() const override;
    bool testDeviceLost() override;
    egl::Error restoreLostDevice() override;

    bool isValidNativeWindow(EGLNativeWindowType window) const override;

    std::string getVendorString() const override;

    egl::Error getDevice(DeviceImpl **device) override;

    egl::Error waitClient() const override;
    egl::Error waitNative(EGLint engine,
                          egl::Surface *drawSurface,
                          egl::Surface *readSurface) const override;

  private:
    void generateExtensions(egl::DisplayExtensions *outExtensions) const override;
    void generateCaps(egl::Caps *outCaps) const override;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_VULKAN_DISPLAYVK_H_
