//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// renderervk_utils:
//    Helper functions for the Vulkan Renderer.
//

#include <vulkan/vulkan.h>

#include "libANGLE/Error.h"

namespace rx
{

const char *VulkanResultString(VkResult result);
egl::Error VulkanEGLError(EGLint errorCode, EGLint extendedErrorCode, VkResult result);

#define ANGLE_VK_EGL_RESULT_CHECK(result, errorCode, extendedCode) \
    if (result)                                                    \
    {                                                              \
        return VulkanEGLError(errorCode, extendedCode, result);    \
    }

}  // namespace rx
