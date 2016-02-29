//
// Copyright 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// BufferD3D.cpp Defines common functionality between the Buffer9 and Buffer11 classes.

#include "libANGLE/renderer/d3d/BufferD3D.h"

#include "common/mathutil.h"
#include "common/utilities.h"
#include "libANGLE/renderer/d3d/IndexBuffer.h"
#include "libANGLE/renderer/d3d/VertexBuffer.h"

namespace rx
{

unsigned int BufferD3D::mNextSerial = 1;

BufferD3D::BufferD3D(BufferFactoryD3D *factory)
    : BufferImpl(),
      mFactory(factory),
      mStaticIndexBuffer(nullptr),
      mStaticBufferCacheTotalSize(0),
      mStaticVertexBufferOutOfDate(false),
      mUnmodifiedDataUse(0),
      mUsage(D3DBufferUsage::STATIC)
{
    updateSerial();
}

BufferD3D::~BufferD3D()
{
    SafeDelete(mStaticIndexBuffer);
}

void BufferD3D::emptyStaticBufferCache()
{
    mStaticVertexBuffers.clear();
    mStaticBufferCacheTotalSize = 0;
}

void BufferD3D::updateSerial()
{
    mSerial = mNextSerial++;
}

void BufferD3D::updateD3DBufferUsage(GLenum usage)
{
    switch (usage)
    {
        case GL_STATIC_DRAW:
        case GL_STATIC_READ:
        case GL_STATIC_COPY:
            mUsage = D3DBufferUsage::STATIC;
            initializeStaticData();
            break;

        case GL_STREAM_DRAW:
        case GL_STREAM_READ:
        case GL_STREAM_COPY:
        case GL_DYNAMIC_READ:
        case GL_DYNAMIC_COPY:
        case GL_DYNAMIC_DRAW:
            mUsage = D3DBufferUsage::DYNAMIC;
            break;
        default:
            UNREACHABLE();
    }
}

void BufferD3D::initializeStaticData()
{
    if (mStaticVertexBuffers.empty())
    {
        mStaticVertexBuffers.push_back(std::make_unique<StaticVertexBufferInterface>(mFactory));
    }
    if (!mStaticIndexBuffer)
    {
        mStaticIndexBuffer = new StaticIndexBufferInterface(mFactory);
    }
}

StaticIndexBufferInterface *BufferD3D::getStaticIndexBuffer()
{
    return mStaticIndexBuffer;
}

StaticVertexBufferInterface *BufferD3D::getStaticVertexBuffer(const gl::VertexAttribute &attribute)
{
    if (mStaticVertexBuffers.empty())
    {
        // Early out if there aren't any static buffers at all
        return nullptr;
    }

    // Early out, the attribute can be added to mStaticVertexBuffer.
    if (mStaticVertexBuffers.size() == 1 && mStaticVertexBuffers[0]->empty())
    {
        return mStaticVertexBuffers[0].get();
    }

    // At this point, see if any of the existing static buffers contains the attribute data
    // If there is a cached static buffer that already contains the attribute, then return it
    for (const auto &staticBuffer : mStaticVertexBuffers)
    {
        if (staticBuffer->matchesAttribute(attribute))
        {
            return staticBuffer.get();
        }
    }

    // At this point, we must create a new static buffer for the attribute data
    // TODO(jmadill): Cache size limiting.
    mStaticVertexBuffers.push_back(std::make_unique<StaticVertexBufferInterface>(mFactory));
    return mStaticVertexBuffers.back().get();
}

void BufferD3D::invalidateStaticData()
{
    emptyStaticBufferCache();

    if (mStaticIndexBuffer && mStaticIndexBuffer->getBufferSize() != 0)
    {
        SafeDelete(mStaticIndexBuffer);
    }

    // If the buffer was created with a static usage then we recreate the static
    // buffers so that they are populated the next time we use this buffer.
    if (mUsage == D3DBufferUsage::STATIC)
    {
        initializeStaticData();
    }

    mUnmodifiedDataUse = 0;
}

// Creates static buffers if sufficient used data has been left unmodified
void BufferD3D::promoteStaticUsage(int dataSize)
{
    if (mStaticVertexBuffers.empty() && !mStaticIndexBuffer)
    {
        mUnmodifiedDataUse += dataSize;

        if (mUnmodifiedDataUse > 3 * getSize())
        {
            initializeStaticData();
        }
    }
}

gl::Error BufferD3D::getIndexRange(GLenum type,
                                   size_t offset,
                                   size_t count,
                                   bool primitiveRestartEnabled,
                                   gl::IndexRange *outRange)
{
    const uint8_t *data = nullptr;
    gl::Error error = getData(&data);
    if (error.isError())
    {
        return error;
    }

    *outRange = gl::ComputeIndexRange(type, data + offset, count, primitiveRestartEnabled);
    return gl::Error(GL_NO_ERROR);
}

}  // namespace rx
