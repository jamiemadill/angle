//
// Copyright (c) 2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// BufferStorage11.h Defines the BufferStorage11 class.

#ifndef LIBGLESV2_RENDERER_BUFFERSTORAGE11_H_
#define LIBGLESV2_RENDERER_BUFFERSTORAGE11_H_

#include "libGLESv2/renderer/BufferStorage.h"
#include "libGLESv2/angletypes.h"

namespace rx
{
class Renderer;
class Renderer11;
class TypedBufferStorage11;
class NativeBuffer11;
class PackStorage11;

enum BufferUsage
{
    BUFFER_USAGE_STAGING = 0,
    BUFFER_USAGE_VERTEX = 1,
    BUFFER_USAGE_INDEX = 2,
    BUFFER_USAGE_PIXEL_UNPACK = 3,
    BUFFER_USAGE_UNIFORM = 4,
    BUFFER_USAGE_PIXEL_PACK = 5,
};

struct PackPixelsParams
{
    PackPixelsParams();
    PackPixelsParams(const gl::Rectangle &area, GLenum format, GLenum type, GLuint outputPitch,
                     const gl::PixelPackState &pack, ptrdiff_t offset);

    gl::Rectangle mArea;
    GLenum mFormat;
    GLenum mType;
    GLuint mOutputPitch;
    gl::Buffer *mPackBuffer;
    gl::PixelPackState mPack;
    ptrdiff_t mOffset;
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
    void packPixels(ID3D11Texture2D *srcTexure, UINT srcSubresource, const PackPixelsParams &params);

    virtual bool isMapped() const;
    virtual void *map(GLbitfield access);
    virtual void unmap();

  private:
    Renderer11 *mRenderer;
    bool mIsMapped;

    std::map<BufferUsage, TypedBufferStorage11*> mTypedBuffers;

    typedef std::pair<ID3D11Buffer *, ID3D11ShaderResourceView *> BufferSRVPair;
    std::map<DXGI_FORMAT, BufferSRVPair> mBufferResourceViews;

    std::vector<unsigned char> mResolvedData;
    DataRevision mResolvedDataRevision;

    unsigned int mReadUsageCount;
    unsigned int mWriteUsageCount;

    size_t mSize;

    void markBufferUsage();
    NativeBuffer11 *getStagingBuffer();
    PackStorage11 *getPackStorage();

    TypedBufferStorage11 *getStorage(BufferUsage usage);
    TypedBufferStorage11 *getLatestStorage() const;
};

// Each instance of BufferStorageD3DBuffer11 is specialized for a class of D3D binding points
// - vertex/transform feedback buffers
// - index buffers
// - pixel unpack buffers
// - uniform buffers
class TypedBufferStorage11
{
  public:
    virtual ~TypedBufferStorage11() {}

    DataRevision getDataRevision() const { return mRevision; }
    BufferUsage getUsage() const { return mUsage; }
    size_t getSize() const { return mBufferSize; }

    void setDataRevision(DataRevision rev) { mRevision = rev; }

    virtual bool copyFromStorage(TypedBufferStorage11 *source, size_t sourceOffset,
                                 size_t size, size_t destOffset) = 0;
    virtual void resize(size_t size, bool preserveData) = 0;

    virtual void *map(GLbitfield access) = 0;
    virtual void unmap() = 0;

  protected:
    TypedBufferStorage11(Renderer11 *renderer, BufferUsage usage);

    Renderer11 *mRenderer;
    DataRevision mRevision;
    const BufferUsage mUsage;
    size_t mBufferSize;
};

class NativeBuffer11 : public TypedBufferStorage11
{
  public:
    NativeBuffer11(Renderer11 *renderer, BufferUsage usage);
    ~NativeBuffer11();

    ID3D11Buffer *getNativeBuffer() const { return mNativeBuffer; }

    virtual bool copyFromStorage(TypedBufferStorage11 *source, size_t sourceOffset,
                                 size_t size, size_t destOffset) ;
    virtual void resize(size_t size, bool preserveData);

    virtual void *map(GLbitfield access);
    virtual void unmap();

  private:
    ID3D11Buffer *mNativeBuffer;

    static void fillBufferDesc(D3D11_BUFFER_DESC* bufferDesc, Renderer *renderer, BufferUsage usage, unsigned int bufferSize);
};

class PackStorage11 : public TypedBufferStorage11
{
  public:
    PackStorage11(Renderer11 *renderer);
    ~PackStorage11();

    virtual bool copyFromStorage(TypedBufferStorage11 *source, size_t sourceOffset,
                                 size_t size, size_t destOffset) ;
    virtual void resize(size_t size, bool preserveData);

    virtual void *map(GLbitfield access);
    virtual void unmap();

    void packPixels(ID3D11Texture2D *srcTexure, UINT srcSubresource, const PackPixelsParams &params);

  private:
    ID3D11Texture2D *mStagingTexture;
    DXGI_FORMAT mTextureFormat;
    gl::Extents mTextureSize;
    unsigned char *mMemoryBuffer;
    PackPixelsParams mPackParams;
};

}

#endif // LIBGLESV2_RENDERER_BUFFERSTORAGE11_H_
