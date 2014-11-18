//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// DisplayD3D.cpp: D3D implementation of egl::Display

#include "libEGL/DisplayD3D.h"
#include "libEGL/SurfaceD3D.h"
#include "libGLESv2/renderer/SwapChain.h"
#include "libGLESv2/renderer/d3d/RendererD3D.h"

namespace egl
{

SurfaceImpl *DisplayD3D::createWindowSurface(Display *display, const Config *config,
                                             EGLNativeWindowType window, EGLint fixedSize,
                                             EGLint width, EGLint height, EGLint postSubBufferSupported)
{
    return SurfaceD3D::createFromWindow(display, config, window, fixedSize,
                                        width, height, postSubBufferSupported);
}

SurfaceImpl *DisplayD3D::createOffscreenSurface(Display *display, const Config *config,
                                                EGLClientBuffer shareHandle, EGLint width, EGLint height,
                                                EGLenum textureFormat, EGLenum textureTarget)
{
    return SurfaceD3D::createOffscreen(display, config, shareHandle,
                                       width, height, textureFormat, textureTarget);
}

Error DisplayD3D::restoreLostDevice()
{
    // Release surface resources to make the Reset() succeed
    for (const auto &surface : mSurfaceSet)
    {
        SurfaceD3D *surfaceD3D = SurfaceD3D::makeSurfaceD3D(surface);
        surfaceD3D->release();
    }

    if (!mRenderer->resetDevice())
    {
        return Error(EGL_BAD_ALLOC);
    }

    // Restore any surfaces that may have been lost
    for (const auto &surface : mSurfaceSet)
    {
        SurfaceD3D *surfaceD3D = SurfaceD3D::makeSurfaceD3D(surface);

        Error error = surfaceD3D->resetSwapChain();
        if (error.isError())
        {
            return error;
        }
    }

    return Error(EGL_SUCCESS);
}

}
