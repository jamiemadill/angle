//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// DisplayImpl.h: Implementation methods of egl::Display

#ifndef LIBEGL_DISPLAYIMPL_H_
#define LIBEGL_DISPLAYIMPL_H_

#include "libEGL/Error.h"
#include "common/angleutils.h"

#include <set>

namespace egl
{
class Display;
class Config;
class Surface;
class SurfaceImpl;

class DisplayImpl
{
  public:
    DisplayImpl() {}
    virtual ~DisplayImpl();

    virtual SurfaceImpl *createWindowSurface(Display *display, const Config *config,
                                             EGLNativeWindowType window, EGLint fixedSize,
                                             EGLint width, EGLint height, EGLint postSubBufferSupported) = 0;
    virtual SurfaceImpl *createOffscreenSurface(Display *display, const Config *config,
                                                EGLClientBuffer shareHandle, EGLint width, EGLint height,
                                                EGLenum textureFormat, EGLenum textureTarget) = 0;
    virtual Error restoreLostDevice() = 0;

    typedef std::set<Surface*> SurfaceSet;
    const SurfaceSet &getSurfaceSet() const { return mSurfaceSet; }
    SurfaceSet &getSurfaceSet() { return mSurfaceSet; }

    void destroySurface(Surface *surface);

  protected:
    SurfaceSet mSurfaceSet;

  private:
    DISALLOW_COPY_AND_ASSIGN(DisplayImpl);
};

}

#endif // LIBEGL_DISPLAYIMPL_H_
