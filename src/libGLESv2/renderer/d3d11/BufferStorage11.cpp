#include "precompiled.h"
//
// Copyright (c) 2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// BufferStorage11.cpp Defines the BufferStorage11 class.

#include "libGLESv2/renderer/d3d11/BufferStorage11.h"
#include "libGLESv2/main.h"
#include "libGLESv2/renderer/d3d11/Renderer11.h"
#include "libGLESv2/renderer/d3d11/formatutils11.h"
#include "libGLESv2/Buffer.h"

namespace rx
{

PackPixelsParams::PackPixelsParams()
    : mFormat(GL_NONE),
      mType(GL_NONE),
      mOutputPitch(0),
      mPackBuffer(NULL),
      mOffset(0)
{}

PackPixelsParams::PackPixelsParams(const gl::Rectangle &area, GLenum format, GLenum type, GLuint outputPitch,
                                   const gl::PixelPackState &pack, ptrdiff_t offset)
    : mArea(area),
      mFormat(format),
      mType(type),
      mOutputPitch(outputPitch),
      mPackBuffer(pack.pixelBuffer.get()),
      mPack(pack.alignment, pack.reverseRowOrder),
      mOffset(offset)
{}

namespace gl_d3d11
{

D3D11_MAP GetD3DMapTypeFromBits(GLbitfield access)
{
    bool readBit = ((access & GL_MAP_READ_BIT) != 0);
    bool writeBit = ((access & GL_MAP_WRITE_BIT) != 0);
    bool discardBit = ((access & (GL_MAP_INVALIDATE_BUFFER_BIT)) != 0);

    ASSERT(!readBit || !discardBit);
    ASSERT(readBit || writeBit);

    if (readBit && !writeBit)
    {
        return D3D11_MAP_READ;
    }
    else if (writeBit && !readBit)
    {
        return (discardBit ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE);
    }
    else if (writeBit && readBit)
    {
        return D3D11_MAP_READ_WRITE;
    }
    else
    {
        UNREACHABLE();
        return D3D11_MAP_READ;
    }
}

}

BufferStorage11::BufferStorage11(Renderer11 *renderer)
    : mRenderer(renderer),
      mMappedStorage(NULL),
      mResolvedDataRevision(0),
      mReadUsageCount(0),
      mWriteUsageCount(0),
      mSize(0)
{
}

BufferStorage11::~BufferStorage11()
{
    for (auto it = mTypedBuffers.begin(); it != mTypedBuffers.end(); it++)
    {
        SafeDelete(it->second);
    }
}

BufferStorage11 *BufferStorage11::makeBufferStorage11(BufferStorage *bufferStorage)
{
    ASSERT(HAS_DYNAMIC_TYPE(BufferStorage11*, bufferStorage));
    return static_cast<BufferStorage11*>(bufferStorage);
}

void *BufferStorage11::getData()
{
    NativeBuffer11 *stagingBuffer = getStagingBuffer();
    if (stagingBuffer->getDataRevision() > mResolvedDataRevision)
    {
        if (stagingBuffer->getSize() > mResolvedData.size())
        {
            mResolvedData.resize(stagingBuffer->getSize());
        }

        ID3D11DeviceContext *context = mRenderer->getDeviceContext();

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT result = context->Map(stagingBuffer->getNativeBuffer(), 0, D3D11_MAP_READ, 0, &mappedResource);
        if (FAILED(result))
        {
            return gl::error(GL_OUT_OF_MEMORY, (void*)NULL);
        }

        memcpy(mResolvedData.data(), mappedResource.pData, stagingBuffer->getSize());

        context->Unmap(stagingBuffer->getNativeBuffer(), 0);

        mResolvedDataRevision = stagingBuffer->getDataRevision();
    }

    return mResolvedData.data();
}

void BufferStorage11::setData(const void* data, unsigned int size, unsigned int offset)
{
    size_t requiredSize = size + offset;
    mWriteUsageCount = 0;
    mSize = std::max(mSize, requiredSize);

    if (data)
    {
        NativeBuffer11 *stagingBuffer = getStagingBuffer();

        // Explicitly resize the staging buffer, preserving data if the new data will not
        // completely fill the buffer
        if (stagingBuffer->getSize() < requiredSize)
        {
            bool preserveData = (offset > 0);
            stagingBuffer->resize(requiredSize, preserveData);
        }

        ID3D11DeviceContext *context = mRenderer->getDeviceContext();

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT result = context->Map(stagingBuffer->getNativeBuffer(), 0, D3D11_MAP_WRITE, 0, &mappedResource);
        if (FAILED(result))
        {
            return gl::error(GL_OUT_OF_MEMORY);
        }

        unsigned char *offsetBufferPointer = reinterpret_cast<unsigned char *>(mappedResource.pData) + offset;
        memcpy(offsetBufferPointer, data, size);

        context->Unmap(stagingBuffer->getNativeBuffer(), 0);

        stagingBuffer->setDataRevision(stagingBuffer->getDataRevision() + 1);
    }
}

void BufferStorage11::copyData(BufferStorage* sourceStorage, unsigned int size,
                               unsigned int sourceOffset, unsigned int destOffset)
{
    BufferStorage11* sourceStorage11 = makeBufferStorage11(sourceStorage);
    if (sourceStorage11)
    {
        TypedBufferStorage11 *dest = getLatestStorage();
        if (!dest)
        {
            dest = getStagingBuffer();
        }

        TypedBufferStorage11 *source = sourceStorage11->getLatestStorage();
        if (source && dest)
        {
            dest->copyFromStorage(source, sourceOffset, size, destOffset);
            dest->setDataRevision(dest->getDataRevision() + 1);
        }

        mSize = std::max(mSize, destOffset + size);
    }
}

void BufferStorage11::clear()
{
    mSize = 0;
    mResolvedDataRevision = 0;
}

unsigned int BufferStorage11::getSize() const
{
    return mSize;
}

bool BufferStorage11::supportsDirectBinding() const
{
    return true;
}

void BufferStorage11::markBufferUsage()
{
    mReadUsageCount++;
    mWriteUsageCount++;

    const unsigned int usageLimit = 5;

    if (mReadUsageCount > usageLimit && mResolvedData.size() > 0)
    {
        mResolvedData.resize(0);
        mResolvedDataRevision = 0;
    }
}

ID3D11Buffer *BufferStorage11::getBuffer(BufferUsage usage)
{
    markBufferUsage();

    TypedBufferStorage11 *typedBuffer = getStorage(usage);
    ASSERT(HAS_DYNAMIC_TYPE(typedBuffer, NativeBuffer11));

    return static_cast<NativeBuffer11*>(typedBuffer)->getNativeBuffer();
}

ID3D11ShaderResourceView *BufferStorage11::getSRV(DXGI_FORMAT srvFormat)
{
    TypedBufferStorage11 *storage = getStorage(BUFFER_USAGE_PIXEL_UNPACK);
    ASSERT(HAS_DYNAMIC_TYPE(storage, NativeBuffer11));
    ID3D11Buffer *buffer = static_cast<NativeBuffer11*>(storage)->getNativeBuffer();

    auto bufferSRVIt = mBufferResourceViews.find(srvFormat);

    if (bufferSRVIt != mBufferResourceViews.end())
    {
        if (bufferSRVIt->second.first == buffer)
        {
            return bufferSRVIt->second.second;
        }
        else
        {
            // The underlying buffer has changed since the SRV was created: recreate the SRV.
            SafeRelease(bufferSRVIt->second.second);
        }
    }

    ID3D11Device *device = mRenderer->getDevice();
    ID3D11ShaderResourceView *bufferSRV = NULL;

    D3D11_SHADER_RESOURCE_VIEW_DESC bufferSRVDesc;
    bufferSRVDesc.Buffer.ElementOffset = 0;
    bufferSRVDesc.Buffer.ElementWidth = mSize / d3d11::GetFormatPixelBytes(srvFormat);
    bufferSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    bufferSRVDesc.Format = srvFormat;

    HRESULT result = device->CreateShaderResourceView(buffer, &bufferSRVDesc, &bufferSRV);
    ASSERT(SUCCEEDED(result));

    mBufferResourceViews[srvFormat] = BufferSRVPair(buffer, bufferSRV);

    return bufferSRV;
}

void BufferStorage11::packPixels(ID3D11Texture2D *srcTexture, UINT srcSubresource, const PackPixelsParams &params)
{
    PackStorage11 *packStorage = getPackStorage();

    TypedBufferStorage11 *latestStorage = getLatestStorage();

    packStorage->packPixels(srcTexture, srcSubresource, params);
    packStorage->setDataRevision(latestStorage ? latestStorage->getDataRevision() + 1 : 1);
}

TypedBufferStorage11 *BufferStorage11::getStorage(BufferUsage usage)
{
    TypedBufferStorage11 *directBuffer = NULL;
    auto directBufferIt = mTypedBuffers.find(usage);
    if (directBufferIt != mTypedBuffers.end())
    {
        directBuffer = directBufferIt->second;
    }

    if (!directBuffer)
    {
        if (usage == BUFFER_USAGE_PIXEL_PACK)
        {
            directBuffer = new PackStorage11(mRenderer);
        }
        else
        {
            // buffer is not allocated, create it
            directBuffer = new NativeBuffer11(mRenderer, usage);
        }

        mTypedBuffers.insert(std::make_pair(usage, directBuffer));
    }

    // resize buffer
    if (directBuffer->getSize() < mSize)
    {
        directBuffer->resize(mSize, true);
    }

    TypedBufferStorage11 *latestBuffer = getLatestStorage();
    if (latestBuffer && latestBuffer->getDataRevision() > directBuffer->getDataRevision())
    {
        // if copyFromStorage returns true, the D3D buffer has been recreated
        // and we should update our serial
        if (directBuffer->copyFromStorage(latestBuffer, 0, latestBuffer->getSize(), 0))
        {
            updateSerial();
        }
        directBuffer->setDataRevision(latestBuffer->getDataRevision());
    }

    return directBuffer;
}

TypedBufferStorage11 *BufferStorage11::getLatestStorage() const
{
    // Even though we iterate over all the direct buffers, it is expected that only
    // 1 or 2 will be present.
    TypedBufferStorage11 *latestStorage = NULL;
    DataRevision latestRevision = 0;
    for (auto it = mTypedBuffers.begin(); it != mTypedBuffers.end(); it++)
    {
        TypedBufferStorage11 *storage = it->second;
        if (storage->getDataRevision() > latestRevision)
        {
            latestStorage = storage;
            latestRevision = storage->getDataRevision();
        }
    }

    return latestStorage;
}

bool BufferStorage11::isMapped() const
{
    return mMappedStorage != NULL;
}

void *BufferStorage11::map(GLbitfield access)
{
    ASSERT(!mMappedStorage);

    TypedBufferStorage11 *latestStorage = getLatestStorage();

    if (latestStorage->getUsage() == BUFFER_USAGE_PIXEL_PACK ||
        latestStorage->getUsage() == BUFFER_USAGE_STAGING)
    {
        mMappedStorage = latestStorage;
    }
    else
    {
        mMappedStorage = getStagingBuffer();
    }

    return mMappedStorage->map(access);
}

void BufferStorage11::unmap()
{
    ASSERT(mMappedStorage);
    mMappedStorage->unmap();
    mMappedStorage = NULL;
}

NativeBuffer11 *BufferStorage11::getStagingBuffer()
{
    return static_cast<NativeBuffer11*>(getStorage(BUFFER_USAGE_STAGING));
}

PackStorage11 *BufferStorage11::getPackStorage()
{
    return static_cast<PackStorage11*>(getStorage(BUFFER_USAGE_PIXEL_PACK));
}

TypedBufferStorage11::TypedBufferStorage11(Renderer11 *renderer, BufferUsage usage)
    : mRenderer(renderer),
      mUsage(usage),
      mRevision(0),
      mBufferSize(0)
{
}

NativeBuffer11::NativeBuffer11(Renderer11 *renderer, BufferUsage usage)
    : TypedBufferStorage11(renderer, usage),
      mNativeBuffer(NULL)
{
}

NativeBuffer11::~NativeBuffer11()
{
    SafeRelease(mNativeBuffer);
}

// Returns true if it recreates the direct buffer
bool NativeBuffer11::copyFromStorage(TypedBufferStorage11 *source, size_t sourceOffset, size_t size, size_t destOffset)
{
    if (source->getUsage() == BUFFER_USAGE_PIXEL_PACK)
    {
        UNIMPLEMENTED();
        return false;
    }

    ASSERT(HAS_DYNAMIC_TYPE(source, NativeBuffer11));

    if (source->getUsage() == BUFFER_USAGE_PIXEL_PACK)
    {
        return false;
    }

    ID3D11DeviceContext *context = mRenderer->getDeviceContext();

    size_t requiredSize = sourceOffset + size;
    bool createBuffer = !mNativeBuffer || mBufferSize < requiredSize;

    // (Re)initialize D3D buffer if needed
    if (createBuffer)
    {
        bool preserveData = (destOffset > 0);
        resize(source->getSize(), preserveData);
    }

    D3D11_BOX srcBox;
    srcBox.left = sourceOffset;
    srcBox.right = sourceOffset + size;
    srcBox.top = 0;
    srcBox.bottom = 1;
    srcBox.front = 0;
    srcBox.back = 1;

    ID3D11Buffer *sourceBuffer = static_cast<NativeBuffer11*>(source)->getNativeBuffer();
    context->CopySubresourceRegion(mNativeBuffer, 0, destOffset, 0, 0, sourceBuffer, 0, &srcBox);

    return createBuffer;
}

void NativeBuffer11::resize(size_t size, bool preserveData)
{
    ID3D11Device *device = mRenderer->getDevice();
    ID3D11DeviceContext *context = mRenderer->getDeviceContext();

    D3D11_BUFFER_DESC bufferDesc;
    fillBufferDesc(&bufferDesc, mRenderer, mUsage, size);

    ID3D11Buffer *newBuffer;
    HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &newBuffer);

    if (FAILED(result))
    {
        return gl::error(GL_OUT_OF_MEMORY);
    }

    if (mNativeBuffer && preserveData)
    {
        D3D11_BOX srcBox;
        srcBox.left = 0;
        srcBox.right = size;
        srcBox.top = 0;
        srcBox.bottom = 1;
        srcBox.front = 0;
        srcBox.back = 1;

        context->CopySubresourceRegion(newBuffer, 0, 0, 0, 0, mNativeBuffer, 0, &srcBox);
    }

    // No longer need the old buffer
    SafeRelease(mNativeBuffer);
    mNativeBuffer = newBuffer;

    mBufferSize = bufferDesc.ByteWidth;
}

void NativeBuffer11::fillBufferDesc(D3D11_BUFFER_DESC* bufferDesc, Renderer *renderer, BufferUsage usage, unsigned int bufferSize)
{
    bufferDesc->ByteWidth = bufferSize;
    bufferDesc->MiscFlags = 0;
    bufferDesc->StructureByteStride = 0;

    switch (usage)
    {
      case BUFFER_USAGE_STAGING:
        bufferDesc->Usage = D3D11_USAGE_STAGING;
        bufferDesc->BindFlags = 0;
        bufferDesc->CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
        break;

      case BUFFER_USAGE_VERTEX:
        bufferDesc->Usage = D3D11_USAGE_DEFAULT;
        bufferDesc->BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc->CPUAccessFlags = 0;
        break;

      case BUFFER_USAGE_INDEX:
        bufferDesc->Usage = D3D11_USAGE_DEFAULT;
        bufferDesc->BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc->CPUAccessFlags = 0;
        break;

      case BUFFER_USAGE_PIXEL_UNPACK:
        bufferDesc->Usage = D3D11_USAGE_DEFAULT;
        bufferDesc->BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bufferDesc->CPUAccessFlags = 0;
        break;

      case BUFFER_USAGE_UNIFORM:
        bufferDesc->Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc->BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc->CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        // Constant buffers must be of a limited size, and aligned to 16 byte boundaries
        // For our purposes we ignore any buffer data past the maximum constant buffer size
        bufferDesc->ByteWidth = roundUp(bufferDesc->ByteWidth, 16u);
        bufferDesc->ByteWidth = std::min(bufferDesc->ByteWidth, renderer->getMaxUniformBufferSize());
        break;

    default:
        UNREACHABLE();
    }
}

void *NativeBuffer11::map(GLbitfield access)
{
    ASSERT(mUsage == BUFFER_USAGE_STAGING);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ID3D11DeviceContext *context = mRenderer->getDeviceContext();
    D3D11_MAP d3dMapType = gl_d3d11::GetD3DMapTypeFromBits(access);
    UINT d3dMapFlag = ((access & GL_MAP_UNSYNCHRONIZED_BIT) != 0 ? D3D11_MAP_FLAG_DO_NOT_WAIT : 0);

    HRESULT result = context->Map(mNativeBuffer, 0, d3dMapType, d3dMapFlag, &mappedResource);
    ASSERT(SUCCEEDED(result));

    return mappedResource.pData;
}

void NativeBuffer11::unmap()
{
    ASSERT(mUsage == BUFFER_USAGE_STAGING);
    ID3D11DeviceContext *context = mRenderer->getDeviceContext();
    context->Unmap(mNativeBuffer, 0);
}

PackStorage11::PackStorage11(Renderer11 *renderer)
    : TypedBufferStorage11(renderer, BUFFER_USAGE_PIXEL_PACK),
      mStagingTexture(NULL),
      mTextureFormat(DXGI_FORMAT_UNKNOWN),
      mMemoryBuffer(NULL),
      mDataModified(false)
{
}

PackStorage11::~PackStorage11()
{
    SafeRelease(mStagingTexture);

    if (mMemoryBuffer != NULL)
    {
        delete [] mMemoryBuffer;
        mMemoryBuffer = NULL;
    }
}

bool PackStorage11::copyFromStorage(TypedBufferStorage11 *source, size_t sourceOffset,
                                    size_t size, size_t destOffset)
{
    UNIMPLEMENTED();
    return false;
}

void PackStorage11::resize(size_t size, bool preserveData)
{
    UNIMPLEMENTED();
}

void *PackStorage11::map(GLbitfield access)
{
    ASSERT(!mMemoryBuffer);

    mMemoryBuffer = new unsigned char[mBufferSize];
    mRenderer->packPixels(mStagingTexture, mPackParams, mMemoryBuffer);
    mDataModified = (mDataModified || (access & GL_MAP_WRITE_BIT) != 0);

    return mMemoryBuffer;
}

void PackStorage11::unmap()
{
    ASSERT(mMemoryBuffer);
    delete [] mMemoryBuffer;
    mMemoryBuffer = NULL;
}

void PackStorage11::packPixels(ID3D11Texture2D *srcTexure, UINT srcSubresource, const PackPixelsParams &params)
{
    mPackParams = params;
    mBufferSize = mPackParams.mOutputPitch * mPackParams.mArea.height;

    D3D11_TEXTURE2D_DESC textureDesc;
    srcTexure->GetDesc(&textureDesc);

    if (mStagingTexture != NULL &&
        (mTextureFormat != textureDesc.Format ||
         mTextureSize.width != params.mArea.width ||
         mTextureSize.height != params.mArea.height))
    {
        SafeRelease(mStagingTexture);
        mTextureSize.width = 0;
        mTextureSize.height = 0;
        mTextureFormat = DXGI_FORMAT_UNKNOWN;
    }

    if (mStagingTexture == NULL)
    {
        ID3D11Device *device = mRenderer->getDevice();
        HRESULT hr;

        mTextureSize.width = params.mArea.width;
        mTextureSize.height = params.mArea.height;
        mTextureFormat = textureDesc.Format;

        D3D11_TEXTURE2D_DESC stagingDesc;
        stagingDesc.Width = params.mArea.width;
        stagingDesc.Height = params.mArea.height;
        stagingDesc.MipLevels = 1;
        stagingDesc.ArraySize = 1;
        stagingDesc.Format = mTextureFormat;
        stagingDesc.SampleDesc.Count = 1;
        stagingDesc.SampleDesc.Quality = 0;
        stagingDesc.Usage = D3D11_USAGE_STAGING;
        stagingDesc.BindFlags = 0;
        stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        stagingDesc.MiscFlags = 0;

        hr = device->CreateTexture2D(&stagingDesc, NULL, &mStagingTexture);
        ASSERT(SUCCEEDED(hr));
    }

    ID3D11Texture2D* srcTex = NULL;
    if (textureDesc.SampleDesc.Count > 1)
    {
        UNIMPLEMENTED();
    }

    ID3D11DeviceContext *immediateContext = mRenderer->getDeviceContext();
    D3D11_BOX srcBox;
    srcBox.left = params.mArea.x;
    srcBox.right = params.mArea.x + params.mArea.width;
    srcBox.top = params.mArea.y;
    srcBox.bottom = params.mArea.y + params.mArea.height;
    srcBox.front = 0;
    srcBox.back = 1;

    // Asynchronous copy
    immediateContext->CopySubresourceRegion(mStagingTexture, 0, 0, 0, 0, srcTexure, srcSubresource, &srcBox);
}

}
