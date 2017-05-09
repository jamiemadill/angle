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
template <typename T>
size_t ComputeMemoryUsage(const T *desc)
{
    return 0;
}

HRESULT CreateResource(ID3D11Device *device,
                       const D3D11_DEPTH_STENCIL_VIEW_DESC *desc,
                       ID3D11Resource *resource,
                       ID3D11DepthStencilView **depthStencilView)
{
    return device->CreateDepthStencilView(resource, desc, depthStencilView);
}

HRESULT CreateResource(ID3D11Device *device,
                       const D3D11_RENDER_TARGET_VIEW_DESC *desc,
                       ID3D11Resource *resource,
                       ID3D11RenderTargetView **renderTargetView)
{
    return device->CreateRenderTargetView(resource, desc, renderTargetView);
}

constexpr std::array<const char *, NumResourceTypes> kResourceTypeNames = {
    {"DepthStencilView", "RenderTargetView"}};
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

template <typename T>
gl::Error ResourceManager11::allocate(Renderer11 *renderer,
                                      const GetDescFromD3D11<T> *desc,
                                      GetInitDataFromD3D11<T> *initData,
                                      Resource11<T> *resourceOut)
{
    ID3D11Device *device = renderer->getDevice();
    T *resource          = nullptr;

    HRESULT hr = CreateResource(device, desc, initData, &resource);

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

template <typename T>
void ResourceManager11::onRelease(T *resource)
{
    ASSERT(resource);

    GetDescFromD3D11<T> desc;
    resource->GetDesc(&desc);
    decrResource(GetResourceTypeFromD3D11<T>(), ComputeMemoryUsage(&desc));
}

template gl::Error ResourceManager11::allocate<ID3D11DepthStencilView>(
    Renderer11 *,
    const D3D11_DEPTH_STENCIL_VIEW_DESC *,
    ID3D11Resource *,
    d3d11::DepthStencilView *);

template gl::Error ResourceManager11::allocate<ID3D11RenderTargetView>(
    Renderer11 *,
    const D3D11_RENDER_TARGET_VIEW_DESC *,
    ID3D11Resource *,
    d3d11::RenderTargetView *);

template void ResourceManager11::onRelease<ID3D11DepthStencilView>(ID3D11DepthStencilView *);
template void ResourceManager11::onRelease<ID3D11RenderTargetView>(ID3D11RenderTargetView *);

}  // namespace rx