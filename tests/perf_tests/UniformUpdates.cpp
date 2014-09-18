//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "UniformUpdates.h"

#include <cassert>
#include <sstream>

#include "shader_utils.h"

std::string UniformUpdatesParams::name() const
{
    std::stringstream strstr;

    strstr << "BufferSubData - " << BenchmarkParams::name()
           << " - " << numUniforms << " uniforms - "
           << iterations << " draw calls";

    return strstr.str();
}

UniformUpdatesBenchmark::UniformUpdatesBenchmark(const UniformUpdatesParams &params)
    : SimpleBenchmark(params.name(), 1280, 720, 2, params.requestedRenderer),
      mParams(params)
{
    mDrawIterations = mParams.iterations;
    assert(mParams.iterations > 0);
}

bool UniformUpdatesBenchmark::initializeBenchmark()
{
    std::stringstream vstrstr;

    vstrstr << "attribute vec2 vPosition;\n"
            << "varying vec4 vOut;\n"
            << "uniform vec4 u[" << mParams.numUniforms << "];\n"
            << "void main()\n"
            << "{\n";

    for (unsigned int uniCount = 0; uniCount < mParams.numUniforms; ++uniCount)
    {
        vstrstr << "    vOut += u[" << uniCount << "];\n";
    }

    vstrstr << "    gl_Position = vec4(vPosition, 0, 1.0);\n"
               "}";

    const std::string &fragShaderSrc =
        "precision mediump float;\n"
        "varying vec4 vOut;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = vOut;\n"
        "}\n";

    mProgram = CompileProgram(vstrstr.str(), fragShaderSrc);
    if (!mProgram)
    {
        return false;
    }

    // Use the program object
    glUseProgram(mProgram);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    static GLfloat vertices[] =
    {
        0.0f, 0.5f,
        -0.5f, -0.5f,
        0.5f, -0.5f,
    };

    glGenBuffers(1, &mBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * 2, vertices, GL_STATIC_DRAW);

    int positionLocation = glGetAttribLocation(mProgram, "vPosition");
    if (positionLocation == -1)
    {
        return false;
    }

    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(positionLocation);

    // Set the viewport
    glViewport(0, 0, getWindow()->getWidth(), getWindow()->getHeight());

    mUniformLocationBase = glGetUniformLocation(mProgram, "u[0]");
    if (mUniformLocationBase == -1)
    {
        return false;
    }

    glDisable(GL_DEPTH_TEST);

    GLenum glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        return false;
    }

    return true;
}

void UniformUpdatesBenchmark::destroyBenchmark()
{
    glDeleteProgram(mProgram);
    glDeleteBuffers(1, &mBuffer);
}

void UniformUpdatesBenchmark::beginDrawBenchmark()
{
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
}

void UniformUpdatesBenchmark::drawBenchmark()
{
    std::vector<GLfloat> data(mParams.numUniforms * 4);
    for (unsigned int it = 0; it < mParams.iterations; ++it)
    {
        data[0] = static_cast<float>(it);
        glUniform4fv(mUniformLocationBase, mParams.numUniforms, data.data());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}
