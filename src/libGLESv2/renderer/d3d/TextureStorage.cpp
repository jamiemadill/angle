//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// TextureStorage.cpp: Implements the abstract rx::TextureStorageInterface class and its concrete derived
// classes TextureStorageInterface2D and TextureStorageInterfaceCube, which act as the interface to the
// GPU-side texture.

#include "libGLESv2/renderer/d3d/TextureStorage.h"
#include "libGLESv2/renderer/d3d/TextureD3D.h"
#include "libGLESv2/renderer/Renderer.h"
#include "libGLESv2/Renderbuffer.h"
#include "libGLESv2/Texture.h"

#include "common/debug.h"
#include "common/mathutil.h"

namespace rx
{
unsigned int TextureStorageInterface::mCurrentTextureSerial = 1;

TextureStorageInterface::TextureStorageInterface(TextureStorage *textureStorage, unsigned int layerCount)
    : mTextureSerial(issueTextureSerial()),
      mInstance(textureStorage),
      mFirstRenderTargetSerial(gl::RenderbufferStorage::issueSerials(static_cast<unsigned int>(textureStorage->getLevelCount()) * layerCount)),
      mMipLevelCount(static_cast<unsigned int>(textureStorage->getLevelCount()))
{}

TextureStorageInterface::~TextureStorageInterface()
{
    delete mInstance;
}

bool TextureStorageInterface::isRenderTarget() const
{
    return mInstance->isRenderTarget();
}

bool TextureStorageInterface::isManaged() const
{
    return mInstance->isManaged();
}

unsigned int TextureStorageInterface::getTextureSerial() const
{
    return mTextureSerial;
}

unsigned int TextureStorageInterface::issueTextureSerial()
{
    return mCurrentTextureSerial++;
}

int TextureStorageInterface::getTopLevel() const
{
    return mInstance->getTopLevel();
}

int TextureStorageInterface::getLevelCount() const
{
    return mInstance->getLevelCount();
}

unsigned int TextureStorageInterface::getRenderTargetSerial(const gl::ImageIndex &index) const
{
    unsigned int layerOffset = (index.hasLayer() ? (static_cast<unsigned int>(index.layerIndex) * mMipLevelCount) : 0);
    return mFirstRenderTargetSerial + static_cast<unsigned int>(index.mipIndex) + layerOffset;
}

TextureStorageInterface2D::TextureStorageInterface2D(Renderer *renderer, SwapChain *swapchain)
    : TextureStorageInterface(renderer->createTextureStorage2D(swapchain), 0)
{}

TextureStorageInterface2D::TextureStorageInterface2D(TextureStorage *storageInstance)
    : TextureStorageInterface(storageInstance, 0)
{}

TextureStorageInterface2D::~TextureStorageInterface2D()
{}

void TextureStorageInterface2D::generateMipmap(int level)
{
    mInstance->generateMipmap(level);
}

TextureStorageInterfaceCube::TextureStorageInterfaceCube(TextureStorage *storageInstance)
    : TextureStorageInterface(storageInstance, 6)
{}

TextureStorageInterfaceCube::~TextureStorageInterfaceCube()
{}

void TextureStorageInterfaceCube::generateMipmap(int faceIndex, int level)
{
    mInstance->generateMipmap(faceIndex, level);
}

TextureStorageInterface3D::TextureStorageInterface3D(TextureStorage *storageInstance, unsigned int depth)
    : TextureStorageInterface(storageInstance, depth)
{}

TextureStorageInterface3D::~TextureStorageInterface3D()
{}

void TextureStorageInterface3D::generateMipmap(int level)
{
    mInstance->generateMipmap(level);
}

TextureStorageInterface2DArray::TextureStorageInterface2DArray(TextureStorage *storageInstance, unsigned int depth)
    : TextureStorageInterface(storageInstance, depth)
{}

TextureStorageInterface2DArray::~TextureStorageInterface2DArray()
{}

void TextureStorageInterface2DArray::generateMipmap(int level)
{
    mInstance->generateMipmap(level);
}

}
