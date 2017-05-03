//
// Copyright 2017 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ResourceManager11:
//   Centralized point of allocation for all D3D11 Buffers and Textures.

#ifndef LIBANGLE_RENDERER_D3D_D3D11_RESOURCEFACTORY11_H_
#define LIBANGLE_RENDERER_D3D_D3D11_RESOURCEFACTORY11_H_

#include <array>
#include <memory>

#include "common/MemoryBuffer.h"
#include "common/angleutils.h"
#include "libANGLE/Constants.h"
#include "libANGLE/Error.h"

namespace rx
{
class GenericResource11;
class Renderer11;
class SharedResource11;

enum class ResourceType
{
    Buffer,
    DepthStencilView,
    ShaderResourceView,
    RenderTargetView,
    Texture2D,
    Texture3D,
    Last
};

constexpr size_t ResourceTypeIndex(ResourceType resourceType)
{
    return static_cast<size_t>(resourceType);
}

constexpr size_t NumResourceTypes = ResourceTypeIndex(ResourceType::Last);

#define ANGLE_TYPE_HELPER_BEGIN(NAME, T) \
    template <T Param>                   \
    struct NAME;
#define ANGLE_TYPE_HELPER_END(NAME, T) \
    \
template<T Param> struct NAME          \
    {                                  \
    };                                 \
    \
template<T Param> using Get##NAME = typename NAME<Param>::Value;

#define ANGLE_TYPE_HELPER(HELPER, TYPE, ID, VALUE) \
    \
template<> struct HELPER<TYPE::ID>                 \
    {                                              \
        using Value = VALUE;                       \
    };

#define ANGLE_INV_TYPE_HELPER_BEGIN(NAME) \
    template <typename Param>             \
    struct NAME;
#define ANGLE_INV_TYPE_HELPER_END(NAME, T)       \
    \
template<typename Param> struct NAME             \
    {                                            \
    };                                           \
    \
template<typename Param> constexpr T Get##NAME() \
    {                                            \
        return NAME<Param>::Value;               \
    }

#define ANGLE_INV_TYPE_HELPER(HELPER, TYPE, ID, VALUE) \
    \
template<> struct HELPER<ID>                           \
    {                                                  \
        static constexpr TYPE Value = TYPE::VALUE;     \
    };

ANGLE_TYPE_HELPER_BEGIN(D3D11Type, ResourceType)
ANGLE_TYPE_HELPER(D3D11Type, ResourceType, Buffer, ID3D11Buffer)
ANGLE_TYPE_HELPER(D3D11Type, ResourceType, DepthStencilView, ID3D11DepthStencilView)
ANGLE_TYPE_HELPER(D3D11Type, ResourceType, RenderTargetView, ID3D11RenderTargetView)
ANGLE_TYPE_HELPER(D3D11Type, ResourceType, ShaderResourceView, ID3D11ShaderResourceView)
ANGLE_TYPE_HELPER(D3D11Type, ResourceType, Texture2D, ID3D11Texture2D)
ANGLE_TYPE_HELPER(D3D11Type, ResourceType, Texture3D, ID3D11Texture3D)
ANGLE_TYPE_HELPER_END(D3D11Type, ResourceType)

ANGLE_TYPE_HELPER_BEGIN(DescType, ResourceType)
ANGLE_TYPE_HELPER(DescType, ResourceType, Buffer, D3D11_BUFFER_DESC)
ANGLE_TYPE_HELPER(DescType, ResourceType, DepthStencilView, D3D11_DEPTH_STENCIL_VIEW_DESC)
ANGLE_TYPE_HELPER(DescType, ResourceType, RenderTargetView, D3D11_RENDER_TARGET_VIEW_DESC)
ANGLE_TYPE_HELPER(DescType, ResourceType, ShaderResourceView, D3D11_SHADER_RESOURCE_VIEW_DESC)
ANGLE_TYPE_HELPER(DescType, ResourceType, Texture2D, D3D11_TEXTURE2D_DESC)
ANGLE_TYPE_HELPER(DescType, ResourceType, Texture3D, D3D11_TEXTURE3D_DESC)
ANGLE_TYPE_HELPER_END(DescType, ResourceType)

ANGLE_TYPE_HELPER_BEGIN(InitDataType, ResourceType)
ANGLE_TYPE_HELPER(InitDataType, ResourceType, Buffer, const D3D11_SUBRESOURCE_DATA)
ANGLE_TYPE_HELPER(InitDataType, ResourceType, DepthStencilView, ID3D11Resource)
ANGLE_TYPE_HELPER(InitDataType, ResourceType, RenderTargetView, ID3D11Resource)
ANGLE_TYPE_HELPER(InitDataType, ResourceType, ShaderResourceView, ID3D11Resource)
ANGLE_TYPE_HELPER(InitDataType, ResourceType, Texture2D, const D3D11_SUBRESOURCE_DATA)
ANGLE_TYPE_HELPER(InitDataType, ResourceType, Texture3D, const D3D11_SUBRESOURCE_DATA)
ANGLE_TYPE_HELPER_END(InitDataType, ResourceType)

ANGLE_INV_TYPE_HELPER_BEGIN(ResourceTypeFromDesc)
ANGLE_INV_TYPE_HELPER(ResourceTypeFromDesc, ResourceType, D3D11_BUFFER_DESC, Buffer)
ANGLE_INV_TYPE_HELPER(ResourceTypeFromDesc,
                      ResourceType,
                      D3D11_DEPTH_STENCIL_VIEW_DESC,
                      DepthStencilView)
ANGLE_INV_TYPE_HELPER(ResourceTypeFromDesc,
                      ResourceType,
                      D3D11_RENDER_TARGET_VIEW_DESC,
                      RenderTargetView)
ANGLE_INV_TYPE_HELPER(ResourceTypeFromDesc,
                      ResourceType,
                      D3D11_SHADER_RESOURCE_VIEW_DESC,
                      ShaderResourceView)
ANGLE_INV_TYPE_HELPER(ResourceTypeFromDesc, ResourceType, D3D11_TEXTURE2D_DESC, Texture2D)
ANGLE_INV_TYPE_HELPER(ResourceTypeFromDesc, ResourceType, D3D11_TEXTURE3D_DESC, Texture3D)
ANGLE_INV_TYPE_HELPER_END(ResourceTypeFromDesc, ResourceType)

// Smart pointer type. Wraps the resource and a factory for safe deletion.
template <ResourceType Type>
class Resource11 : angle::NonCopyable
{
  public:
    Resource11();
    ~Resource11();
    Resource11(Resource11 &&movedObj);
    Resource11 &operator=(Resource11 &&movedObj);

    GetD3D11Type<Type> *get() const { return mResource; }
    void setDebugName(const char *name);
    void set(GetD3D11Type<Type> *resource);
    bool valid() const;
    void reset();

    GenericResource11 makeGeneric();

  private:
    friend class GenericResource11;
    friend class ResourceManager11;
    Resource11(GetD3D11Type<Type> *resource, ResourceManager11 *factory);

    GetD3D11Type<Type> *mResource;
    ResourceManager11 *mFactory;
};

class GenericResource11 : angle::NonCopyable
{
  public:
    GenericResource11();
    ~GenericResource11();
    GenericResource11(GenericResource11 &&movedObj);
    GenericResource11 &operator=(GenericResource11 &&movedObj);
    void reset();
    void setDebugName(const char *name);
    void set(ID3D11Resource *resource, ResourceType resourceType);

    template <ResourceType ResourceT>
    GenericResource11(Resource11<ResourceT> &&movedObj)
        : mGenericResource(movedObj.mResource),
          mResourceType(ResourceT),
          mFactory(movedObj.mFactory)
    {
        movedObj.mResource = nullptr;
        movedObj.mFactory  = nullptr;
    }

    template <ResourceType ResourceT>
    GenericResource11 &operator=(Resource11<ResourceT> &&movedObj)
    {
        mGenericResource   = movedObj.mResource;
        mResourceType      = ResourceT;
        mFactory           = movedObj.mFactory;
        movedObj.mResource = nullptr;
        movedObj.mFactory  = nullptr;
        return *this;
    }

    ResourceType getResourceType() const { return mResourceType; }
    bool valid() const { return mGenericResource != nullptr; }

    SharedResource11 makeShared();

    template <ResourceType ResourceT>
    GetD3D11Type<ResourceT> *getAs() const
    {
        ASSERT(ResourceT == mResourceType);
        return static_cast<GetD3D11Type<ResourceT> *>(mGenericResource);
    }

    template <typename DescT>
    void getDesc(DescT *descOut) const;

    // Will ASSERT that the resource type is correct.
    ID3D11Resource *asResource() const;

    bool operator==(const GenericResource11 &other) const;
    bool operator!=(const GenericResource11 &other) const;

  private:
    ID3D11DeviceChild *mGenericResource;
    ResourceType mResourceType;
    ResourceManager11 *mFactory;
};

class SharedResource11
{
  public:
    SharedResource11();
    ~SharedResource11();
    SharedResource11(const SharedResource11 &sharedObj);
    SharedResource11(SharedResource11 &&movedObj);
    SharedResource11 &operator=(const SharedResource11 &sharedObj);
    SharedResource11 &operator=(SharedResource11 &&movedObj);
    void reset();

    SharedResource11(GenericResource11 &&genericResource);
    SharedResource11 &operator=(GenericResource11 &&genericResource);
    void setDebugName(const char *name);
    void set(ID3D11Resource *resource, ResourceType resourceType);

    ResourceType getResourceType() const { return mSharedResource->getResourceType(); }
    ID3D11Resource *asResource() const { return mSharedResource->asResource(); }
    bool valid() const { return mSharedResource->valid(); }

    template <ResourceType ResourceT>
    GetD3D11Type<ResourceT> *getAs() const
    {
        return mSharedResource->getAs<ResourceT>();
    }

    template <typename DescT>
    void getDesc(DescT *descOut) const
    {
        return mSharedResource->getDesc(descOut);
    }

    bool operator==(const SharedResource11 &other) const
    {
        return *mSharedResource == *other.mSharedResource;
    }
    bool operator!=(const SharedResource11 &other) const
    {
        return *mSharedResource != *other.mSharedResource;
    }

  private:
    std::shared_ptr<GenericResource11> mSharedResource;
};

class ResourceManager11 final : angle::NonCopyable
{
  public:
    ResourceManager11();
    ~ResourceManager11();

    template <ResourceType Type>
    gl::Error allocate(Renderer11 *renderer,
                       const GetDescType<Type> &desc,
                       GetInitDataType<Type> *initData,
                       Resource11<Type> *resourceOut)
    {
        return allocateImpl(renderer, &desc, initData, resourceOut);
    }

    // Some View resources can be allocated without a desc.
    template <ResourceType Type>
    gl::Error allocate(Renderer11 *renderer,
                       GetInitDataType<Type> *initData,
                       Resource11<Type> *resourceOut)
    {
        return allocateImpl(renderer, nullptr, initData, resourceOut);
    }

  private:
    friend class GenericResource11;
    template <ResourceType ResourceT>
    friend class Resource11;

    template <ResourceType ResourceT>
    void onRelease(GetD3D11Type<ResourceT> *resource);

    template <ResourceType Type>
    gl::Error allocateImpl(Renderer11 *renderer,
                           const GetDescType<Type> *desc,
                           GetInitDataType<Type> *initData,
                           Resource11<Type> *resourceOut);

    void incrResource(ResourceType resourceType, size_t memorySize);
    void decrResource(ResourceType resourceType, size_t memorySize);

    template <ResourceType Type>
    GetInitDataType<Type> *createInitDataIfNeeded(const GetDescType<Type> *desc);

    std::array<size_t, NumResourceTypes> mAllocatedResourceCounts;
    std::array<size_t, NumResourceTypes> mAllocatedResourceDeviceMemory;
    angle::MemoryBuffer mZeroMemory;

    std::vector<D3D11_SUBRESOURCE_DATA> mShadowInitData;
};

namespace d3d11
{
using Buffer             = Resource11<ResourceType::Buffer>;
using DepthStencilView   = Resource11<ResourceType::DepthStencilView>;
using RenderTargetView   = Resource11<ResourceType::RenderTargetView>;
using ShaderResourceView = Resource11<ResourceType::ShaderResourceView>;
using Texture2D          = Resource11<ResourceType::Texture2D>;
using Texture3D          = Resource11<ResourceType::Texture3D>;
}  // namespace d3d11

}  // namespace rx

#endif  // LIBANGLE_RENDERER_D3D_D3D11_RESOURCEFACTORY11_H_
