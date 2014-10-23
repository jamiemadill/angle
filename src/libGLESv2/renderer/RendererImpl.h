//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// RendererImpl.h: Defines the interface for a back-end renderer implementation.
//

#ifndef LIBGLESV2_RENDERER_RENDERERIMPL_H_
#define LIBGLESV2_RENDERER_RENDERERIMPL_H_

#include "libGLESv2/renderer/Renderer.h"

namespace rx
{

class RendererImpl
{
  public:
    virtual EGLint initialize() = 0;
    virtual bool resetDevice() = 0;

    virtual int generateConfigs(ConfigDesc **configDescList) = 0;
    virtual void deleteConfigs(ConfigDesc *configDescList) = 0;

    virtual gl::Error sync(bool block) = 0;

    virtual gl::Error drawArrays(GLenum mode, GLsizei count, GLsizei instances, bool transformFeedbackActive) = 0;
    virtual gl::Error drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices,
                                   gl::Buffer *elementArrayBuffer, const TranslatedIndexData &indexInfo, GLsizei instances) = 0;
    virtual gl::Error clear(const gl::ClearParameters &clearParams, gl::Framebuffer *frameBuffer) = 0;
    virtual gl::Error readPixels(gl::Framebuffer *framebuffer, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format,
                                 GLenum type, GLuint outputPitch, const gl::PixelPackState &pack, uint8_t *pixels) = 0;

    // TODO(jmadill): caps?
    virtual bool getShareHandleSupport() const = 0;
    virtual bool getPostSubBufferSupport() const = 0;

    // Shader creation
    virtual ShaderImpl *createShader(GLenum type) = 0;
    virtual ProgramImpl *createProgram() = 0;

    // Texture creation
    virtual TextureImpl *createTexture(GLenum target) = 0;

    // Buffer creation
    virtual BufferImpl *createBuffer() = 0;

    // Vertex Array creation
    virtual VertexArrayImpl *createVertexArray() = 0;

    // Query and Fence creation
    virtual QueryImpl *createQuery(GLenum type) = 0;
    virtual FenceNVImpl *createFenceNV() = 0;
    virtual FenceSyncImpl *createFenceSync() = 0;

    // Transform Feedback creation
    virtual TransformFeedbackImpl* createTransformFeedback() = 0;

    // lost device
    //TODO(jmadill): investigate if this stuff is necessary in GL
    void notifyDeviceLost();
    bool isDeviceLost();
    bool testDeviceLost(bool notify);
    bool testDeviceResettable();
};

}

#endif // LIBGLESV2_RENDERER_RENDERERIMPL_H_
