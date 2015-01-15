//
// Copyright (c) 2012-2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// ShaderExecutable11.cpp: Implements a D3D11-specific class to contain shader
// executable implementation details.

#include "libANGLE/renderer/d3d/d3d11/ShaderExecutable11.h"

#include "common/utilities.h"
#include "libANGLE/renderer/d3d/d3d11/Renderer11.h"

namespace rx
{

ShaderExecutable11::ShaderExecutable11(Renderer11 *renderer, const uint8_t *function, size_t length)
    : ShaderExecutable(function, length),
      mRenderer(renderer),
      mPixelExecutable(nullptr),
      mVertexExecutable(nullptr),
      mGeometryExecutable(nullptr),
      mStreamOutExecutable(nullptr)
{
}

ShaderExecutable11::~ShaderExecutable11()
{
    SafeRelease(mVertexExecutable);
    SafeRelease(mPixelExecutable);
    SafeRelease(mGeometryExecutable);
    SafeRelease(mStreamOutExecutable);
}

ShaderExecutable11 *ShaderExecutable11::makeShaderExecutable11(ShaderExecutable *executable)
{
    ASSERT(HAS_DYNAMIC_TYPE(ShaderExecutable11*, executable));
    return static_cast<ShaderExecutable11*>(executable);
}

gl::Error ShaderExecutable11::finishLoad(ShaderType shaderType,
                                         const std::vector<gl::LinkedVarying> &transformFeedbackVaryings,
                                         bool separatedOutputBuffers)
{
    ID3D11Device *device = mRenderer->getDevice();

    switch (shaderType)
    {
      case SHADER_VERTEX:
        {
            ID3D11VertexShader *vertexShader = NULL;
            ID3D11GeometryShader *streamOutShader = NULL;

            HRESULT result = device->CreateVertexShader(getFunction(), getLength(), NULL, &vertexShader);
            ASSERT(SUCCEEDED(result));
            if (FAILED(result))
            {
                return gl::Error(GL_OUT_OF_MEMORY, "Failed to create vertex shader, result: 0x%X.", result);
            }

            if (transformFeedbackVaryings.size() > 0)
            {
                std::vector<D3D11_SO_DECLARATION_ENTRY> soDeclaration;
                for (size_t i = 0; i < transformFeedbackVaryings.size(); i++)
                {
                    const gl::LinkedVarying &varying = transformFeedbackVaryings[i];
                    GLenum transposedType = gl::TransposeMatrixType(varying.type);

                    for (size_t j = 0; j < varying.semanticIndexCount; j++)
                    {
                        D3D11_SO_DECLARATION_ENTRY entry = { 0 };
                        entry.Stream = 0;
                        entry.SemanticName = varying.semanticName.c_str();
                        entry.SemanticIndex = varying.semanticIndex + j;
                        entry.StartComponent = 0;
                        entry.ComponentCount = gl::VariableColumnCount(transposedType);
                        entry.OutputSlot = (separatedOutputBuffers ? i : 0);
                        soDeclaration.push_back(entry);
                    }
                }

                result = device->CreateGeometryShaderWithStreamOutput(getFunction(), getLength(),
                                                                      soDeclaration.data(), soDeclaration.size(),
                                                                      NULL, 0, 0, NULL, &streamOutShader);
                ASSERT(SUCCEEDED(result));
                if (FAILED(result))
                {
                    return gl::Error(GL_OUT_OF_MEMORY, "Failed to create steam output shader, result: 0x%X.", result);
                }
            }

            mVertexExecutable = vertexShader;
            mStreamOutExecutable = streamOutShader;
        }
        break;
      case SHADER_PIXEL:
        {
            ID3D11PixelShader *pixelShader = NULL;

            HRESULT result = device->CreatePixelShader(getFunction(), getLength(), NULL, &pixelShader);
            ASSERT(SUCCEEDED(result));
            if (FAILED(result))
            {
                return gl::Error(GL_OUT_OF_MEMORY, "Failed to create pixel shader, result: 0x%X.", result);
            }

            mPixelExecutable = pixelShader;
        }
        break;
      case SHADER_GEOMETRY:
        {
            ID3D11GeometryShader *geometryShader = NULL;

            HRESULT result = device->CreateGeometryShader(getFunction(), getLength(), NULL, &geometryShader);
            ASSERT(SUCCEEDED(result));
            if (FAILED(result))
            {
                return gl::Error(GL_OUT_OF_MEMORY, "Failed to create geometry shader, result: 0x%X.", result);
            }

            mGeometryExecutable = geometryShader;
        }
        break;
      default:
        UNREACHABLE();
        return gl::Error(GL_INVALID_OPERATION);
    }

    return gl::Error(GL_NO_ERROR);
}

ID3D11VertexShader *ShaderExecutable11::getVertexShader() const
{
    return mVertexExecutable;
}

ID3D11PixelShader *ShaderExecutable11::getPixelShader() const
{
    return mPixelExecutable;
}

ID3D11GeometryShader *ShaderExecutable11::getGeometryShader() const
{
    return mGeometryExecutable;
}

ID3D11GeometryShader *ShaderExecutable11::getStreamOutShader() const
{
    return mStreamOutExecutable;
}

UniformStorage11::UniformStorage11(Renderer11 *renderer, size_t initialSize)
    : UniformStorage(initialSize),
      mConstantBuffer(NULL)
{
    ID3D11Device *d3d11Device = renderer->getDevice();

    if (initialSize > 0)
    {
        D3D11_BUFFER_DESC constantBufferDescription = {0};
        constantBufferDescription.ByteWidth = initialSize;
        constantBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
        constantBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        constantBufferDescription.MiscFlags = 0;
        constantBufferDescription.StructureByteStride = 0;

        HRESULT result = d3d11Device->CreateBuffer(&constantBufferDescription, NULL, &mConstantBuffer);
        UNUSED_ASSERTION_VARIABLE(result);
        ASSERT(SUCCEEDED(result));
    }
}

UniformStorage11::~UniformStorage11()
{
    SafeRelease(mConstantBuffer);
}

const UniformStorage11 *UniformStorage11::makeUniformStorage11(const UniformStorage *uniformStorage)
{
    ASSERT(HAS_DYNAMIC_TYPE(const UniformStorage11*, uniformStorage));
    return static_cast<const UniformStorage11*>(uniformStorage);
}

}
