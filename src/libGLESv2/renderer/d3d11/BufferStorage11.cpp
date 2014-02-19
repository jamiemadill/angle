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

namespace rx
{

namespace gl_d3d11
{

D3D11_MAP ConvertMapBits(GLbitfield access)
{
    bool readBit = ((access & GL_MAP_READ_BIT) > 0);
    bool writeBit = ((access & GL_MAP_WRITE_BIT) > 0);
    bool discardBit = ((access & (GL_MAP_INVALIDATE_BUFFER_BIT)) > 0);

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
      mMappedResource(NULL),
      mResolvedDataRevision(0),
      mReadUsageCount(0),
      mWriteUsageCount(0),
      mSize(0)
{
}

BufferStorage11::~BufferStorage11()
{
    for (auto it = mDirectBuffers.begin(); it != mDirectBuffers.end(); it++)
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
    DirectBufferStorage11 *stagingBuffer = getStagingStorage();
    if (stagingBuffer->getDataRevision() > mResolvedDataRevision)
    {
        if (stagingBuffer->getSize() > mResolvedData.size())
        {
            mResolvedData.resize(stagingBuffer->getSize());
        }

        ID3D11DeviceContext *context = mRenderer->getDeviceContext();

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT result = context->Map(stagingBuffer->getD3DResource(), 0, D3D11_MAP_READ, 0, &mappedResource);
        if (FAILED(result))
        {
            return gl::error(GL_OUT_OF_MEMORY, (void*)NULL);
        }

        memcpy(mResolvedData.data(), mappedResource.pData, stagingBuffer->getSize());

        context->Unmap(stagingBuffer->getD3DResource(), 0);

        mResolvedDataRevision = stagingBuffer->getDataRevision();
    }

    return mResolvedData.data();
}

void BufferStorage11::setData(const void* data, unsigned int size, unsigned int offset)
{
    DirectBufferStorage11 *stagingBuffer = getStagingStorage();

    // Explicitly resize the staging buffer, preserving data if the new data will not
    // completely fill the buffer
    size_t requiredSize = size + offset;
    if (stagingBuffer->getSize() < requiredSize)
    {
        bool preserveData = (offset > 0);
        stagingBuffer->resize(requiredSize, preserveData);
    }

    if (data)
    {
        ID3D11DeviceContext *context = mRenderer->getDeviceContext();

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT result = context->Map(stagingBuffer->getD3DResource(), 0, D3D11_MAP_WRITE, 0, &mappedResource);
        if (FAILED(result))
        {
            return gl::error(GL_OUT_OF_MEMORY);
        }

        unsigned char *offsetBufferPointer = reinterpret_cast<unsigned char *>(mappedResource.pData) + offset;
        memcpy(offsetBufferPointer, data, size);

        context->Unmap(stagingBuffer->getD3DResource(), 0);
    }

    stagingBuffer->setDataRevision(stagingBuffer->getDataRevision() + 1);
    mWriteUsageCount = 0;
    mSize = std::max(mSize, requiredSize);
}

void BufferStorage11::copyData(BufferStorage* sourceStorage, unsigned int size,
                               unsigned int sourceOffset, unsigned int destOffset)
{
    BufferStorage11* sourceStorage11 = makeBufferStorage11(sourceStorage);
    if (sourceStorage11)
    {
        DirectBufferStorage11 *dest = getLatestStorage();
        if (!dest)
        {
            dest = getStagingStorage();
        }

        DirectBufferStorage11 *source = sourceStorage11->getLatestStorage();
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
    ASSERT(usage != BUFFER_USAGE_PIXEL_PACK);

    markBufferUsage();
    return static_cast<ID3D11Buffer*>(getStorage(usage)->getD3DResource());
}

ID3D11ShaderResourceView *BufferStorage11::getSRV(DXGI_FORMAT srvFormat)
{
    DirectBufferStorage11 *storage = getStorage(BUFFER_USAGE_PIXEL_UNPACK);
    ID3D11Buffer *buffer = static_cast<ID3D11Buffer*>(storage->getD3DResource());

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

DirectBufferStorage11 *BufferStorage11::getStorage(BufferUsage usage)
{
    DirectBufferStorage11 *directBuffer = NULL;
    auto directBufferIt = mDirectBuffers.find(usage);
    if (directBufferIt != mDirectBuffers.end())
    {
        directBuffer = directBufferIt->second;
    }

    if (!directBuffer)
    {
        // buffer is not allocated, create it
        if (usage == BUFFER_USAGE_PIXEL_PACK)
        {
            directBuffer = new PackBufferStorage11(mRenderer, usage);
        }
        else
        {
            directBuffer = new NativeBufferStorage11(mRenderer, usage);
        }

        mDirectBuffers.insert(std::make_pair(usage, directBuffer));
    }

    DirectBufferStorage11 *latestBuffer = getLatestStorage();
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

DirectBufferStorage11 *BufferStorage11::getLatestStorage()
{
    return getLatestAccessibleStorage(false);
}

DirectBufferStorage11 *BufferStorage11::getLatestAccessibleStorage(bool requireCPUAccess)
{
    // Even though we iterate over all the direct buffers, it is expected that only
    // 1 or 2 will be present.
    DirectBufferStorage11 *latestStorage = NULL;
    DataRevision latestRevision = 0;
    for (auto it = mDirectBuffers.begin(); it != mDirectBuffers.end(); it++)
    {
        DirectBufferStorage11 *storage = it->second;
        if (storage->getDataRevision() > latestRevision &&
            (storage->isCPUAccessible() || !requireCPUAccess))
        {
            latestStorage = storage;
            latestRevision = storage->getDataRevision();
        }
    }

    return latestStorage;
}

bool BufferStorage11::isMapped() const
{
    return (mMappedResource != NULL);
}

void *BufferStorage11::map(GLbitfield access)
{
    ASSERT(!isMapped());

    D3D11_MAPPED_SUBRESOURCE mapInfo;
    HRESULT result;
    ID3D11DeviceContext *context = mRenderer->getDeviceContext();
    D3D11_MAP d3dMapType = gl_d3d11::ConvertMapBits(access);
    UINT d3dMapFlag = ((access & GL_MAP_UNSYNCHRONIZED_BIT) > 0 ? D3D11_MAP_FLAG_DO_NOT_WAIT : 0);
    DirectBufferStorage11 *latestStorage = getLatestAccessibleStorage(true);

    if (!latestStorage)
    {
        latestStorage = getStagingStorage();
        ASSERT(latestStorage);
    }

    mMappedResource = latestStorage->getD3DResource();

    result = context->Map(mMappedResource, 0, d3dMapType, d3dMapFlag, &mapInfo);
    ASSERT(SUCCEEDED(result));

    return mapInfo.pData;
}

void BufferStorage11::unmap()
{
    ASSERT(mMappedResource);

    ID3D11DeviceContext *context = mRenderer->getDeviceContext();
    context->Unmap(mMappedResource, 0);

    mMappedResource = false;
}

size_t BufferStorage11::getPackTextureSize()
{
    return getPackStorage()->getPackTextureSize();
}

void BufferStorage11::stagePackTexture()
{
    PackBufferStorage11 *packStorage = getPackStorage();

    packStorage->stagePackTexture();
    packStorage->setDataRevision(packStorage->getDataRevision() + 1);
}

ID3D11RenderTargetView *BufferStorage11::getPackTextureRTV(DXGI_FORMAT packFormat, size_t minimumSize)
{
    return getPackStorage()->getPackTextureRTV(packFormat, minimumSize);
}

DirectBufferStorage11 *BufferStorage11::getStagingStorage()
{
    return getStorage(BUFFER_USAGE_STAGING);
}

PackBufferStorage11 *BufferStorage11::getPackStorage()
{
    return static_cast<PackBufferStorage11*>(getStorage(BUFFER_USAGE_PIXEL_PACK));
}

DirectBufferStorage11::DirectBufferStorage11(Renderer11 *renderer, BufferUsage usage)
    : mRenderer(renderer),
      mUsage(usage),
      mDataRevision(0),
      mBufferSize(0)
{
}

NativeBufferStorage11::NativeBufferStorage11(Renderer11 *renderer, BufferUsage usage)
    : DirectBufferStorage11(renderer, usage),
      mD3DBuffer(NULL)
{
}

NativeBufferStorage11::~NativeBufferStorage11()
{
    SafeRelease(mD3DBuffer);
}

// Returns true if it recreates the direct buffer
bool NativeBufferStorage11::copyFromStorage(DirectBufferStorage11 *source, size_t sourceOffset,
                                            size_t size, size_t destOffset)
{
    if (source->getUsage() == BUFFER_USAGE_PIXEL_PACK)
    {
        // TODO: handle pack buffer (texture)
        UNIMPLEMENTED();
        return false;
    }

    ID3D11DeviceContext *context = mRenderer->getDeviceContext();

    size_t requiredSize = sourceOffset + size;
    bool createBuffer = !mD3DBuffer || mBufferSize < requiredSize;

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

    context->CopySubresourceRegion(mD3DBuffer, 0, destOffset, 0, 0, source->getD3DResource(), 0, &srcBox);

    return createBuffer;
}

void NativeBufferStorage11::resize(size_t size, bool preserveData)
{
    ID3D11Device *device = mRenderer->getDevice();
    ID3D11DeviceContext *context = mRenderer->getDeviceContext();

    D3D11_BUFFER_DESC bufferDesc;
    fillBufferDesc(&bufferDesc, mUsage, size);

    ID3D11Buffer *newBuffer;
    HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &newBuffer);

    if (FAILED(result))
    {
        return gl::error(GL_OUT_OF_MEMORY);
    }

    if (mD3DBuffer && preserveData)
    {
        D3D11_BOX srcBox;
        srcBox.left = 0;
        srcBox.right = size;
        srcBox.top = 0;
        srcBox.bottom = 1;
        srcBox.front = 0;
        srcBox.back = 1;

        context->CopySubresourceRegion(newBuffer, 0, 0, 0, 0, mD3DBuffer, 0, &srcBox);
    }

    // No longer need the old buffer
    SafeRelease(mD3DBuffer);
    mD3DBuffer = newBuffer;

    mBufferSize = bufferDesc.ByteWidth;
}

void NativeBufferStorage11::fillBufferDesc(D3D11_BUFFER_DESC *bufferDesc, BufferUsage usage, unsigned int bufferSize) const
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
        bufferDesc->ByteWidth = std::min(bufferDesc->ByteWidth, mRenderer->getMaxUniformBufferSize());
        break;

      default:
        UNREACHABLE();
    }
}

PackBufferStorage11::PackBufferStorage11(Renderer11 *renderer, BufferUsage usage)
    : DirectBufferStorage11(renderer, usage),
      mPackFormat(DXGI_FORMAT_UNKNOWN),
      mPackTextureSize(0),
      mStagingTexture(NULL),
      mPackTexture(NULL),
      mPackTextureRTV(NULL)
{
}

PackBufferStorage11::~PackBufferStorage11()
{
    SafeRelease(mStagingTexture);
    SafeRelease(mPackTexture);
    SafeRelease(mPackTextureRTV);
}

ID3D11RenderTargetView *PackBufferStorage11::getPackTextureRTV(DXGI_FORMAT packFormat, size_t minimumSize)
{
    size_t packTextureCapacity = mPackTextureSize * mPackTextureSize;

    if (mPackFormat != packFormat || minimumSize > packTextureCapacity)
    {
        // TODO: preserve contents

        mPackTextureSize = 0;
        mPackFormat = DXGI_FORMAT_UNKNOWN;

        SafeRelease(mPackTexture);
        SafeRelease(mStagingTexture);
    }

    if (mPackTexture == NULL)
    {
        ID3D11Device *device = mRenderer->getDevice();
        HRESULT hr;

        unsigned int textureDim = 32;
        while (textureDim * textureDim < minimumSize)
        {
            textureDim *= 2;
        }

        mPackTextureSize = textureDim;
        mPackFormat = packFormat;

        D3D11_TEXTURE2D_DESC storageDesc;
        storageDesc.Width = textureDim;
        storageDesc.Height = textureDim;
        storageDesc.MipLevels = 1;
        storageDesc.ArraySize = 1;
        storageDesc.Format = packFormat;
        storageDesc.SampleDesc.Count = 1;
        storageDesc.SampleDesc.Quality = 0;
        storageDesc.Usage = D3D11_USAGE_DEFAULT;
        storageDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
        storageDesc.CPUAccessFlags = 0;
        storageDesc.MiscFlags = 0;

        hr = device->CreateTexture2D(&storageDesc, NULL, &mPackTexture);
        if (FAILED(hr))
        {
            return gl::error<ID3D11RenderTargetView*>(GL_OUT_OF_MEMORY, NULL);
        }

        D3D11_TEXTURE2D_DESC stagingDesc;
        stagingDesc.Width = textureDim;
        stagingDesc.Height = textureDim;
        stagingDesc.MipLevels = 1;
        stagingDesc.ArraySize = 1;
        stagingDesc.Format = packFormat;
        stagingDesc.SampleDesc.Count = 1;
        stagingDesc.SampleDesc.Quality = 0;
        stagingDesc.Usage = D3D11_USAGE_STAGING;
        stagingDesc.BindFlags = 0;
        stagingDesc.CPUAccessFlags = (D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE);
        stagingDesc.MiscFlags = 0;

        hr = device->CreateTexture2D(&stagingDesc, NULL, &mStagingTexture);
        if (FAILED(hr))
        {
            return gl::error<ID3D11RenderTargetView*>(GL_OUT_OF_MEMORY, NULL);
        }

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
        rtvDesc.Format = packFormat;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;

        hr = device->CreateRenderTargetView(mPackTexture, &rtvDesc, &mPackTextureRTV);
        if (FAILED(hr))
        {
            return gl::error<ID3D11RenderTargetView*>(GL_OUT_OF_MEMORY, NULL);
        }
    }

    return mPackTextureRTV;
}

// Returns true if it recreates the direct buffer
bool PackBufferStorage11::copyFromStorage(DirectBufferStorage11 *source, size_t sourceOffset,
                                            size_t size, size_t destOffset)
{
    // TODO: PackBufferStorage11::copyFromStorage
    UNIMPLEMENTED();
    return false;
}

void PackBufferStorage11::stagePackTexture()
{
    ID3D11DeviceContext *immediateContext = mRenderer->getDeviceContext();
    immediateContext->CopyResource(mStagingTexture, mPackTexture);
}

void PackBufferStorage11::resize(size_t size, bool preserveData)
{
    UNIMPLEMENTED();
}

}
