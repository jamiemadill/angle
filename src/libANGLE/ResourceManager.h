//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// ResourceManager.h : Defines the ResourceManager class, which tracks objects
// shared by multiple GL contexts.

#ifndef LIBANGLE_RESOURCEMANAGER_H_
#define LIBANGLE_RESOURCEMANAGER_H_

#include "common/angleutils.h"
#include "libANGLE/angletypes.h"
#include "libANGLE/HandleAllocator.h"

#include "angle_gl.h"

namespace rx
{
class Renderer;
}

namespace gl
{
class Buffer;
class Shader;
class Program;
class Texture;
class Renderbuffer;
class Sampler;
class FenceSync;
struct Data;

class ResourceManager
{
  public:
    explicit ResourceManager(rx::Renderer *renderer);
    ~ResourceManager();

    void addRef();
    void release();

    GLuint createBuffer();
    GLuint createShader(const gl::Data &data, GLenum type);
    GLuint createProgram();
    GLuint createTexture();
    GLuint createRenderbuffer();
    GLuint createSampler();
    GLuint createFenceSync();

    void deleteBuffer(GLuint buffer);
    void deleteShader(GLuint shader);
    void deleteProgram(GLuint program);
    void deleteTexture(GLuint texture);
    void deleteRenderbuffer(GLuint renderbuffer);
    void deleteSampler(GLuint sampler);
    void deleteFenceSync(GLuint fenceSync);

    Buffer *getBuffer(GLuint handle);
    Shader *getShader(GLuint handle);
    Program *getProgram(GLuint handle) const;
    Texture *getTexture(GLuint handle);
    Renderbuffer *getRenderbuffer(GLuint handle);
    Sampler *getSampler(GLuint handle);
    FenceSync *getFenceSync(GLuint handle);

    void setRenderbuffer(GLuint handle, Renderbuffer *renderbuffer);

    void checkBufferAllocation(unsigned int buffer);
    void checkTextureAllocation(GLuint texture, GLenum type);
    void checkRenderbufferAllocation(GLuint renderbuffer);
    void checkSamplerAllocation(GLuint sampler);

    bool isSampler(GLuint sampler);

  private:
    DISALLOW_COPY_AND_ASSIGN(ResourceManager);

    rx::Renderer *mRenderer;
    std::size_t mRefCount;

    typedef rx::UnorderedMap<GLuint, Buffer*> BufferMap;
    BufferMap mBufferMap;
    HandleAllocator mBufferHandleAllocator;

    typedef rx::UnorderedMap<GLuint, Shader*> ShaderMap;
    ShaderMap mShaderMap;

    typedef rx::UnorderedMap<GLuint, Program*> ProgramMap;
    ProgramMap mProgramMap;
    HandleAllocator mProgramShaderHandleAllocator;

    typedef rx::UnorderedMap<GLuint, Texture*> TextureMap;
    TextureMap mTextureMap;
    HandleAllocator mTextureHandleAllocator;

    typedef rx::UnorderedMap<GLuint, Renderbuffer*> RenderbufferMap;
    RenderbufferMap mRenderbufferMap;
    HandleAllocator mRenderbufferHandleAllocator;

    typedef rx::UnorderedMap<GLuint, Sampler*> SamplerMap;
    SamplerMap mSamplerMap;
    HandleAllocator mSamplerHandleAllocator;

    typedef rx::UnorderedMap<GLuint, FenceSync*> FenceMap;
    FenceMap mFenceSyncMap;
    HandleAllocator mFenceSyncHandleAllocator;
};

}

#endif // LIBANGLE_RESOURCEMANAGER_H_
