//
// Copyright (c) 2012-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// ShaderExecutable9.cpp: Implements a D3D9-specific class to contain shader
// executable implementation details.

#include "libANGLE/renderer/d3d/d3d9/ShaderExecutable9.h"

#include "common/debug.h"
#include "libANGLE/renderer/d3d/d3d9/Renderer9.h"

namespace rx
{

ShaderExecutable9::ShaderExecutable9(Renderer9 *renderer, const uint8_t *function, size_t length)
    : ShaderExecutable(function, length),
      mRenderer(renderer),
      mPixelExecutable(nullptr),
      mVertexExecutable(nullptr)
{
}

ShaderExecutable9::~ShaderExecutable9()
{
    SafeRelease(mVertexExecutable);
    SafeRelease(mPixelExecutable);
}

ShaderExecutable9 *ShaderExecutable9::makeShaderExecutable9(ShaderExecutable *executable)
{
    ASSERT(HAS_DYNAMIC_TYPE(ShaderExecutable9*, executable));
    return static_cast<ShaderExecutable9*>(executable);
}

IDirect3DVertexShader9 *ShaderExecutable9::getVertexShader() const
{
    return mVertexExecutable;
}

IDirect3DPixelShader9 *ShaderExecutable9::getPixelShader() const
{
    return mPixelExecutable;
}

gl::Error ShaderExecutable9::finishLoad(ShaderType type,
                                        const std::vector<gl::LinkedVarying> &transformFeedbackVaryings,
                                        bool separatedOutputBuffers)
{
    // Transform feedback is not supported in ES2 or D3D9
    ASSERT(transformFeedbackVaryings.size() == 0);

    const DWORD *dwordFunction = reinterpret_cast<const DWORD *>(getFunction());

    switch (type)
    {
      case SHADER_VERTEX:
        {
            IDirect3DVertexShader9 *vshader = NULL;
            gl::Error error = mRenderer->createVertexShader(dwordFunction, getLength(), &vshader);
            if (error.isError())
            {
                return error;
            }

            mVertexExecutable = vshader;
        }
        break;
      case SHADER_PIXEL:
        {
            IDirect3DPixelShader9 *pshader = NULL;
            gl::Error error = mRenderer->createPixelShader(dwordFunction, getLength(), &pshader);
            if (error.isError())
            {
                return error;
            }

            mPixelExecutable = pshader;
        }
        break;
      default:
        UNREACHABLE();
        return gl::Error(GL_INVALID_OPERATION);
    }

    return gl::Error(GL_NO_ERROR);
}

ShaderExecutable *Renderer9::createShaderExecutable(const uint8_t *function, size_t length)
{
    return new ShaderExecutable9(this, function, length);
}

}
