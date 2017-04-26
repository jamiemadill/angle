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

#include "common/angleutils.h"
#include "libANGLE/Error.h"

namespace rx
{
class Renderer11;

enum class ResourceType
{
    Texture2D,
    Texture3D,
    Buffer,
    Last
};

constexpr size_t ResourceTypeIndex(ResourceType resourceType)
{
    return static_cast<size_t>(resourceType);
}

constexpr size_t NumResourceTypes = ResourceTypeIndex(ResourceType::Last);

template <ResourceType Type>
struct ResourceTypeToType;

template <ResourceType Type>
struct ResourceTypeToDesc;

#define ANGLE_STRUCT_HELPER_TYPE(HELPER, ID, TYPE) \
    \
template<> struct HELPER<ID>                       \
    {                                              \
        using Type = TYPE;                         \
    };

ANGLE_STRUCT_HELPER_TYPE(ResourceTypeToType, ResourceType::Texture2D, ID3D11Texture2D);
ANGLE_STRUCT_HELPER_TYPE(ResourceTypeToType, ResourceType::Texture3D, ID3D11Texture3D);
ANGLE_STRUCT_HELPER_TYPE(ResourceTypeToType, ResourceType::Buffer, ID3D11Buffer);

ANGLE_STRUCT_HELPER_TYPE(ResourceTypeToDesc, ResourceType::Texture2D, D3D11_TEXTURE2D_DESC);
ANGLE_STRUCT_HELPER_TYPE(ResourceTypeToDesc, ResourceType::Texture3D, D3D11_TEXTURE3D_DESC);
ANGLE_STRUCT_HELPER_TYPE(ResourceTypeToDesc, ResourceType::Buffer, D3D11_BUFFER_DESC);

template <ResourceType ResourceT>
struct ResourceTypeToType
{
};

template <ResourceType ResourceT>
struct ResourceTypeToDesc
{
};

template <ResourceType ResourceT>
using GetResourceType = typename ResourceTypeToType<ResourceT>::Type;

template <ResourceType ResourceT>
using GetDescType = typename ResourceTypeToDesc<ResourceT>::Type;

// Smart pointer type. Wraps the resource and a factory for safe deletion.
template <ResourceType Type>
class Resource11 final : angle::NonCopyable
{
  public:
    Resource11();
    ~Resource11();

    GetResourceType<Type> *get() const { return mResource; }

  private:
    friend class ResourceManager11;
    Resource11(GetResourceType<Type> *resource, ResourceManager11 *factory);
    Resource11(Resource11 &&movedObj);
    Resource11 &operator=(Resource11 &&movedObj);

    GetResourceType<Type> *mResource;
    ResourceManager11 *mFactory;
};

class ResourceManager11 final : angle::NonCopyable
{
  public:
    ResourceManager11();
    ~ResourceManager11();

    template <ResourceType Type>
    gl::Error allocate(Renderer11 *renderer,
                       const GetDescType<Type> &desc,
                       const D3D11_SUBRESOURCE_DATA *initData,
                       Resource11<Type> *resourceOut);

  private:
    template <ResourceType ResourceT>
    friend class Resource11;

    template <ResourceType ResourceT>
    void release(GetResourceType<ResourceT> *resource);

    void incrResource(ResourceType resourceType, size_t memorySize);
    void decrResource(ResourceType resourceType, size_t memorySize);

    std::array<size_t, NumResourceTypes> mAllocatedResourceCounts;
    std::array<size_t, NumResourceTypes> mAllocatedResourceDeviceMemory;
};

namespace d3d11
{
using Texture2D = Resource11<ResourceType::Texture2D>;
using Texture3D = Resource11<ResourceType::Texture3D>;
using Buffer    = Resource11<ResourceType::Buffer>;
}  // namespace d3d11

}  // namespace rx

#endif  // LIBANGLE_RENDERER_D3D_D3D11_RESOURCEFACTORY11_H_
