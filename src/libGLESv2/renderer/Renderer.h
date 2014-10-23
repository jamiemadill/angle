//
// Copyright (c) 2012-2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// Renderer.h: Defines a back-end specific class that hides the details of the
// implementation-specific renderer.

#ifndef LIBGLESV2_RENDERER_RENDERER_H_
#define LIBGLESV2_RENDERER_RENDERER_H_

#include "libGLESv2/Uniform.h"
#include "libGLESv2/angletypes.h"
#include "libGLESv2/Caps.h"
#include "common/NativeWindow.h"
#include "libGLESv2/Error.h"
#include "libGLESv2/renderer/Workarounds.h"

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
class RendererImpl;
struct TranslatedIndexData;

struct ConfigDesc
{
    GLenum  renderTargetFormat;
    GLenum  depthStencilFormat;
    GLint   multiSample;
    bool    fastConfig;
    bool    es3Capable;
};

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

class Renderer final
{
  public:
    Renderer(rx::RendererImpl *implementation);
    ~Renderer();

    EGLint initialize();
    bool resetDevice();

    // TODO(jmadill): figure out ConfigDesc namespace
    int generateConfigs(rx::ConfigDesc **configDescList);
    void deleteConfigs(rx::ConfigDesc *configDescList);

    gl::Error sync(bool block);

    // TODO(jmadill): pass state and essetial params only
    gl::Error drawArrays(GLenum mode, GLsizei count, GLsizei instances, bool transformFeedbackActive);
    gl::Error drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices,
                           gl::Buffer *elementArrayBuffer, const rx::TranslatedIndexData &indexInfo, GLsizei instances);
    gl::Error clear(const gl::ClearParameters &clearParams, gl::Framebuffer *frameBuffer);
    gl::Error readPixels(gl::Framebuffer *framebuffer, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format,
                         GLenum type, GLuint outputPitch, const gl::PixelPackState &pack, uint8_t *pixels);

    // TODO(jmadill): caps?
    bool getShareHandleSupport() const;
    bool getPostSubBufferSupport() const;

    // Shader creation
    rx::ShaderImpl *createShader(GLenum type);
    rx::ProgramImpl *createProgram();

    // Texture creation
    rx::TextureImpl *createTexture(GLenum target);

    // Buffer creation
    rx::BufferImpl *createBuffer();

    // Vertex Array creation
    rx::VertexArrayImpl *createVertexArray();

    // Query and Fence creation
    rx::QueryImpl *createQuery(GLenum type);
    rx::FenceNVImpl *createFenceNV();
    rx::FenceSyncImpl *createFenceSync();

    // Transform Feedback creation
    rx::TransformFeedbackImpl* createTransformFeedback();

    rx::RendererImpl *getImplementation() { return mImplementation; }
    const rx::RendererImpl *getImplementation() const { return mImplementation; }

    // lost device
    //TODO(jmadill): investigate if this stuff is necessary in GL
    void notifyDeviceLost();
    bool isDeviceLost();
    bool testDeviceLost(bool notify);
    bool testDeviceResettable();

  private:
    rx::RendererImpl *mImplementation;
};

}

namespace rx
{
class TextureStorage;
class VertexBuffer;
class IndexBuffer;
class ShaderExecutable;
class SwapChain;
class RenderTarget;
class Image;
class TextureStorage;
class UniformStorage;

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
