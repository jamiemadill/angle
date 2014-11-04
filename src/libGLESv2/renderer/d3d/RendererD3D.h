// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// RendererD3D.h: Defines a back-end specific class for the DirectX renderer.

#ifndef LIBGLESV2_RENDERER_RENDERERD3D_H_
#define LIBGLESV2_RENDERER_RENDERERD3D_H_

#include "libGLESv2/renderer/Renderer.h"
#include "libGLESv2/Data.h"

//FIXME(jmadill): std::array is currently prohibited by Chromium style guide
#include <array>

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

class RendererD3D : public Renderer
{
  public:
    explicit RendererD3D(egl::Display *display);
    virtual ~RendererD3D();

    static RendererD3D *makeRendererD3D(Renderer *renderer);

    gl::Error drawArrays(const gl::Data &data,
                         GLenum mode, GLint first,
                         GLsizei count, GLsizei instances) override;

    gl::Error drawElements(const gl::Data &data,
                           GLenum mode, GLsizei count, GLenum type,
                           const GLvoid *indices, GLsizei instances,
                           const RangeUI &indexRange) override;

    gl::Error clear(const gl::Data &data, GLbitfield mask) override;
    gl::Error clearBufferfv(const gl::Data &data, GLenum buffer, int drawbuffer, const GLfloat *values) override;
    gl::Error clearBufferuiv(const gl::Data &data, GLenum buffer, int drawbuffer, const GLuint *values) override;
    gl::Error clearBufferiv(const gl::Data &data, GLenum buffer, int drawbuffer, const GLint *values) override;
    gl::Error clearBufferfi(const gl::Data &data, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) override;

    // Direct3D Specific methods
    virtual SwapChain *createSwapChain(rx::NativeWindow nativeWindow, HANDLE shareHandle, GLenum backBufferFormat, GLenum depthBufferFormat) = 0;

    virtual gl::Error generateSwizzle(gl::Texture *texture) = 0;
    virtual gl::Error setSamplerState(gl::SamplerType type, int index, gl::Texture *texture, const gl::SamplerState &sampler) = 0;
    virtual gl::Error setTexture(gl::SamplerType type, int index, gl::Texture *texture) = 0;

    virtual gl::Error setUniformBuffers(const gl::Buffer *vertexUniformBuffers[], const gl::Buffer *fragmentUniformBuffers[]) = 0;

    virtual gl::Error setRasterizerState(const gl::RasterizerState &rasterState) = 0;
    virtual gl::Error setBlendState(const gl::Framebuffer *framebuffer, const gl::BlendState &blendState, const gl::ColorF &blendColor,
                                    unsigned int sampleMask) = 0;
    virtual gl::Error setDepthStencilState(const gl::DepthStencilState &depthStencilState, int stencilRef,
                                           int stencilBackRef, bool frontFaceCCW) = 0;

    virtual void setScissorRectangle(const gl::Rectangle &scissor, bool enabled) = 0;
    virtual void setViewport(const gl::Rectangle &viewport, float zNear, float zFar, GLenum drawMode, GLenum frontFace,
                             bool ignoreViewport) = 0;

    virtual gl::Error applyRenderTarget(const gl::Framebuffer *frameBuffer) = 0;
    virtual gl::Error applyShaders(gl::ProgramBinary *programBinary, const gl::VertexFormat inputLayout[], const gl::Framebuffer *framebuffer,
                                   bool rasterizerDiscard, bool transformFeedbackActive) = 0;
    virtual gl::Error applyUniforms(const ProgramImpl &program, const std::vector<gl::LinkedUniform*> &uniformArray) = 0;
    virtual bool applyPrimitiveType(GLenum primitiveType, GLsizei elementCount) = 0;
    virtual gl::Error applyVertexBuffer(const gl::State &state, GLint first, GLsizei count, GLsizei instances) = 0;
    virtual gl::Error applyIndexBuffer(const GLvoid *indices, gl::Buffer *elementArrayBuffer, GLsizei count, GLenum mode, GLenum type, TranslatedIndexData *indexInfo) = 0;
    virtual void applyTransformFeedbackBuffers(const gl::State& state) = 0;

    virtual void markAllStateDirty() = 0;

    virtual unsigned int getReservedVertexUniformVectors() const = 0;
    virtual unsigned int getReservedFragmentUniformVectors() const = 0;
    virtual unsigned int getReservedVertexUniformBuffers() const = 0;
    virtual unsigned int getReservedFragmentUniformBuffers() const = 0;
    virtual bool getShareHandleSupport() const = 0;
    virtual bool getPostSubBufferSupport() const = 0;

    // Pixel operations
    virtual gl::Error copyImage2D(gl::Framebuffer *framebuffer, const gl::Rectangle &sourceRect, GLenum destFormat,
                                  GLint xoffset, GLint yoffset, TextureStorage *storage, GLint level) = 0;
    virtual gl::Error copyImageCube(gl::Framebuffer *framebuffer, const gl::Rectangle &sourceRect, GLenum destFormat,
                                    GLint xoffset, GLint yoffset, TextureStorage *storage, GLenum target, GLint level) = 0;
    virtual gl::Error copyImage3D(gl::Framebuffer *framebuffer, const gl::Rectangle &sourceRect, GLenum destFormat,
                                  GLint xoffset, GLint yoffset, GLint zOffset, TextureStorage *storage, GLint level) = 0;
    virtual gl::Error copyImage2DArray(gl::Framebuffer *framebuffer, const gl::Rectangle &sourceRect, GLenum destFormat,
                                       GLint xoffset, GLint yoffset, GLint zOffset, TextureStorage *storage, GLint level) = 0;

    virtual gl::Error blitRect(gl::Framebuffer *readTarget, const gl::Rectangle &readRect, gl::Framebuffer *drawTarget, const gl::Rectangle &drawRect,
                               const gl::Rectangle *scissor, bool blitRenderTarget, bool blitDepth, bool blitStencil, GLenum filter) = 0;

    virtual gl::Error readPixels(gl::Framebuffer *framebuffer, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format,
                                 GLenum type, GLuint outputPitch, const gl::PixelPackState &pack, uint8_t *pixels) = 0;

    // RenderTarget creation
    virtual RenderTarget *createRenderTarget(SwapChain *swapChain, bool depth) = 0;
    virtual RenderTarget *createRenderTarget(int width, int height, GLenum format, GLsizei samples) = 0;

    // Shader operations
    virtual void releaseShaderCompiler() = 0;
    virtual gl::Error loadExecutable(const void *function, size_t length, rx::ShaderType type,
                                     const std::vector<gl::LinkedVarying> &transformFeedbackVaryings,
                                     bool separatedOutputBuffers, ShaderExecutable **outExecutable) = 0;
    virtual gl::Error compileToExecutable(gl::InfoLog &infoLog, const std::string &shaderHLSL, rx::ShaderType type,
                                          const std::vector<gl::LinkedVarying> &transformFeedbackVaryings,
                                          bool separatedOutputBuffers, D3DWorkaroundType workaround,
                                          ShaderExecutable **outExectuable) = 0;
    virtual UniformStorage *createUniformStorage(size_t storageSize) = 0;

    // Image operations
    virtual Image *createImage() = 0;
    virtual gl::Error generateMipmap(Image *dest, Image *source) = 0;
    virtual TextureStorage *createTextureStorage2D(SwapChain *swapChain) = 0;
    virtual TextureStorage *createTextureStorage2D(GLenum internalformat, bool renderTarget, GLsizei width, GLsizei height, int levels) = 0;
    virtual TextureStorage *createTextureStorageCube(GLenum internalformat, bool renderTarget, int size, int levels) = 0;
    virtual TextureStorage *createTextureStorage3D(GLenum internalformat, bool renderTarget, GLsizei width, GLsizei height, GLsizei depth, int levels) = 0;
    virtual TextureStorage *createTextureStorage2DArray(GLenum internalformat, bool renderTarget, GLsizei width, GLsizei height, GLsizei depth, int levels) = 0;

    // Current GLES client version
    void setCurrentClientVersion(int clientVersion) { mCurrentClientVersion = clientVersion; }
    int getCurrentClientVersion() const { return mCurrentClientVersion; }

    // Buffer-to-texture and Texture-to-buffer copies
    virtual bool supportsFastCopyBufferToTexture(GLenum internalFormat) const = 0;
    virtual gl::Error fastCopyBufferToTexture(const gl::PixelUnpackState &unpack, unsigned int offset, RenderTarget *destRenderTarget,
                                              GLenum destinationFormat, GLenum sourcePixelsType, const gl::Box &destArea) = 0;

    virtual rx::VertexConversionType getVertexConversionType(const gl::VertexFormat &vertexFormat) const = 0;
    virtual GLenum getVertexComponentType(const gl::VertexFormat &vertexFormat) const = 0;

    virtual VertexBuffer *createVertexBuffer() = 0;
    virtual IndexBuffer *createIndexBuffer() = 0;

  protected:
    virtual gl::Error drawArrays(GLenum mode, GLsizei count, GLsizei instances, bool transformFeedbackActive) = 0;
    virtual gl::Error drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices,
                                   gl::Buffer *elementArrayBuffer, const TranslatedIndexData &indexInfo, GLsizei instances) = 0;
    virtual gl::Error clear(const gl::ClearParameters &clearParams, const gl::Framebuffer *frameBuffer) = 0;

    egl::Display *mDisplay;

  private:
    DISALLOW_COPY_AND_ASSIGN(RendererD3D);

    //FIXME(jmadill): std::array is currently prohibited by Chromium style guide
    typedef std::array<unsigned int, gl::IMPLEMENTATION_MAX_FRAMEBUFFER_ATTACHMENTS> FramebufferTextureSerialArray;

    gl::Error generateSwizzles(const gl::Data &data, gl::SamplerType type);
    gl::Error generateSwizzles(const gl::Data &data);

    gl::Error applyRenderTarget(const gl::Data &data, GLenum drawMode, bool ignoreViewport);
    gl::Error applyState(const gl::Data &data, GLenum drawMode);
    bool applyTransformFeedbackBuffers(const gl::Data &data);
    gl::Error applyShaders(const gl::Data &data, bool transformFeedbackActive);
    gl::Error applyTextures(const gl::Data &data, gl::SamplerType shaderType,
                            const FramebufferTextureSerialArray &framebufferSerials, size_t framebufferSerialCount);
    gl::Error applyTextures(const gl::Data &data);
    gl::Error applyUniformBuffers(const gl::Data &data);

    bool skipDraw(const gl::Data &data, GLenum drawMode);
    void markTransformFeedbackUsage(const gl::Data &data);

    size_t getBoundFramebufferTextureSerials(const gl::Data &data,
                                             FramebufferTextureSerialArray *outSerialArray);
    gl::Texture *getIncompleteTexture(GLenum type);

    int mCurrentClientVersion;
    gl::TextureMap mIncompleteTextures;
};

}

#endif // LIBGLESV2_RENDERER_RENDERERD3D_H_
