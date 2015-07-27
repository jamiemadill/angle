//
// Copyright 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// StablityTest:
//   Tests pertaining to flakes and other hard-to-reproduce bugs in ANGLE.
//

#include "test_utils/ANGLETest.h"

using namespace angle;

namespace
{

class StabilityTest : public ANGLETest
{
  protected:
    StabilityTest()
    {
        setWindowWidth(64);
        setWindowHeight(64);
    }
};

// Stale buffers could be re-used in the GL back-end in some cases
TEST_P(StabilityTest, AppliedBuffersBug)
{
    std::vector<float> oneData(64, 1.0f);
    std::vector<float> twoData(64, 2.0f);

    std::string vertexShader =
        "attribute vec4 position;\n"
        "attribute float testAttrib;\n"
        "varying float v;\n"
        "void main() {\n"
        "  gl_Position = position;\n"
        "  v = testAttrib;\n"
        "}";

    std::string fragShader =
        "varying highp float v;\n"
        "void main() {\n"
        "  gl_FragColor = vec4(v == 1.0 ? 1.0 : 0.0, v == 2.0 ? 1.0 : 0.0, 0.0, 1.0);\n"
        "}";

    GLuint program = CompileProgram(vertexShader, fragShader);
    ASSERT_NE(0u, program);

    glUseProgram(program);

    GLint attribLocation = glGetAttribLocation(program, "testAttrib");
    ASSERT_NE(-1, attribLocation);

    GLuint firstBuffer;
    glGenBuffers(1, &firstBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, firstBuffer);
    glBufferData(GL_ARRAY_BUFFER, oneData.size() * sizeof(float), &oneData[0], GL_STATIC_DRAW);
    glVertexAttribPointer(attribLocation, 1, GL_FLOAT, GL_FALSE, 4, nullptr);
    glEnableVertexAttribArray(attribLocation);

    drawQuad(program, "position", 0.5f);
    ASSERT_GL_NO_ERROR();

    EXPECT_PIXEL_EQ(0, 0, 255, 0, 0, 255);

    glDeleteBuffers(1, &firstBuffer);

    GLuint secondBuffer;
    glGenBuffers(1, &secondBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, secondBuffer);
    glBufferData(GL_ARRAY_BUFFER, twoData.size() * sizeof(float), &twoData[0], GL_STATIC_DRAW);
    glVertexAttribPointer(attribLocation, 1, GL_FLOAT, GL_FALSE, 4, nullptr);
    glEnableVertexAttribArray(attribLocation);

    drawQuad(program, "position", 0.5f);
    ASSERT_GL_NO_ERROR();

    EXPECT_PIXEL_EQ(0, 0, 0, 255, 0, 255);
    glDeleteBuffers(1, &secondBuffer);

    glDeleteProgram(program);
}

ANGLE_INSTANTIATE_TEST(StabilityTest, ES2_OPENGL());

}  // anonymous namespace
