//
// Copyright 2017 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ResourceManager11:
//   Centralized point of allocation for all D3D11 Buffers and Textures.

#include "libANGLE/renderer/d3d/d3d11/ResourceManager11.h"

#include "common/debug.h"
#include "libANGLE/renderer/d3d/d3d11/Renderer11.h"
#include "libANGLE/renderer/d3d/d3d11/formatutils11.h"

namespace rx
{

namespace
{
size_t ComputeMippedMemoryUsage(size_t baseLevelSize, size_t mipLevels)
{
    size_t sizeSum = 0;
    for (size_t level = 0; level < mipLevels; ++level)
    {
        sizeSum += baseLevelSize >> level;
    }
    return sizeSum;
}

size_t ComputeMemoryUsage(const D3D11_TEXTURE2D_DESC *desc)
{
    ASSERT(desc);
    size_t pixelBytes = static_cast<size_t>(d3d11::GetDXGIFormatSizeInfo(desc->Format).pixelBytes);
    size_t levelSize =
        static_cast<size_t>(desc->Width * desc->Height * desc->ArraySize) * pixelBytes;
    return ComputeMippedMemoryUsage(levelSize, static_cast<size_t>(desc->MipLevels));
}

size_t ComputeMemoryUsage(const D3D11_TEXTURE3D_DESC *desc)
{
    ASSERT(desc);
    size_t pixelBytes = static_cast<size_t>(d3d11::GetDXGIFormatSizeInfo(desc->Format).pixelBytes);
    size_t levelSize  = static_cast<size_t>(desc->Width * desc->Height * desc->Depth) * pixelBytes;
    return ComputeMippedMemoryUsage(levelSize, static_cast<size_t>(desc->MipLevels));
}

size_t ComputeMemoryUsage(const D3D11_BUFFER_DESC *desc)
{
    ASSERT(desc);
    return static_cast<size_t>(desc->ByteWidth);
}

template <typename T>
size_t ComputeMemoryUsage(const T *desc)
{
    return 0;
}

template <ResourceType ResourceT>
size_t ComputeGenericMemoryUsage(ID3D11DeviceChild *genericResource)
{
    auto *typedResource = static_cast<GetD3D11Type<ResourceT> *>(genericResource);
    GetDescType<ResourceT> desc;
    typedResource->GetDesc(&desc);
    return ComputeMemoryUsage(&desc);
}

size_t ComputeGenericMemoryUsage(ResourceType resourceType, ID3D11DeviceChild *resource)
{
    switch (resourceType)
    {
        case ResourceType::Texture2D:
            return ComputeGenericMemoryUsage<ResourceType::Texture2D>(resource);
        case ResourceType::Texture3D:
            return ComputeGenericMemoryUsage<ResourceType::Texture3D>(resource);
        case ResourceType::Buffer:
            return ComputeGenericMemoryUsage<ResourceType::Buffer>(resource);

        default:
            return 0;
    }
}

HRESULT CreateResource(ID3D11Device *device,
                       const D3D11_TEXTURE2D_DESC *desc,
                       const D3D11_SUBRESOURCE_DATA *initData,
                       ID3D11Texture2D **texture)
{
    return device->CreateTexture2D(desc, initData, texture);
}

HRESULT CreateResource(ID3D11Device *device,
                       const D3D11_TEXTURE3D_DESC *desc,
                       const D3D11_SUBRESOURCE_DATA *initData,
                       ID3D11Texture3D **texture)
{
    return device->CreateTexture3D(desc, initData, texture);
}

HRESULT CreateResource(ID3D11Device *device,
                       const D3D11_BUFFER_DESC *desc,
                       const D3D11_SUBRESOURCE_DATA *initData,
                       ID3D11Buffer **buffer)
{
    return device->CreateBuffer(desc, initData, buffer);
}

HRESULT CreateResource(ID3D11Device *device,
                       const D3D11_RENDER_TARGET_VIEW_DESC *desc,
                       ID3D11Resource *resource,
                       ID3D11RenderTargetView **renderTargetView)
{
    return device->CreateRenderTargetView(resource, desc, renderTargetView);
}

HRESULT CreateResource(ID3D11Device *device,
                       const D3D11_SHADER_RESOURCE_VIEW_DESC *desc,
                       ID3D11Resource *resource,
                       ID3D11ShaderResourceView **resourceOut)
{
    return device->CreateShaderResourceView(resource, desc, resourceOut);
}

HRESULT CreateResource(ID3D11Device *device,
                       const D3D11_DEPTH_STENCIL_VIEW_DESC *desc,
                       ID3D11Resource *resource,
                       ID3D11DepthStencilView **resourceOut)
{
    return device->CreateDepthStencilView(resource, desc, resourceOut);
}

DXGI_FORMAT GetTypedDepthStencilFormat(DXGI_FORMAT dxgiFormat)
{
    switch (dxgiFormat)
    {
        case DXGI_FORMAT_R16_TYPELESS:
            return DXGI_FORMAT_D16_UNORM;
        case DXGI_FORMAT_R24G8_TYPELESS:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case DXGI_FORMAT_R32_TYPELESS:
            return DXGI_FORMAT_D32_FLOAT;
        case DXGI_FORMAT_R32G8X24_TYPELESS:
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        default:
            return dxgiFormat;
    }
}

template <typename T>
gl::Error ClearResource(Renderer11 *renderer, const GetDescFromD3D11<T> *desc, T *texture);

template <>
gl::Error ClearResource<ID3D11Texture2D>(Renderer11 *renderer,
                                         const D3D11_TEXTURE2D_DESC *desc,
                                         ID3D11Texture2D *texture)
{
    ID3D11DeviceContext *context = renderer->getDeviceContext();

    if ((desc->BindFlags & D3D11_BIND_DEPTH_STENCIL) != 0)
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        dsvDesc.Flags  = 0;
        dsvDesc.Format = GetTypedDepthStencilFormat(desc->Format);

        const auto &format = d3d11_angle::GetFormat(dsvDesc.Format);
        UINT clearFlags    = (format.depthBits > 0 ? D3D11_CLEAR_DEPTH : 0) |
                          (format.stencilBits > 0 ? D3D11_CLEAR_STENCIL : 0);

        // Must process each mip level individually.
        for (UINT mipLevel = 0; mipLevel < desc->MipLevels; ++mipLevel)
        {
            if (desc->SampleDesc.Count == 0)
            {
                dsvDesc.Texture2D.MipSlice = mipLevel;
                dsvDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
            }
            else
            {
                dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            }

            d3d11::DepthStencilView dsv;
            ANGLE_TRY(renderer->allocateResource(dsvDesc, texture, &dsv));

            context->ClearDepthStencilView(dsv.get(), clearFlags, 0.2f, 3);
        }
    }
    else
    {
        ASSERT((desc->BindFlags & D3D11_BIND_RENDER_TARGET) != 0);
        d3d11::RenderTargetView rtv;
        ANGLE_TRY(renderer->allocateResourceNoDesc(texture, &rtv));

        const FLOAT zero[4] = {0.3f, 0.5f, 0.7f, 0.5f};
        context->ClearRenderTargetView(rtv.get(), zero);
    }

    return gl::NoError();
}

template <typename T>
gl::Error ClearResource(Renderer11 *renderer, const GetDescFromD3D11<T> *desc, T *texture)
{
    // No-op.
    return gl::NoError();
}

constexpr std::array<const char *, NumResourceTypes> kResourceTypeNames = {{
    "Texture", "Texture3D", "Buffer", "RenderTargetView", "ShaderResourceView", "DepthStencilView",
}};
}  // anonymous namespace

// ResourceManager11 Implementation.
ResourceManager11::ResourceManager11()
    : mAllocatedResourceCounts({{}}), mAllocatedResourceDeviceMemory({{}})
{
}

ResourceManager11::~ResourceManager11()
{
    for (size_t count : mAllocatedResourceCounts)
    {
        ASSERT(count == 0);
    }

    for (size_t memorySize : mAllocatedResourceDeviceMemory)
    {
        ASSERT(memorySize == 0);
    }
}

template <>
const D3D11_SUBRESOURCE_DATA *ResourceManager11::createInitDataIfNeeded<ResourceType::Texture2D>(
    const D3D11_TEXTURE2D_DESC *desc)
{
    ASSERT(desc);

    if ((desc->BindFlags & (D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_RENDER_TARGET)) != 0)
    {
        // This will be done using ClearView methods.
        return nullptr;
    }

    size_t requiredSize = ComputeMemoryUsage(desc);
    if (mZeroMemory.size() < requiredSize)
    {
        mZeroMemory.resize(requiredSize);
        mZeroMemory.fill(0x48);
    }

    const auto &formatSizeInfo = d3d11::GetDXGIFormatSizeInfo(desc->Format);

    UINT subresourceCount = desc->MipLevels * desc->ArraySize;
    if (mShadowInitData.size() < subresourceCount)
    {
        mShadowInitData.resize(subresourceCount);
    }

    for (UINT mipLevel = 0; mipLevel < desc->MipLevels; ++mipLevel)
    {
        for (UINT arrayIndex = 0; arrayIndex < desc->ArraySize; ++arrayIndex)
        {
            UINT subresourceIndex = D3D11CalcSubresource(mipLevel, arrayIndex, desc->MipLevels);
            D3D11_SUBRESOURCE_DATA *data = &mShadowInitData[subresourceIndex];

            UINT levelWidth = desc->Width >> mipLevel;

            data->SysMemPitch      = levelWidth * formatSizeInfo.pixelBytes;
            data->SysMemSlicePitch = data->SysMemPitch * desc->Height;
            data->pSysMem          = mZeroMemory.data();
        }
    }

    return mShadowInitData.data();
}

template <ResourceType Type>
GetInitDataType<Type> *ResourceManager11::createInitDataIfNeeded(const GetDescType<Type> *desc)
{
    // No-op.
    return nullptr;
}

template <typename T>
gl::Error ResourceManager11::allocate(Renderer11 *renderer,
                                      const GetDescFromD3D11<T> *desc,
                                      GetInitDataFromD3D11<T> *initData,
                                      Resource11<T> *resourceOut)
{
    ID3D11Device *device = renderer->getDevice();
    T *resource          = nullptr;

    GetInitDataFromD3D11<T> *shadowInitData = initData;
    if (!shadowInitData)
    {
        shadowInitData = createInitDataIfNeeded<GetResourceTypeFromD3D11<T>()>(desc);
    }

    HRESULT hr = CreateResource(device, desc, shadowInitData, &resource);

    if (FAILED(hr))
    {
        ASSERT(!resource);
        if (d3d11::isDeviceLostError(hr))
        {
            renderer->notifyDeviceLost();
        }
        return gl::OutOfMemory() << "Error allocating "
                                 << std::string(kResourceTypeNames[ResourceTypeIndex<T>()]) << ". "
                                 << gl::FmtHR(hr);
    }

    if (!shadowInitData)
    {
        ANGLE_TRY(ClearResource(renderer, desc, resource));
    }

    ASSERT(resource);
    incrResource(GetResourceTypeFromD3D11<T>(), ComputeMemoryUsage(desc));
    *resourceOut = std::move(Resource11<T>(resource, this));
    return gl::NoError();
}

void ResourceManager11::incrResource(ResourceType resourceType, size_t memorySize)
{
    mAllocatedResourceCounts[ResourceTypeIndex(resourceType)]++;
    mAllocatedResourceDeviceMemory[ResourceTypeIndex(resourceType)] += memorySize;
}

void ResourceManager11::decrResource(ResourceType resourceType, size_t memorySize)
{
    ASSERT(mAllocatedResourceCounts[ResourceTypeIndex(resourceType)] > 0);
    mAllocatedResourceCounts[ResourceTypeIndex(resourceType)]--;
    ASSERT(mAllocatedResourceDeviceMemory[ResourceTypeIndex(resourceType)] >= memorySize);
    mAllocatedResourceDeviceMemory[ResourceTypeIndex(resourceType)] -= memorySize;
}

void ResourceManager11::onReleaseResource(ResourceType resourceType, ID3D11Resource *resource)
{
    ASSERT(resource);
    decrResource(resourceType, ComputeGenericMemoryUsage(resourceType, resource));
}

template <>
void ResourceManager11::onRelease(ID3D11Resource *resource)
{
    // For untyped ID3D11Resource, they must call onReleaseResource.
    UNREACHABLE();
}

template <typename T>
void ResourceManager11::onRelease(T *resource)
{
    ASSERT(resource);

    GetDescFromD3D11<T> desc;
    resource->GetDesc(&desc);
    decrResource(GetResourceTypeFromD3D11<T>(), ComputeMemoryUsage(&desc));
}

template gl::Error ResourceManager11::allocate<ID3D11Texture2D>(Renderer11 *,
                                                                const D3D11_TEXTURE2D_DESC *,
                                                                const D3D11_SUBRESOURCE_DATA *,
                                                                Resource11<ID3D11Texture2D> *);
template gl::Error ResourceManager11::allocate<ID3D11Texture3D>(Renderer11 *,
                                                                const D3D11_TEXTURE3D_DESC *,
                                                                const D3D11_SUBRESOURCE_DATA *,
                                                                d3d11::Texture3D *);
template gl::Error ResourceManager11::allocate<ID3D11Buffer>(Renderer11 *,
                                                             const D3D11_BUFFER_DESC *,
                                                             const D3D11_SUBRESOURCE_DATA *,
                                                             d3d11::Buffer *);
template gl::Error ResourceManager11::allocate<ID3D11RenderTargetView>(
    Renderer11 *,
    const D3D11_RENDER_TARGET_VIEW_DESC *,
    ID3D11Resource *,
    d3d11::RenderTargetView *);
template gl::Error ResourceManager11::allocate<ID3D11ShaderResourceView>(
    Renderer11 *,
    const D3D11_SHADER_RESOURCE_VIEW_DESC *,
    ID3D11Resource *,
    d3d11::ShaderResourceView *);
template gl::Error ResourceManager11::allocate<ID3D11DepthStencilView>(
    Renderer11 *,
    const D3D11_DEPTH_STENCIL_VIEW_DESC *,
    ID3D11Resource *,
    d3d11::DepthStencilView *);

template void ResourceManager11::onRelease<ID3D11Texture2D>(ID3D11Texture2D *);
template void ResourceManager11::onRelease<ID3D11Texture3D>(ID3D11Texture3D *);
template void ResourceManager11::onRelease<ID3D11Buffer>(ID3D11Buffer *);
template void ResourceManager11::onRelease<ID3D11RenderTargetView>(ID3D11RenderTargetView *);
template void ResourceManager11::onRelease<ID3D11ShaderResourceView>(ID3D11ShaderResourceView *);
template void ResourceManager11::onRelease<ID3D11DepthStencilView>(ID3D11DepthStencilView *);

}  // namespace rx
