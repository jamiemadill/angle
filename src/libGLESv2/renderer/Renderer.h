//
// Copyright (c) 2012-2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// Renderer.h: Defines a back-end specific class that hides the details of the
// implementation-specific renderer.

#ifndef LIBGLESV2_RENDERER_RENDERER_H_
#define LIBGLESV2_RENDERER_RENDERER_H_

#include "libGLESv2/Caps.h"
#include "libGLESv2/Error.h"
#include "libGLESv2/Uniform.h"
#include "libGLESv2/angletypes.h"
#include "libGLESv2/renderer/Workarounds.h"
#include "common/NativeWindow.h"
#include "common/mathutil.h"

#include <cstdint>

#include <EGL/egl.h>

#if !defined(ANGLE_COMPILE_OPTIMIZATION_LEVEL)
// WARNING: D3DCOMPILE_OPTIMIZATION_LEVEL3 may lead to a DX9 shader compiler hang.
// It should only be used selectively to work around specific bugs.
#define ANGLE_COMPILE_OPTIMIZATION_LEVEL D3DCOMPILE_OPTIMIZATION_LEVEL1
#endif

namespace egl
{
class Display;
}

namespace gl
{
class InfoLog;
class ProgramBinary;
struct LinkedVarying;
struct VertexAttribute;
class Buffer;
class Texture;
class Framebuffer;
struct VertexAttribCurrentValueData;
struct Data;
}

namespace rx
{
class QueryImpl;
class FenceNVImpl;
class FenceSyncImpl;
class BufferImpl;
class VertexArrayImpl;
class BufferStorage;
class ShaderImpl;
class ProgramImpl;
class TextureImpl;
class TransformFeedbackImpl;
struct TranslatedIndexData;
struct Workarounds;

struct ConfigDesc
{
    GLenum  renderTargetFormat;
    GLenum  depthStencilFormat;
    GLint   multiSample;
    bool    fastConfig;
    bool    es3Capable;
};

class Renderer
{
  public:
    Renderer();
    virtual ~Renderer();

    virtual EGLint initialize() = 0;
    virtual bool resetDevice() = 0;

    virtual int generateConfigs(rx::ConfigDesc **configDescList) = 0;
    virtual void deleteConfigs(rx::ConfigDesc *configDescList) = 0;

    virtual gl::Error sync(bool block) = 0;

    // TODO(jmadill): pass state and essetial params only
    virtual gl::Error drawArrays(const gl::Data &data, GLenum mode,
                                 GLint first, GLsizei count, GLsizei instances) = 0;
    virtual gl::Error drawElements(const gl::Data &data, GLenum mode, GLsizei count, GLenum type,
                                   const GLvoid *indices, GLsizei instances,
                                   const rx::RangeUI &indexRange) = 0;
    virtual gl::Error clear(const gl::ClearParameters &clearParams, gl::Framebuffer *frameBuffer) = 0;
    virtual gl::Error readPixels(gl::Framebuffer *framebuffer, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format,
                                 GLenum type, GLuint outputPitch, const gl::PixelPackState &pack, uint8_t *pixels) = 0;

    // TODO(jmadill): caps? and virtual for egl::Display
    virtual bool getShareHandleSupport() const = 0;
    virtual bool getPostSubBufferSupport() const = 0;

    // Shader creation
    virtual rx::ShaderImpl *createShader(GLenum type) = 0;
    virtual rx::ProgramImpl *createProgram() = 0;

    // Shader operations
    virtual void releaseShaderCompiler() = 0;

    // Texture creation
    virtual rx::TextureImpl *createTexture(GLenum target) = 0;

    // Buffer creation
    virtual rx::BufferImpl *createBuffer() = 0;

    // Vertex Array creation
    virtual rx::VertexArrayImpl *createVertexArray() = 0;

    // Query and Fence creation
    virtual rx::QueryImpl *createQuery(GLenum type) = 0;
    virtual rx::FenceNVImpl *createFenceNV() = 0;
    virtual rx::FenceSyncImpl *createFenceSync() = 0;

    // Transform Feedback creation
    virtual rx::TransformFeedbackImpl *createTransformFeedback() = 0;

    // lost device
    //TODO(jmadill): investigate if this stuff is necessary in GL
    virtual void notifyDeviceLost() = 0;
    virtual bool isDeviceLost() = 0;
    virtual bool testDeviceLost(bool notify) = 0;
    virtual bool testDeviceResettable() = 0;

    virtual DWORD getAdapterVendor() const = 0;
    virtual std::string getRendererDescription() const = 0;
    virtual GUID getAdapterIdentifier() const = 0;

    // Renderer capabilities (virtual because of egl::Display)
    virtual const gl::Caps &getRendererCaps() const;
    const gl::TextureCapsMap &getRendererTextureCaps() const;
    virtual const gl::Extensions &getRendererExtensions() const;
    const rx::Workarounds &getWorkarounds() const;

    // TODO(jmadill): needed by egl::Display, probably should be removed
    virtual int getMajorShaderModel() const = 0;
    virtual int getMinSwapInterval() const = 0;
    virtual int getMaxSwapInterval() const = 0;
    virtual bool getLUID(LUID *adapterLuid) const = 0;

  private:
    DISALLOW_COPY_AND_ASSIGN(Renderer);

    virtual void generateCaps(gl::Caps *outCaps, gl::TextureCapsMap* outTextureCaps, gl::Extensions *outExtensions) const = 0;
    virtual rx::Workarounds generateWorkarounds() const = 0;

    mutable bool mCapsInitialized;
    mutable gl::Caps mCaps;
    mutable gl::TextureCapsMap mTextureCaps;
    mutable gl::Extensions mExtensions;

    mutable bool mWorkaroundsInitialized;
    mutable rx::Workarounds mWorkarounds;
};

struct dx_VertexConstants
{
    float depthRange[4];
    float viewAdjust[4];
};

struct dx_PixelConstants
{
    float depthRange[4];
    float viewCoords[4];
    float depthFront[4];
};

enum ShaderType
{
    SHADER_VERTEX,
    SHADER_PIXEL,
    SHADER_GEOMETRY
};

}
#endif // LIBGLESV2_RENDERER_RENDERER_H_
