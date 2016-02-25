//
// Copyright 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// TextureVk.h: Defines the class interface for TextureVk.

#ifndef LIBANGLE_RENDERER_VULKAN_TEXTUREVK_H_
#define LIBANGLE_RENDERER_VULKAN_TEXTUREVK_H_

#include "libANGLE/renderer/TextureImpl.h"

namespace rx
{

class TextureVk : public TextureImpl
{
  public:
    TextureVk();
    ~TextureVk() override;

    void setUsage(GLenum usage) override;

    gl::Error setImage(GLenum target,
                       size_t level,
                       GLenum internalFormat,
                       const gl::Extents &size,
                       GLenum format,
                       GLenum type,
                       const gl::PixelUnpackState &unpack,
                       const uint8_t *pixels) override;
    gl::Error setSubImage(GLenum target,
                          size_t level,
                          const gl::Box &area,
                          GLenum format,
                          GLenum type,
                          const gl::PixelUnpackState &unpack,
                          const uint8_t *pixels) override;

    gl::Error setCompressedImage(GLenum target,
                                 size_t level,
                                 GLenum internalFormat,
                                 const gl::Extents &size,
                                 const gl::PixelUnpackState &unpack,
                                 size_t imageSize,
                                 const uint8_t *pixels) override;
    gl::Error setCompressedSubImage(GLenum target,
                                    size_t level,
                                    const gl::Box &area,
                                    GLenum format,
                                    const gl::PixelUnpackState &unpack,
                                    size_t imageSize,
                                    const uint8_t *pixels) override;

    gl::Error copyImage(GLenum target,
                        size_t level,
                        const gl::Rectangle &sourceArea,
                        GLenum internalFormat,
                        const gl::Framebuffer *source) override;
    gl::Error copySubImage(GLenum target,
                           size_t level,
                           const gl::Offset &destOffset,
                           const gl::Rectangle &sourceArea,
                           const gl::Framebuffer *source) override;

    gl::Error setStorage(GLenum target,
                         size_t levels,
                         GLenum internalFormat,
                         const gl::Extents &size) override;

    gl::Error setEGLImageTarget(GLenum target, egl::Image *image) override;

    gl::Error generateMipmaps(const gl::TextureState &textureState) override;

    void bindTexImage(egl::Surface *surface) override;
    void releaseTexImage() override;

    gl::Error getAttachmentRenderTarget(const gl::FramebufferAttachment::Target &target,
                                        FramebufferAttachmentRenderTarget **rtOut) override;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_VULKAN_TEXTUREVK_H_
