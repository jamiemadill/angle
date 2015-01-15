//
// Copyright (c) 2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// ShaderExecutable.h: Defines a renderer-agnostic class to contain shader
// executable implementation details.

#ifndef LIBANGLE_RENDERER_SHADEREXECUTABLE_H_
#define LIBANGLE_RENDERER_SHADEREXECUTABLE_H_

#include "common/angleutils.h"
#include "common/debug.h"
#include "libANGLE/Error.h"
#include "libANGLE/Program.h"
#include "libANGLE/angletypes.h"

#include <vector>
#include <cstdint>

#include <ppltasks.h> // For create_task

namespace rx
{

#define USE_CONCURRENCY

struct CompileWrapper
{
    CompileWrapper() {}

#ifdef USE_CONCURRENCY

    struct ErrorFunctionPair
    {
        gl::Error error;
        const void *function;
        size_t length;
    };

    typedef Concurrency::task<ErrorFunctionPair> CompileTask;

    CompileWrapper(const CompileTask &compileTask)
        : mCompileTask(compileTask)
    {}

    CompileTask mCompileTask;

    gl::Error getFunction(std::vector<uint8_t> *bufferOut)
    {
        auto result = mCompileTask.get();
        bufferOut->resize(result.length);
        memcpy(&bufferOut[0], result.function, result.length);
        return result.error;
    }
#endif

    CompileWrapper(const uint8_t *function, size_t length, const gl::Error &error)
        : mFunction(function),
          mLength(length),
          mError(error)
    {}

    gl::Error getFunction(std::vector<uint8_t> *bufferOut)
    {
        bufferOut->resize(mLength);
        memcpy(&bufferOut[0], mFunction, mLength);
        return mError;
    }

    const uint8_t *mFunction;
    size_t mLength;
    gl::Error mError;
};

class ShaderExecutable
{
public:
    ShaderExecutable(const CompileWrapper &compileWrapper)
        : mCompileWrapper(compileWrapper)
    {
    }

    virtual ~ShaderExecutable() {}

    gl::Error finishCompile(ShaderType shaderType,
                            const std::vector<gl::LinkedVarying> &transformFeedbackVaryings,
                            bool separatedOutputBuffers)
    {
        gl::Error error = mCompileWrapper.getFunction(&mFunctionBuffer);

        if (!error.isError())
        {
            error = finishLoad(shaderType, transformFeedbackVaryings, separatedOutputBuffers);
        }

        return error;
    }

    virtual gl::Error finishLoad(ShaderType shaderType,
                                 const std::vector<gl::LinkedVarying> &transformFeedbackVaryings,
                                 bool separatedOutputBuffers) {}

    const uint8_t *getFunction() const
    {
        return mFunctionBuffer.data();
    }

    size_t getLength() const
    {
        return mFunctionBuffer.size();
    }

    const std::string &getDebugInfo() const
    {
        return mDebugInfo;
    }

    void appendDebugInfo(const std::string &info)
    {
        mDebugInfo += info;
    }

  private:
    DISALLOW_COPY_AND_ASSIGN(ShaderExecutable);

    CompileWrapper mCompileWrapper;
    std::vector<uint8_t> mFunctionBuffer;
    std::string mDebugInfo;
};

class UniformStorage
{
  public:
    UniformStorage(size_t initialSize)
        : mSize(initialSize)
    {
    }

    virtual ~UniformStorage() {}

    size_t size() const { return mSize; }

  private:
    size_t mSize;
};

}

#endif // LIBANGLE_RENDERER_SHADEREXECUTABLE_H_
