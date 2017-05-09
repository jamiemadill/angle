//
// Copyright (c) 2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// SwapChain11.h: Defines a back-end specific class for the D3D11 swap chain.

#ifndef LIBANGLE_RENDERER_D3D_D3D11_SWAPCHAIN11_H_
#define LIBANGLE_RENDERER_D3D_D3D11_SWAPCHAIN11_H_

#include "common/angleutils.h"
#include "libANGLE/renderer/d3d/SwapChainD3D.h"
#include "libANGLE/renderer/d3d/d3d11/RenderTarget11.h"
#include "libANGLE/renderer/d3d/d3d11/ResourceManager11.h"

namespace rx
{
class Renderer11;
class NativeWindow11;

class SwapChain11 final : public SwapChainD3D
{
  public:
    SwapChain11(Renderer11 *renderer,
                NativeWindow11 *nativeWindow,
                HANDLE shareHandle,
                IUnknown *d3dTexture,
                GLenum backBufferFormat,
                GLenum depthBufferFormat,
                EGLint orientation,
                EGLint samples);
    virtual ~SwapChain11();

    EGLint resize(EGLint backbufferWidth, EGLint backbufferHeight);
    EGLint reset(EGLint backbufferWidth, EGLint backbufferHeight, EGLint swapInterval) override;
    EGLint swapRect(EGLint x, EGLint y, EGLint width, EGLint height) override;
    void recreate() override;

    RenderTargetD3D *getColorRenderTarget() override { return &mColorRenderTarget; }
    RenderTargetD3D *getDepthStencilRenderTarget() override { return &mDepthStencilRenderTarget; }

    const SharedResource11 &getOffscreenTexture() const;
    ID3D11RenderTargetView *getRenderTarget();
    ID3D11ShaderResourceView *getRenderTargetShaderResource();

    const SharedResource11 &getDepthStencilTexture() const;
    ID3D11DepthStencilView *getDepthStencil();
    ID3D11ShaderResourceView *getDepthStencilShaderResource();

    EGLint getWidth() const { return mWidth; }
    EGLint getHeight() const { return mHeight; }
    void *getKeyedMutex() override { return mKeyedMutex; }
    EGLint getSamples() const { return mEGLSamples; }

    egl::Error getSyncValues(EGLuint64KHR *ust, EGLuint64KHR *msc, EGLuint64KHR *sbc) override;

  private:
    void release();
    void initPassThroughResources();

    void releaseOffscreenColorBuffer();
    void releaseOffscreenDepthBuffer();
    EGLint resetOffscreenBuffers(int backbufferWidth, int backbufferHeight);
    EGLint resetOffscreenColorBuffer(int backbufferWidth, int backbufferHeight);
    EGLint resetOffscreenDepthBuffer(int backbufferWidth, int backbufferHeight);

    DXGI_FORMAT getSwapChainNativeFormat() const;

    EGLint copyOffscreenToBackbuffer(EGLint x, EGLint y, EGLint width, EGLint height);
    EGLint present(EGLint x, EGLint y, EGLint width, EGLint height);
    UINT getD3DSamples() const;

    Renderer11 *mRenderer;
    EGLint mWidth;
    EGLint mHeight;
    const EGLint mOrientation;
    bool mAppCreatedShareHandle;
    unsigned int mSwapInterval;
    bool mPassThroughResourcesInit;

    NativeWindow11 *mNativeWindow;  // Handler for the Window that the surface is created for.

    bool mFirstSwap;
    IDXGISwapChain *mSwapChain;
    IDXGISwapChain1 *mSwapChain1;
    IDXGIKeyedMutex *mKeyedMutex;

    SharedResource11 mBackBufferTexture;
    ID3D11RenderTargetView *mBackBufferRTView;
    ID3D11ShaderResourceView *mBackBufferSRView;

    const bool mNeedsOffscreenTexture;
    SharedResource11 mOffscreenTexture;
    ID3D11RenderTargetView *mOffscreenRTView;
    ID3D11ShaderResourceView *mOffscreenSRView;

    SharedResource11 mDepthStencilTexture;
    ID3D11DepthStencilView *mDepthStencilDSView;
    ID3D11ShaderResourceView *mDepthStencilSRView;

    ID3D11Buffer *mQuadVB;
    ID3D11SamplerState *mPassThroughSampler;
    ID3D11InputLayout *mPassThroughIL;
    ID3D11VertexShader *mPassThroughVS;
    ID3D11PixelShader *mPassThroughPS;
    ID3D11RasterizerState *mPassThroughRS;

    SurfaceRenderTarget11 mColorRenderTarget;
    SurfaceRenderTarget11 mDepthStencilRenderTarget;

    EGLint mEGLSamples;
    LONGLONG mQPCFrequency;
};

}  // namespace rx
#endif // LIBANGLE_RENDERER_D3D_D3D11_SWAPCHAIN11_H_
