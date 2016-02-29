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
    gl::Error updateDirtyAndDynamicAttribs(VertexDataManager *vertexDataManager,
                                           const gl::State &state,
                                           GLint start,
                                           GLsizei count,
                                           GLsizei instances);

    const std::vector<TranslatedAttribute> &getTranslatedAttribs() const;

  private:
    void updateVertexAttribStorage(size_t attribIndex);
    void markBufferDataDirty(size_t attribIndex);

    Renderer11 *mRenderer;
    std::vector<VertexStorageType> mAttributeStorageTypes;
    std::vector<TranslatedAttribute> mTranslatedAttribs;
    std::vector<size_t> mDynamicAttribIndexes;
    std::vector<size_t> mDirtyAttribIndexes;

    // We need to keep a safe pointer to the Buffer so we can attach the correct dirty callbacks.
    std::vector<BindingPointer<gl::Buffer>> mCurrentBuffers;

    std::vector<NotificationCallback> mOnBufferDataDirty;
};

}  // namespace rx

#endif // LIBANGLE_RENDERER_D3D_D3D11_VERTEXARRAY11_H_
