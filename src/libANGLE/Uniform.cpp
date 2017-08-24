//
// Copyright (c) 2010-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "libANGLE/Uniform.h"

#include "common/utilities.h"

#include <cstring>

namespace gl
{

LinkedUniform::LinkedUniform()
    : bufferIndex(-1), blockInfo(sh::BlockMemberInfo::getDefaultBlockInfo()), typeInfo(nullptr)
{
}

LinkedUniform::LinkedUniform(GLenum typeIn,
                             GLenum precisionIn,
                             const std::string &nameIn,
                             unsigned int arraySizeIn,
                             const int bindingIn,
                             const int offsetIn,
                             const int locationIn,
                             const int bufferIndexIn,
                             const sh::BlockMemberInfo &blockInfoIn)
    : bufferIndex(bufferIndexIn), blockInfo(blockInfoIn)
{
    type      = typeIn;
    precision = precisionIn;
    name      = nameIn;
    arraySize = arraySizeIn;
    binding   = bindingIn;
    offset    = offsetIn;
    location  = locationIn;

    typeInfo = &GetUniformTypeInfo(type);
}

LinkedUniform::LinkedUniform(const sh::Uniform &uniform)
    : sh::Uniform(uniform), bufferIndex(-1), blockInfo(sh::BlockMemberInfo::getDefaultBlockInfo())
{
    typeInfo = &GetUniformTypeInfo(type);
}

LinkedUniform::LinkedUniform(const LinkedUniform &uniform)
    : sh::Uniform(uniform), bufferIndex(uniform.bufferIndex), blockInfo(uniform.blockInfo), typeInfo(uniform.typeInfo)
{
}

LinkedUniform &LinkedUniform::operator=(const LinkedUniform &uniform)
{
    sh::Uniform::operator=(uniform);
    bufferIndex          = uniform.bufferIndex;
    blockInfo            = uniform.blockInfo;
    typeInfo             = uniform.typeInfo;

    return *this;
}

LinkedUniform::~LinkedUniform()
{
}

bool LinkedUniform::isInDefaultBlock() const
{
    return bufferIndex == -1;
}

bool LinkedUniform::isSampler() const
{
    return typeInfo->isSampler;
}

bool LinkedUniform::isImage() const
{
    return typeInfo->isImageType;
}

bool LinkedUniform::isAtomicCounter() const
{
    return IsAtomicCounterType(type);
}

bool LinkedUniform::isField() const
{
    return name.find('.') != std::string::npos;
}

size_t LinkedUniform::getElementSize() const
{
    return typeInfo->externalSize;
}

size_t LinkedUniform::getElementComponents() const
{
    return typeInfo->componentCount;
}

ShaderVariableBuffer::ShaderVariableBuffer()
    : binding(0),
      dataSize(0),
      vertexStaticUse(false),
      fragmentStaticUse(false),
      computeStaticUse(false)
{
}

ShaderVariableBuffer::~ShaderVariableBuffer()
{
}

UniformBlock::UniformBlock() : isArray(false), arrayElement(0)
{
}

UniformBlock::UniformBlock(const std::string &nameIn,
                           bool isArrayIn,
                           unsigned int arrayElementIn,
                           int bindingIn)
    : name(nameIn), isArray(isArrayIn), arrayElement(arrayElementIn)
{
    binding = bindingIn;
}

std::string UniformBlock::nameWithArrayIndex() const
{
    std::stringstream fullNameStr;
    fullNameStr << name;
    if (isArray)
    {
        fullNameStr << "[" << arrayElement << "]";
    }

    return fullNameStr.str();
}
}
