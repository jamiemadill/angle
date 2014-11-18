//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// DisplayImpl.cpp: Implementation methods of egl::Display

#include "libEGL/DisplayImpl.h"
#include "libEGL/Surface.h"

namespace egl
{

DisplayImpl::~DisplayImpl()
{
    while (!mSurfaceSet.empty())
    {
        destroySurface(*mSurfaceSet.begin());
    }
}

void DisplayImpl::destroySurface(Surface *surface)
{
    delete surface;
    mSurfaceSet.erase(surface);
}

}
