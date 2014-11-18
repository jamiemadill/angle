//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// DisplayD3D.h: D3D implementation of egl::Display

#ifndef LIBEGL_DISPLAYD3D_H_
#define LIBEGL_DISPLAYD3D_H_

#include "libEGL/DisplayImpl.h"

namespace rx
{
class RendererD3D;
}

namespace egl
{

class DisplayD3D : public DisplayImpl
{
  public:
    DisplayD3D(rx::RendererD3D *renderer) {}
    SurfaceImpl *createWindowSurface(Display *display, const Config *config,
                                     EGLNativeWindowType window, EGLint fixedSize,
                                     EGLint width, EGLint height, EGLint postSubBufferSupported) override;
    SurfaceImpl *createOffscreenSurface(Display *display, const Config *config,
                                        EGLClientBuffer shareHandle, EGLint width, EGLint height,
                                        EGLenum textureFormat, EGLenum textureTarget) override;
    Error restoreLostDevice() override;

  private:
    DISALLOW_COPY_AND_ASSIGN(DisplayD3D);

    rx::RendererD3D *mRenderer;
};

}

#endif // LIBEGL_DISPLAYD3D_H_
