//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// VertexArray11:
//   Implementation of rx::VertexArray11.
//

#include "libANGLE/renderer/d3d/d3d11/VertexArray11.h"

#include "common/BitSetIterator.h"

namespace rx
{

namespace
{
size_t GetAttribIndex(unsigned long dirtyBit)
{
    if (dirtyBit >= gl::VertexArray::DIRTY_BIT_ATTRIB_0_ENABLED &&
        dirtyBit < gl::VertexArray::DIRTY_BIT_ATTRIB_MAX_ENABLED)
    {
        return dirtyBit - gl::VertexArray::DIRTY_BIT_ATTRIB_0_ENABLED;
    }

    if (dirtyBit >= gl::VertexArray::DIRTY_BIT_ATTRIB_0_POINTER &&
        dirtyBit < gl::VertexArray::DIRTY_BIT_ATTRIB_MAX_POINTER)
    {
        return dirtyBit - gl::VertexArray::DIRTY_BIT_ATTRIB_0_POINTER;
    }

    ASSERT(dirtyBit >= gl::VertexArray::DIRTY_BIT_ATTRIB_0_DIVISOR &&
           dirtyBit < gl::VertexArray::DIRTY_BIT_ATTRIB_MAX_DIVISOR);
    return static_cast<size_t>(dirtyBit) - gl::VertexArray::DIRTY_BIT_ATTRIB_0_DIVISOR;
}
}  // anonymous namespace

VertexArray11::VertexArray11(const gl::VertexArray::Data &data, Renderer11 *renderer)
    : VertexArrayImpl(data),
      mRenderer(renderer),
      mAttributeStorageTypes(data.getVertexAttributes().size(),
                             VertexStorageType::VERTEX_STORAGE_UNKNOWN)
{
}

VertexArray11::~VertexArray11()
{
}

void VertexArray11::syncState(const gl::VertexArray::DirtyBits &dirtyBits)
{
    for (auto dirtyBit : angle::IterateBitSet(dirtyBits))
    {
        if (dirtyBit == gl::VertexArray::DIRTY_BIT_ELEMENT_ARRAY_BUFFER)
            continue;

        size_t attribIndex = GetAttribIndex(dirtyBit);
        updateVertexAttribStorage(attribIndex);
    }
}

void VertexArray11::updateVertexAttribStorage(size_t attribIndex)
{
    const auto &attrib = mData.getVertexAttribute(attribIndex);

    auto newStorageType = ClassifyAttributeStorage(attrib);
    auto oldStorageType = mAttributeStorageTypes[attribIndex];

    if (newStorageType == oldStorageType)
        return;

    auto *oldStorageList = getStorageIndexList(oldStorageType);
    if (oldStorageList)
    {
        oldStorageList->erase(
            std::remove(oldStorageList->begin(), oldStorageList->end(), attribIndex));
    }

    mAttributeStorageTypes[attribIndex] = newStorageType;

    auto *newStorageList = getStorageIndexList(newStorageType);
    if (newStorageList)
    {
        newStorageList->push_back(attribIndex);
    }

    // We assume if we're syncing state and updating storage, this is the bound VAO.
    mRenderer->getStateManager()->invalidateInputLayout();
}

std::vector<size_t> *VertexArray11::getStorageIndexList(VertexStorageType storageType)
{
    switch (storageType)
    {
        case VertexStorageType::VERTEX_STORAGE_CURRENT_VALUE:
            return &mCurrentValueAttribIndexes;
        case VertexStorageType::VERTEX_STORAGE_DYNAMIC:
            return &mDynamicAttribIndexes;
        case VertexStorageType::VERTEX_STORAGE_STATIC:
            return &mStaticAttribIndexes;
        default:
            return nullptr;
    }
}

}  // namespace rx
