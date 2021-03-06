//
// Copyright (c) 2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// BufferStorage11.h Defines the BufferStorage11 class.

#ifndef LIBGLESV2_RENDERER_BUFFERSTORAGE11_H_
#define LIBGLESV2_RENDERER_BUFFERSTORAGE11_H_

#include "libGLESv2/renderer/BufferStorage.h"

namespace rx
{
class Renderer;
class Renderer11;
class DirectBufferStorage11;
class NativeBufferStorage11;
class PackBufferStorage11;

enum BufferUsage
{
    BUFFER_USAGE_STAGING = 0,
    BUFFER_USAGE_VERTEX = 1,
    BUFFER_USAGE_INDEX = 2,
    BUFFER_USAGE_PIXEL_UNPACK = 3,
    BUFFER_USAGE_PIXEL_PACK = 4,
    BUFFER_USAGE_UNIFORM = 5,
};

typedef size_t DataRevision;

class BufferStorage11 : public BufferStorage
{
  public:
    explicit BufferStorage11(Renderer11 *renderer);
    virtual ~BufferStorage11();

    static BufferStorage11 *makeBufferStorage11(BufferStorage *bufferStorage);

    virtual void *getData();
    virtual void setData(const void* data, unsigned int size, unsigned int offset);
    virtual void copyData(BufferStorage* sourceStorage, unsigned int size,
                          unsigned int sourceOffset, unsigned int destOffset);
    virtual void clear();
    virtual unsigned int getSize() const;
    virtual bool supportsDirectBinding() const;

    ID3D11Buffer *getBuffer(BufferUsage usage);
    ID3D11ShaderResourceView *getSRV(DXGI_FORMAT srvFormat);

    virtual bool isMapped() const;
    virtual void *map(GLbitfield access);
    virtual void unmap();

    ID3D11RenderTargetView *getPackTextureRTV(DXGI_FORMAT packFormat, size_t minimumSize);
    size_t getPackTextureSize();
    void stagePackTexture();

  private:

    enum StorageType
    {
        STORAGE_NONE,
        STORAGE_BUFFER,
        STORAGE_TEXTURE
    };

    Renderer11 *mRenderer;
    ID3D11Resource *mMappedResource;

    StorageType mMapState;

    std::map<BufferUsage, DirectBufferStorage11*> mDirectBuffers;

    typedef std::pair<ID3D11Buffer *, ID3D11ShaderResourceView *> BufferSRVPair;
    std::map<DXGI_FORMAT, BufferSRVPair> mBufferResourceViews;

    std::vector<unsigned char> mResolvedData;
    DataRevision mResolvedDataRevision;

    unsigned int mReadUsageCount;
    unsigned int mWriteUsageCount;

    size_t mSize;

    void markBufferUsage();
    DirectBufferStorage11 *getStagingStorage();
    PackBufferStorage11 *getPackStorage();

    DirectBufferStorage11 *getStorage(BufferUsage usage);
    DirectBufferStorage11 *getLatestStorage();
    DirectBufferStorage11 *getLatestAccessibleStorage(bool requireCPUAccess);
};

// Each instance of BufferStorageD3DBuffer11 is specialized for a class of D3D binding points
// - vertex/transform feedback buffers
// - index buffers
// - pixel unpack buffers
// - pixel pack buffers
// - uniform buffers
class DirectBufferStorage11
{
  public:
    DirectBufferStorage11(Renderer11 *renderer, BufferUsage usage);
    virtual ~DirectBufferStorage11() {}

    BufferUsage getUsage() const { return mUsage; }
    size_t getSize() const { return mBufferSize; }
    DataRevision getDataRevision() const { return mDataRevision; }
    void setDataRevision(DataRevision rev) { mDataRevision = rev; }

    virtual ID3D11Resource *getD3DResource() const = 0;
    virtual bool isCPUAccessible() const = 0;
    virtual bool copyFromStorage(DirectBufferStorage11 *source, size_t sourceOffset,
                                 size_t size, size_t destOffset) = 0;
    virtual void resize(size_t size, bool preserveData) = 0;

  protected:
    Renderer11 *mRenderer;
    const BufferUsage mUsage;
    DataRevision mDataRevision;
    size_t mBufferSize;
};

class NativeBufferStorage11 : public DirectBufferStorage11
{
  public:
    NativeBufferStorage11(Renderer11 *renderer, BufferUsage usage);
    ~NativeBufferStorage11();

    virtual ID3D11Resource *getD3DResource() const { return mD3DBuffer; }
    virtual bool isCPUAccessible() const { return (mUsage == BUFFER_USAGE_STAGING); }
    virtual bool copyFromStorage(DirectBufferStorage11 *source, size_t sourceOffset,
                                 size_t size, size_t destOffset);
    virtual void resize(size_t size, bool preserveData);

  private:
    ID3D11Buffer *mD3DBuffer;

    void fillBufferDesc(D3D11_BUFFER_DESC* bufferDesc, BufferUsage usage, unsigned int bufferSize) const;
};

// For pixel pack buffers, which are backed by a texture
class PackBufferStorage11 : public DirectBufferStorage11
{
  public:
    PackBufferStorage11(Renderer11 *renderer, BufferUsage usage);
    ~PackBufferStorage11();

    virtual ID3D11Resource *getD3DResource() const { return mStagingTexture; }
    virtual bool isCPUAccessible() const { return true; }
    virtual bool copyFromStorage(DirectBufferStorage11 *source, size_t sourceOffset,
                                 size_t size, size_t destOffset);
    virtual void resize(size_t size, bool preserveData);

    size_t getPackTextureSize() const { return mPackTextureSize; }
    ID3D11RenderTargetView *getPackTextureRTV(DXGI_FORMAT packFormat, size_t minimumSize);
    void stagePackTexture();

  private:
    DXGI_FORMAT mPackFormat;
    size_t mPackTextureSize;
    ID3D11Texture2D *mStagingTexture;
    ID3D11Texture2D *mPackTexture;
    ID3D11RenderTargetView *mPackTextureRTV;
};

}

#endif // LIBGLESV2_RENDERER_BUFFERSTORAGE11_H_
