//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "BufferSubData.h"

#include <cassert>
#include <sstream>

#include "shader_utils.h"

namespace
{

GLfloat *GetFloatData(GLint componentCount)
{
    static GLfloat vertices2[] =
    {
        1, 2,
        0, 0,
        2, 0,
    };

    static GLfloat vertices3[] =
    {
        1, 2, 1,
        0, 0, 1,
        2, 0, 1,
    };

    static GLfloat vertices4[] =
    {
        1, 2, 1, 3,
        0, 0, 1, 3,
        2, 0, 1, 3,
    };

    switch (componentCount)
    {
      case 2: return vertices2;
      case 3: return vertices3;
      case 4: return vertices4;
      default: return NULL;
    }
}

template <class T>
GLsizeiptr GetNormalizedData(GLsizeiptr numElements, GLfloat *floatData, std::vector<uint8_t> *data)
{
    GLsizeiptr triDataSize = sizeof(T) * numElements;
    data->resize(triDataSize);

    T *destPtr = reinterpret_cast<T*>(data->data());

    for (GLsizeiptr dataIndex = 0; dataIndex < numElements; dataIndex++)
    {
        GLfloat scaled = floatData[dataIndex] * 0.25f;
        destPtr[dataIndex] = static_cast<T>(scaled * static_cast<GLfloat>(std::numeric_limits<T>::max()));
    }

    return triDataSize;
}

template <class T>
GLsizeiptr GetIntData(GLsizeiptr numElements, GLfloat *floatData, std::vector<uint8_t> *data)
{
    GLsizeiptr triDataSize = sizeof(T) * numElements;
    data->resize(triDataSize);

    T *destPtr = reinterpret_cast<T*>(data->data());

    for (GLsizeiptr dataIndex = 0; dataIndex < numElements; dataIndex++)
    {
        destPtr[dataIndex] = static_cast<T>(floatData[dataIndex]);
    }

    return triDataSize;
}

GLsizeiptr GetVertexData(GLenum type, GLint componentCount, GLboolean normalized, std::vector<uint8_t> *data)
{
    GLsizeiptr triDataSize = 0;
    GLfloat *floatData = GetFloatData(componentCount);

    if (type == GL_FLOAT)
    {
        triDataSize = sizeof(GLfloat) * componentCount * 3;
        data->resize(triDataSize);
        memcpy(data->data(), floatData, triDataSize);
    }
    else if (normalized == GL_TRUE)
    {
        GLsizeiptr numElements = componentCount * 3;

        switch (type)
        {
          case GL_BYTE:           triDataSize = GetNormalizedData<GLbyte>(numElements, floatData, data); break;
          case GL_SHORT:          triDataSize = GetNormalizedData<GLshort>(numElements, floatData, data); break;
          case GL_INT:            triDataSize = GetNormalizedData<GLint>(numElements, floatData, data); break;
          case GL_UNSIGNED_BYTE:  triDataSize = GetNormalizedData<GLubyte>(numElements, floatData, data); break;
          case GL_UNSIGNED_SHORT: triDataSize = GetNormalizedData<GLushort>(numElements, floatData, data); break;
          case GL_UNSIGNED_INT:   triDataSize = GetNormalizedData<GLuint>(numElements, floatData, data); break;
          default: assert(0);
        }
    }
    else
    {
        GLsizeiptr numElements = componentCount * 3;

        switch (type)
        {
          case GL_BYTE:           triDataSize = GetIntData<GLbyte>(numElements, floatData, data); break;
          case GL_SHORT:          triDataSize = GetIntData<GLshort>(numElements, floatData, data); break;
          case GL_INT:            triDataSize = GetIntData<GLint>(numElements, floatData, data); break;
          case GL_UNSIGNED_BYTE:  triDataSize = GetIntData<GLubyte>(numElements, floatData, data); break;
          case GL_UNSIGNED_SHORT: triDataSize = GetIntData<GLushort>(numElements, floatData, data); break;
          case GL_UNSIGNED_INT:   triDataSize = GetIntData<GLuint>(numElements, floatData, data); break;
          default: assert(0);
        }
    }

    return triDataSize;
}

}

std::string BufferSubDataParams::name() const
{
    std::stringstream strstr;

    strstr << "BufferSubData - ";

    switch (requestedRenderer)
    {
      case EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE: strstr << "D3D11"; break;
      case EGL_PLATFORM_ANGLE_TYPE_D3D9_ANGLE: strstr << "D3D9"; break;
      default: strstr << "UNKNOWN RENDERER (" << requestedRenderer << ")"; break;
    }

    strstr << " - ";

    if (vertexNormalized)
    {
        strstr << "Norm";
    }

    switch (vertexType)
    {
      case GL_FLOAT: strstr << "Float"; break;
      case GL_INT: strstr << "Int"; break;
      case GL_BYTE: strstr << "Byte"; break;
      case GL_SHORT: strstr << "Short"; break;
      case GL_UNSIGNED_INT: strstr << "UInt"; break;
      case GL_UNSIGNED_BYTE: strstr << "UByte"; break;
      case GL_UNSIGNED_SHORT: strstr << "UShort"; break;
      default: strstr << "UNKNOWN FORMAT (" << vertexType << ")"; break;
    }

    strstr << vertexComponentCount;

    strstr << " - " << updateSize << "b updates - ";
    strstr << (bufferSize >> 10) << "k buffer - ";
    strstr << iterations << " updates";

    return strstr.str();
}

BufferSubDataBenchmark::BufferSubDataBenchmark(const BufferSubDataParams &params)
    : SimpleBenchmark(params.name(), 1280, 720, 2, params.requestedRenderer),
      mParams(params)
{
    mDrawIterations = mParams.iterations;

    assert(mParams.vertexComponentCount > 1);
    assert(mParams.iterations > 0);
}

bool BufferSubDataBenchmark::initializeBenchmark()
{
    const std::string vs = SHADER_SOURCE
    (
        attribute vec2 vPosition;
        uniform float uScale;
        uniform float uOffset;
        void main()
        {
            gl_Position = vec4(vPosition * vec2(uScale) - vec2(uOffset), 0, 1);
        }
    );

    const std::string fs = SHADER_SOURCE
    (
        precision mediump float;
        void main()
        {
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    );

    mProgram = CompileProgram(vs, fs);
    if (!mProgram)
    {
        return false;
    }

    // Use the program object
    glUseProgram(mProgram);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glGenBuffers(1, &mBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
    glBufferData(GL_ARRAY_BUFFER, mParams.bufferSize, 0, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, mParams.vertexComponentCount, mParams.vertexType,
                          mParams.vertexNormalized, 0, 0);
    glEnableVertexAttribArray(0);

    mUpdateData = new uint8_t[mParams.updateSize];

    std::vector<uint8_t> data;
    GLsizei triDataSize = GetVertexData(mParams.vertexType,
                                        mParams.vertexComponentCount,
                                        mParams.vertexNormalized, &data);

    mNumTris = mParams.updateSize / triDataSize;
    for (int i = 0, offset = 0; i < mNumTris; ++i)
    {
        memcpy(mUpdateData + offset, data.data(), triDataSize);
        offset += triDataSize;
    }

    // Set the viewport
    glViewport(0, 0, getWindow()->getWidth(), getWindow()->getHeight());

    GLfloat scale = 0.5f;
    GLfloat offset = 0.5f;

    if (mParams.vertexNormalized == GL_TRUE)
    {
        scale = 2.0f;
        offset = 0.5f;
    }

    glUniform1f(glGetUniformLocation(mProgram, "uScale"), scale);
    glUniform1f(glGetUniformLocation(mProgram, "uOffset"), offset);

    GLenum glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        return false;
    }

    return true;
}

void BufferSubDataBenchmark::destroyBenchmark()
{
    glDeleteProgram(mProgram);
    glDeleteBuffers(1, &mBuffer);
    delete[] mUpdateData;
}

void BufferSubDataBenchmark::beginDrawBenchmark()
{
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
}

void BufferSubDataBenchmark::drawBenchmark()
{
    for (unsigned int it = 0; it < mParams.iterations; it++)
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, mParams.updateSize, mUpdateData);
        glDrawArrays(GL_TRIANGLES, 0, 3 * mNumTris);
    }
}
