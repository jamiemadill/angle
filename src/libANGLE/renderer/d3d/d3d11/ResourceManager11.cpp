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

size_t ComputeMemoryUsage(const D3D11_TEXTURE2D_DESC &desc)
{
    size_t pixelBytes = static_cast<size_t>(d3d11::GetDXGIFormatSizeInfo(desc.Format).pixelBytes);
    size_t levelSize  = static_cast<size_t>(desc.Width * desc.Height * desc.ArraySize) * pixelBytes;
    return ComputeMippedMemoryUsage(levelSize, static_cast<size_t>(desc.MipLevels));
}

size_t ComputeMemoryUsage(const D3D11_TEXTURE3D_DESC &desc)
{
    size_t pixelBytes = static_cast<size_t>(d3d11::GetDXGIFormatSizeInfo(desc.Format).pixelBytes);
    size_t levelSize  = static_cast<size_t>(desc.Width * desc.Height * desc.Depth) * pixelBytes;
    return ComputeMippedMemoryUsage(levelSize, static_cast<size_t>(desc.MipLevels));
}

size_t ComputeMemoryUsage(const D3D11_BUFFER_DESC &desc)
{
    return static_cast<size_t>(desc.ByteWidth);
}

template <ResourceType ResourceT>
size_t ComputeGenericMemoryUsage(ID3D11Resource *genericResource)
{
    auto *typedResource = static_cast<GetD3D11Type<ResourceT> *>(genericResource);
    GetDescType<ResourceT> desc;
    typedResource->GetDesc(&desc);
    return ComputeMemoryUsage(desc);
}

size_t ComputeGenericMemoryUsage(ResourceType resourceType, ID3D11Resource *resource)
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
            UNREACHABLE();
            return 0;
    }
}

HRESULT CreateResource(ID3D11Device *device,
                       const D3D11_TEXTURE2D_DESC &desc,
                       const D3D11_SUBRESOURCE_DATA *initData,
                       ID3D11Texture2D **texture)
{
    return device->CreateTexture2D(&desc, initData, texture);
}

HRESULT CreateResource(ID3D11Device *device,
                       const D3D11_TEXTURE3D_DESC &desc,
                       const D3D11_SUBRESOURCE_DATA *initData,
                       ID3D11Texture3D **texture)
{
    return device->CreateTexture3D(&desc, initData, texture);
}

HRESULT CreateResource(ID3D11Device *device,
                       const D3D11_BUFFER_DESC &desc,
                       const D3D11_SUBRESOURCE_DATA *initData,
                       ID3D11Buffer **buffer)
{
    return device->CreateBuffer(&desc, initData, buffer);
}

const char *ResourceTypeString(ResourceType resourceType)
{
    switch (resourceType)
    {
        case ResourceType::Texture2D:
            return "Texture";
        case ResourceType::Texture3D:
            return "Texture3D";
        case ResourceType::Buffer:
            return "Buffer";
        default:
            UNREACHABLE();
            return "";
    }
}
}  // anonymous namespace

// Resource11 Implementation
template <ResourceType Type>
Resource11<Type>::Resource11() : mResource(nullptr), mFactory(nullptr)
{
}

template <ResourceType Type>
Resource11<Type>::Resource11(Resource11 &&movedObj)
    : mResource(movedObj.mResource), mFactory(movedObj.mFactory)
{
    movedObj.mResource = nullptr;
    movedObj.mFactory  = nullptr;
}

template <ResourceType Type>
Resource11<Type>::Resource11(GetD3D11Type<Type> *resource, ResourceManager11 *factory)
    : mResource(resource), mFactory(factory)
{
}

template <ResourceType Type>
Resource11<Type>::~Resource11()
{
    reset();
}

template <ResourceType Type>
Resource11<Type> &Resource11<Type>::operator=(Resource11 &&movedObj)
{
    std::swap(mResource, movedObj.mResource);
    std::swap(mFactory, movedObj.mFactory);
    return *this;
}

template <ResourceType Type>
void Resource11<Type>::setDebugName(const char *name)
{
    d3d11::SetDebugName(mResource, name);
}

template <ResourceType Type>
void Resource11<Type>::set(GetD3D11Type<Type> *resource)
{
    ASSERT(!valid());
    mResource = resource;
}

template <ResourceType Type>
bool Resource11<Type>::valid() const
{
    return (mResource != nullptr);
}

template <ResourceType Type>
void Resource11<Type>::reset()
{
    if (mResource)
    {
        // We can have a nullptr factory when holding passed-in resources.
        if (mFactory)
        {
            mFactory->onRelease<Type>(mResource);
            mFactory = nullptr;
        }
        mResource->Release();
        mResource = nullptr;
    }
}

template <ResourceType Type>
GenericResource11 Resource11<Type>::makeGeneric()
{
    GenericResource11 genericResource(std::move(*this));
    return genericResource;
}

template class Resource11<ResourceType::Texture2D>;
template class Resource11<ResourceType::Texture3D>;
template class Resource11<ResourceType::Buffer>;

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

template <ResourceType Type>
gl::Error ResourceManager11::allocate(Renderer11 *renderer,
                                      const GetDescType<Type> &desc,
                                      const D3D11_SUBRESOURCE_DATA *initData,
                                      Resource11<Type> *resourceOut)
{
    ID3D11Device *device         = renderer->getDevice();
    GetD3D11Type<Type> *resource = nullptr;
    HRESULT hr                   = CreateResource(device, desc, initData, &resource);
    if (FAILED(hr))
    {
        ASSERT(!resource);
        if (d3d11::isDeviceLostError(hr))
        {
            renderer->notifyDeviceLost();
        }
        return gl::OutOfMemory() << "Error allocating " << std::string(ResourceTypeString(Type))
                                 << ". " << gl::FmtHR(hr);
    }
    ASSERT(resource);
    incrResource(Type, ComputeMemoryUsage(desc));
    *resourceOut = std::move(Resource11<Type>(resource, this));
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

template <ResourceType ResourceT>
void ResourceManager11::onRelease(GetD3D11Type<ResourceT> *resource)
{
    ASSERT(resource);

    GetDescType<ResourceT> desc;
    resource->GetDesc(&desc);
    decrResource(ResourceT, ComputeMemoryUsage(desc));
}

template gl::Error ResourceManager11::allocate<ResourceType::Texture2D>(
    Renderer11 *,
    const D3D11_TEXTURE2D_DESC &,
    const D3D11_SUBRESOURCE_DATA *,
    Resource11<ResourceType::Texture2D> *);
template gl::Error ResourceManager11::allocate<ResourceType::Texture3D>(
    Renderer11 *,
    const D3D11_TEXTURE3D_DESC &,
    const D3D11_SUBRESOURCE_DATA *,
    Resource11<ResourceType::Texture3D> *);
template gl::Error ResourceManager11::allocate<ResourceType::Buffer>(
    Renderer11 *,
    const D3D11_BUFFER_DESC &,
    const D3D11_SUBRESOURCE_DATA *,
    Resource11<ResourceType::Buffer> *);

template void ResourceManager11::onRelease<ResourceType::Texture2D>(ID3D11Texture2D *);
template void ResourceManager11::onRelease<ResourceType::Texture3D>(ID3D11Texture3D *);
template void ResourceManager11::onRelease<ResourceType::Buffer>(ID3D11Buffer *);

// GenericResource11 Implementation.
GenericResource11::GenericResource11()
    : mGenericResource(nullptr), mResourceType(ResourceType::Last), mFactory(nullptr)
{
}

GenericResource11::~GenericResource11()
{
    reset();
}

GenericResource11::GenericResource11(GenericResource11 &&movedObj)
    : mGenericResource(movedObj.mGenericResource),
      mResourceType(movedObj.mResourceType),
      mFactory(movedObj.mFactory)
{
    movedObj.mGenericResource = nullptr;
    movedObj.mResourceType    = ResourceType::Last;
    movedObj.mFactory         = nullptr;
}

GenericResource11 &GenericResource11::operator=(GenericResource11 &&movedObj)
{
    std::swap(mGenericResource, movedObj.mGenericResource);
    std::swap(mResourceType, movedObj.mResourceType);
    std::swap(mFactory, movedObj.mFactory);
    return *this;
}

void GenericResource11::reset()
{
    if (valid())
    {
        if (mFactory)
        {
            size_t memorySize = ComputeGenericMemoryUsage(mResourceType, mGenericResource);
            mFactory->decrResource(mResourceType, memorySize);
            mFactory = nullptr;
        }
        mGenericResource->Release();
        mGenericResource = nullptr;
        mResourceType    = ResourceType::Last;
    }
}

void GenericResource11::setDebugName(const char *name)
{
    d3d11::SetDebugName(mGenericResource, name);
}

void GenericResource11::set(ID3D11Resource *resource, ResourceType resourceType)
{
    ASSERT(!valid());
    mGenericResource = resource;
    mResourceType    = resourceType;
}

SharedResource11 GenericResource11::makeShared()
{
    SharedResource11 sharedResource(std::move(*this));
    return sharedResource;
}

template <typename DescT>
void GenericResource11::getDesc(DescT *descOut)
{
    return static_cast<GetD3D11Type<GetResourceTypeFromDesc<DescT>()> *>(mGenericResource)
        ->GetDesc(descOut);
}

template void GenericResource11::getDesc(D3D11_BUFFER_DESC *);
template void GenericResource11::getDesc(D3D11_TEXTURE2D_DESC *);
template void GenericResource11::getDesc(D3D11_TEXTURE3D_DESC *);

// SharedResource11 Implementation.

SharedResource11::SharedResource11() : mSharedResource(new GenericResource11())
{
}

SharedResource11::~SharedResource11()
{
}

SharedResource11::SharedResource11(const SharedResource11 &sharedObj)
    : mSharedResource(sharedObj.mSharedResource)
{
}

SharedResource11::SharedResource11(SharedResource11 &&movedObj)
    : mSharedResource(new GenericResource11())
{
    std::swap(mSharedResource, movedObj.mSharedResource);
}

SharedResource11 &SharedResource11::operator=(const SharedResource11 &sharedObj)
{
    mSharedResource = sharedObj.mSharedResource;
    return *this;
}

SharedResource11 &SharedResource11::operator=(SharedResource11 &&movedObj)
{
    std::swap(mSharedResource, movedObj.mSharedResource);
    return *this;
}

void SharedResource11::reset()
{
    mSharedResource->reset();
}

SharedResource11::SharedResource11(GenericResource11 &&genericResource)
    : mSharedResource(new GenericResource11(std::move(genericResource)))
{
}

SharedResource11 &SharedResource11::operator=(GenericResource11 &&genericResource)
{
    mSharedResource.reset(new GenericResource11(std::move(genericResource)));
    return *this;
}

void SharedResource11::setDebugName(const char *name)
{
    ASSERT(valid());
    mSharedResource->setDebugName(name);
}

void SharedResource11::set(ID3D11Resource *resource, ResourceType resourceType)
{
    ASSERT(!valid());
    mSharedResource->set(resource, resourceType);
}

}  // namespace rx
