//
// Copyright 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// VertexArray11.h: Defines the rx::VertexArray11 class which implements rx::VertexArrayImpl.

#ifndef LIBANGLE_RENDERER_D3D_D3D11_VERTEXARRAY11_H_
#define LIBANGLE_RENDERER_D3D_D3D11_VERTEXARRAY11_H_

#include "libANGLE/renderer/VertexArrayImpl.h"
#include "libANGLE/renderer/d3d/d3d11/Renderer11.h"

namespace rx
{
class Renderer11;

class VertexArray11 : public VertexArrayImpl
{
  public:
    VertexArray11(const gl::VertexArray::Data &data, Renderer11 *renderer);
    ~VertexArray11() override;

    void syncState(const gl::VertexArray::DirtyBits &dirtyBits) override;

  private:
    void updateVertexAttribStorage(size_t attribIndex);

    std::vector<size_t> *getStorageIndexList(VertexStorageType storageType);

    Renderer11 *mRenderer;
    std::vector<VertexStorageType> mAttributeStorageTypes;
};

}  // namespace rx

#endif // LIBANGLE_RENDERER_D3D_D3D11_VERTEXARRAY11_H_
