//
// Copyright (c) 2012-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// ShaderExecutable9.h: Defines a D3D9-specific class to contain shader
// executable implementation details.

#ifndef LIBANGLE_RENDERER_D3D_D3D9_SHADEREXECUTABLE9_H_
#define LIBANGLE_RENDERER_D3D_D3D9_SHADEREXECUTABLE9_H_

#include "libANGLE/renderer/ShaderExecutable.h"

namespace rx
{

class ShaderExecutable9 : public ShaderExecutable
{
  public:
    ShaderExecutable9(Renderer9 *renderer, const uint8_t *function, size_t length);
    virtual ~ShaderExecutable9();

    static ShaderExecutable9 *makeShaderExecutable9(ShaderExecutable *executable);

    IDirect3DPixelShader9 *getPixelShader() const;
    IDirect3DVertexShader9 *getVertexShader() const;

    gl::Error ShaderExecutable9::finishLoad(ShaderType type,
                                            const std::vector<gl::LinkedVarying> &transformFeedbackVaryings,
                                            bool separatedOutputBuffers) override;

  private:
    DISALLOW_COPY_AND_ASSIGN(ShaderExecutable9);

    Renderer9 *mRenderer;
    IDirect3DPixelShader9 *mPixelExecutable;
    IDirect3DVertexShader9 *mVertexExecutable;
};

}

#endif // LIBANGLE_RENDERER_D3D_D3D9_SHADEREXECUTABLE9_H_