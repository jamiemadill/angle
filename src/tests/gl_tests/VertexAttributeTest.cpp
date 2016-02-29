//
// Copyright 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "test_utils/ANGLETest.h"

using namespace angle;

namespace
{

class VertexAttributeTest : public ANGLETest
{
  protected:
    VertexAttributeTest()
    {
        setWindowWidth(128);
        setWindowHeight(128);
        setConfigRedBits(8);
        setConfigGreenBits(8);
        setConfigBlueBits(8);
        setConfigAlphaBits(8);
        setConfigDepthBits(24);

        mProgram = 0;
        mTestAttrib = -1;
        mExpectedAttrib = -1;
    }

    struct TestData
    {
        GLenum type;
        GLboolean normalized;

        const void *inputData;
        const GLfloat *expectedData;
    };

    void runTest(const TestData& test)
    {
        // TODO(geofflang): Figure out why this is broken on AMD OpenGL
        if (IsAMD() && getPlatformRenderer() == EGL_PLATFORM_ANGLE_TYPE_OPENGL_ANGLE)
        {
            std::cout << "Test skipped on AMD OpenGL." << std::endl;
            return;
        }

        GLint viewportSize[4];
        glGetIntegerv(GL_VIEWPORT, viewportSize);

        GLint midPixelX = (viewportSize[0] + viewportSize[2]) / 2;
        GLint midPixelY = (viewportSize[1] + viewportSize[3]) / 2;

        for (GLint i = 0; i < 4; i++)
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glVertexAttribPointer(mTestAttrib, i + 1, test.type, test.normalized, 0, test.inputData);
            glVertexAttribPointer(mExpectedAttrib, i + 1, GL_FLOAT, GL_FALSE, 0, test.expectedData);

            glEnableVertexAttribArray(mTestAttrib);
            glEnableVertexAttribArray(mExpectedAttrib);

            drawQuad(mProgram, "position", 0.5f);

            glDisableVertexAttribArray(mTestAttrib);
            glDisableVertexAttribArray(mExpectedAttrib);

            // We need to offset our checks from triangle edges to ensure we don't fall on a single tri
            // Avoid making assumptions of drawQuad with four checks to check the four possible tri regions
            EXPECT_PIXEL_EQ((midPixelX + viewportSize[0]) / 2, midPixelY, 255, 255, 255, 255);
            EXPECT_PIXEL_EQ((midPixelX + viewportSize[2]) / 2, midPixelY, 255, 255, 255, 255);
            EXPECT_PIXEL_EQ(midPixelX, (midPixelY + viewportSize[1]) / 2, 255, 255, 255, 255);
            EXPECT_PIXEL_EQ(midPixelX, (midPixelY + viewportSize[3]) / 2, 255, 255, 255, 255);
        }
    }

    void SetUp() override
    {
        ANGLETest::SetUp();

        const std::string testVertexShaderSource = SHADER_SOURCE
        (
            attribute highp vec4 position;
            attribute highp vec4 test;
            attribute highp vec4 expected;

            varying highp vec4 color;

            void main(void)
            {
                gl_Position = position;
                color = vec4(lessThan(abs(test - expected), vec4(1.0 / 64.0)));
            }
        );

        const std::string testFragmentShaderSource = SHADER_SOURCE
        (
            varying highp vec4 color;
            void main(void)
            {
                gl_FragColor = color;
            }
        );

        mProgram = CompileProgram(testVertexShaderSource, testFragmentShaderSource);
        if (mProgram == 0)
        {
            FAIL() << "shader compilation failed.";
        }

        mTestAttrib = glGetAttribLocation(mProgram, "test");
        mExpectedAttrib = glGetAttribLocation(mProgram, "expected");

        glUseProgram(mProgram);

        glClearColor(0, 0, 0, 0);
        glClearDepthf(0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
    }

    void TearDown() override
    {
        glDeleteProgram(mProgram);

        ANGLETest::TearDown();
    }

    GLuint compileMultiAttribProgram(GLint attribCount)
    {
        std::stringstream shaderStream;

        shaderStream << "attribute highp vec4 position;" << std::endl;
        for (GLint attribIndex = 0; attribIndex < attribCount; ++attribIndex)
        {
            shaderStream << "attribute float a" << attribIndex << ";" << std::endl;
        }
        shaderStream << "varying highp float color;" << std::endl
                     << "void main() {" << std::endl
                     << "  gl_Position = position;" << std::endl
                     << "  color = 0.0;" << std::endl;
        for (GLint attribIndex = 0; attribIndex < attribCount; ++attribIndex)
        {
            shaderStream << "  color += a" << attribIndex << ";" << std::endl;
        }
        shaderStream << "}" << std::endl;

        const std::string testFragmentShaderSource = SHADER_SOURCE
        (
            varying highp float color;
            void main(void)
            {
                gl_FragColor = vec4(color, 0.0, 0.0, 1.0);
            }
        );

        return CompileProgram(shaderStream.str(), testFragmentShaderSource);
    }

    void setupMultiAttribs(GLuint program, GLint attribCount, GLfloat value)
    {
        glUseProgram(program);
        for (GLint attribIndex = 0; attribIndex < attribCount; ++attribIndex)
        {
            std::stringstream attribStream;
            attribStream << "a" << attribIndex;
            GLint location = glGetAttribLocation(program, attribStream.str().c_str());
            ASSERT_NE(-1, location);
            glVertexAttrib1f(location, value);
            glDisableVertexAttribArray(location);
        }
    }

    static const size_t mVertexCount = 24;

    GLuint mProgram;
    GLint mTestAttrib;
    GLint mExpectedAttrib;
};

TEST_P(VertexAttributeTest, UnsignedByteUnnormalized)
{
    GLubyte inputData[mVertexCount] = { 0, 1, 2, 3, 4, 5, 6, 7, 125, 126, 127, 128, 129, 250, 251, 252, 253, 254, 255 };
    GLfloat expectedData[mVertexCount];
    for (size_t i = 0; i < mVertexCount; i++)
    {
        expectedData[i] = inputData[i];
    }

    TestData data = { GL_UNSIGNED_BYTE, GL_FALSE, inputData, expectedData };
    runTest(data);
}

TEST_P(VertexAttributeTest, UnsignedByteNormalized)
{
    GLubyte inputData[mVertexCount] = { 0, 1, 2, 3, 4, 5, 6, 7, 125, 126, 127, 128, 129, 250, 251, 252, 253, 254, 255 };
    GLfloat expectedData[mVertexCount];
    for (size_t i = 0; i < mVertexCount; i++)
    {
        expectedData[i] = inputData[i] / 255.0f;
    }

    TestData data = { GL_UNSIGNED_BYTE, GL_TRUE, inputData, expectedData };
    runTest(data);
}

TEST_P(VertexAttributeTest, ByteUnnormalized)
{
    GLbyte inputData[mVertexCount] = { 0, 1, 2, 3, 4, -1, -2, -3, -4, 125, 126, 127, -128, -127, -126 };
    GLfloat expectedData[mVertexCount];
    for (size_t i = 0; i < mVertexCount; i++)
    {
        expectedData[i] = inputData[i];
    }

    TestData data = { GL_BYTE, GL_FALSE, inputData, expectedData };
    runTest(data);
}

TEST_P(VertexAttributeTest, ByteNormalized)
{
    GLbyte inputData[mVertexCount] = { 0, 1, 2, 3, 4, -1, -2, -3, -4, 125, 126, 127, -128, -127, -126 };
    GLfloat expectedData[mVertexCount];
    for (size_t i = 0; i < mVertexCount; i++)
    {
        expectedData[i] = ((2.0f * inputData[i]) + 1.0f) / 255.0f;
    }

    TestData data = { GL_BYTE, GL_TRUE, inputData, expectedData };
    runTest(data);
}

TEST_P(VertexAttributeTest, UnsignedShortUnnormalized)
{
    GLushort inputData[mVertexCount] = { 0, 1, 2, 3, 254, 255, 256, 32766, 32767, 32768, 65533, 65534, 65535 };
    GLfloat expectedData[mVertexCount];
    for (size_t i = 0; i < mVertexCount; i++)
    {
        expectedData[i] = inputData[i];
    }

    TestData data = { GL_UNSIGNED_SHORT, GL_FALSE, inputData, expectedData };
    runTest(data);
}

TEST_P(VertexAttributeTest, UnsignedShortNormalized)
{
    GLushort inputData[mVertexCount] = { 0, 1, 2, 3, 254, 255, 256, 32766, 32767, 32768, 65533, 65534, 65535 };
    GLfloat expectedData[mVertexCount];
    for (size_t i = 0; i < mVertexCount; i++)
    {
        expectedData[i] = inputData[i] / 65535.0f;
    }

    TestData data = { GL_UNSIGNED_SHORT, GL_TRUE, inputData, expectedData };
    runTest(data);
}

TEST_P(VertexAttributeTest, ShortUnnormalized)
{
    GLshort inputData[mVertexCount] = {  0, 1, 2, 3, -1, -2, -3, -4, 32766, 32767, -32768, -32767, -32766 };
    GLfloat expectedData[mVertexCount];
    for (size_t i = 0; i < mVertexCount; i++)
    {
        expectedData[i] = inputData[i];
    }

    TestData data = { GL_SHORT, GL_FALSE, inputData, expectedData };
    runTest(data);
}

TEST_P(VertexAttributeTest, ShortNormalized)
{
    GLshort inputData[mVertexCount] = {  0, 1, 2, 3, -1, -2, -3, -4, 32766, 32767, -32768, -32767, -32766 };
    GLfloat expectedData[mVertexCount];
    for (size_t i = 0; i < mVertexCount; i++)
    {
        expectedData[i] = ((2.0f * inputData[i]) + 1.0f) / 65535.0f;
    }

    TestData data = { GL_SHORT, GL_TRUE, inputData, expectedData };
    runTest(data);
}

// Validate that we can support GL_MAX_ATTRIBS attribs
TEST_P(VertexAttributeTest, MaxAttribs)
{
    // TODO(jmadill): Figure out why we get this error on AMD/OpenGL and Intel.
    if ((IsIntel() || IsAMD()) && GetParam().getRenderer() == EGL_PLATFORM_ANGLE_TYPE_OPENGL_ANGLE)
    {
        std::cout << "Test skipped on Intel and AMD." << std::endl;
        return;
    }

    GLint maxAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
    ASSERT_GL_NO_ERROR();

    // Reserve one attrib for position
    GLint drawAttribs = maxAttribs - 1;

    GLuint program = compileMultiAttribProgram(drawAttribs);
    ASSERT_NE(0u, program);

    setupMultiAttribs(program, drawAttribs, 0.5f / static_cast<float>(drawAttribs));
    drawQuad(program, "position", 0.5f);

    EXPECT_GL_NO_ERROR();
    EXPECT_PIXEL_NEAR(0, 0, 128, 0, 0, 255, 1);
}

// Validate that we cannot support GL_MAX_ATTRIBS+1 attribs
TEST_P(VertexAttributeTest, MaxAttribsPlusOne)
{
    // TODO(jmadill): Figure out why we get this error on AMD/ES2/OpenGL
    if (IsAMD() && GetParam() == ES2_OPENGL())
    {
        std::cout << "Test disabled on AMD/ES2/OpenGL" << std::endl;
        return;
    }

    GLint maxAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
    ASSERT_GL_NO_ERROR();

    // Exceed attrib count by one (counting position)
    GLint drawAttribs = maxAttribs;

    GLuint program = compileMultiAttribProgram(drawAttribs);
    ASSERT_EQ(0u, program);
}

// Simple test for when we use glBindAttribLocation
TEST_P(VertexAttributeTest, SimpleBindAttribLocation)
{
    // TODO(jmadill): Figure out why this fails on Intel.
    if (IsIntel() && GetParam().getRenderer() == EGL_PLATFORM_ANGLE_TYPE_OPENGL_ANGLE)
    {
        std::cout << "Test skipped on Intel." << std::endl;
        return;
    }

    // Re-use the multi-attrib program, binding attribute 0
    GLuint program = compileMultiAttribProgram(1);
    glBindAttribLocation(program, 2, "position");
    glBindAttribLocation(program, 3, "a0");
    glLinkProgram(program);

    // Setup and draw the quad
    setupMultiAttribs(program, 1, 0.5f);
    drawQuad(program, "position", 0.5f);
    EXPECT_GL_NO_ERROR();
    EXPECT_PIXEL_NEAR(0, 0, 128, 0, 0, 255, 1);
}

GLsizei TypeStride(GLenum attribType)
{
    switch (attribType)
    {
        case GL_UNSIGNED_BYTE:
        case GL_BYTE:
            return 1;
        case GL_UNSIGNED_SHORT:
        case GL_SHORT:
            return 2;
        case GL_UNSIGNED_INT:
        case GL_INT:
        case GL_FLOAT:
            return 4;
        default:
            UNREACHABLE();
            return 0;
    }
}

template <typename T>
GLfloat Normalize(T value)
{
    if (std::is_signed<T>::value)
    {
        return static_cast<GLfloat>(value) / static_cast<GLfloat>(std::numeric_limits<T>::max());
    }
    else
    {
        GLfloat scaled =
            static_cast<GLfloat>(value) + static_cast<GLfloat>(std::numeric_limits<T>::min());
        GLfloat normx2 = scaled / static_cast<GLfloat>(std::numeric_limits<T>::max());
        return normx2 - 1.0f;
    }
}

template <typename DestT>
std::vector<GLfloat> GetExpectedData(const std::vector<GLubyte> &srcData,
                                     GLenum attribType,
                                     GLboolean normalized)
{
    std::vector<GLfloat> expectedData;

    const DestT *typedSrcPtr = reinterpret_cast<const DestT *>(srcData.data());
    size_t iterations        = srcData.size() / TypeStride(attribType);

    if (normalized)
    {
        for (size_t index = 0; index < iterations; ++index)
        {
            expectedData.push_back(Normalize(typedSrcPtr[index]));
        }
    }
    else
    {
        for (size_t index = 0; index < iterations; ++index)
        {
            expectedData.push_back(static_cast<GLfloat>(typedSrcPtr[index]));
        }
    }

    return expectedData;
}

template <typename DestT>
std::vector<GLfloat> GetExpectedUnormData(const std::vector<GLubyte> &srcData, GLenum attribType)
{
    std::vector<GLfloat> expectedData;

    const DestT *typedSrcPtr = reinterpret_cast<DestT>(srcData.data());
    size_t iterations        = srcData.size() / TypeSize(attribType);

    for (size_t index = 0; index < iterations; ++index)
    {
        DestT base = typedSrcPtr[index];
        normalize(base);

        expectedData.push_back(static_cast<GLfloat>());
    }

    return expectedData;
}

// Test sourcing vertex data in different ways from the same buffer.
TEST_P(VertexAttributeTest, SameBufferManyAttributes)
{
    if (isAMD() && getPlatformRenderer() == EGL_PLATFORM_ANGLE_TYPE_OPENGL_ANGLE)
    {
        std::cout << "Test skipped on AMD OpenGL." << std::endl;
        return;
    }

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    std::vector<GLubyte> srcData;
    for (GLubyte i = 0; i < std::numeric_limits<GLubyte>::max(); ++i)
    {
        srcData.push_back(i);
    }

    glBufferData(GL_ARRAY_BUFFER, srcData.size(), srcData.data(), GL_STATIC_DRAW);

    GLint viewportSize[4];
    glGetIntegerv(GL_VIEWPORT, viewportSize);

    struct AttribData
    {
        AttribData(GLenum typeIn, GLint sizeIn, GLboolean normalizedIn, GLsizei strideIn)
            : type(typeIn), size(sizeIn), normalized(normalizedIn), stride(strideIn)
        {
        }

        GLenum type;
        GLint size;
        GLboolean normalized;
        GLsizei stride;
    };

    std::vector<GLenum> attribTypes;
    attribTypes.push_back(GL_BYTE);
    attribTypes.push_back(GL_UNSIGNED_BYTE);
    attribTypes.push_back(GL_SHORT);
    attribTypes.push_back(GL_UNSIGNED_SHORT);

    if (getClientVersion() >= 3)
    {
        attribTypes.push_back(GL_INT);
        attribTypes.push_back(GL_UNSIGNED_INT);
    }

    std::vector<AttribData> datas;

    const GLint maxSize = 1;
    const GLsizei maxStride = 0;

    for (GLenum attribType : attribTypes)
    {
        for (GLint attribSize = 1; attribSize <= maxSize; ++attribSize)
        {
            for (GLsizei strideOffset = 0; strideOffset <= maxStride; ++strideOffset)
            {
                GLsizei stride =
                    TypeStride(attribType) * static_cast<GLsizei>(attribSize) + strideOffset;

                datas.push_back(AttribData(attribType, attribSize, GL_FALSE, stride));
                //if (attribType != GL_FLOAT)
                //{
                //    datas.push_back(AttribData(attribType, attribSize, GL_TRUE, stride));
                //}
            }
        }
    }

    std::map<GLenum, std::vector<GLfloat>> expectedData;
    std::map<GLenum, std::vector<GLfloat>> normExpectedData;

    expectedData[GL_BYTE]          = GetExpectedData<GLbyte>(srcData, GL_BYTE, GL_FALSE);
    expectedData[GL_UNSIGNED_BYTE] = GetExpectedData<GLubyte>(srcData, GL_UNSIGNED_BYTE, GL_FALSE);
    expectedData[GL_SHORT] = GetExpectedData<GLshort>(srcData, GL_SHORT, GL_FALSE);
    expectedData[GL_UNSIGNED_SHORT] =
        GetExpectedData<GLushort>(srcData, GL_UNSIGNED_SHORT, GL_FALSE);
    expectedData[GL_INT]          = GetExpectedData<GLint>(srcData, GL_INT, GL_FALSE);
    expectedData[GL_UNSIGNED_INT] = GetExpectedData<GLuint>(srcData, GL_UNSIGNED_INT, GL_FALSE);
    expectedData[GL_FLOAT]        = GetExpectedData<GLfloat>(srcData, GL_FLOAT, GL_FALSE);

    normExpectedData[GL_BYTE] = GetExpectedData<GLbyte>(srcData, GL_BYTE, GL_TRUE);
    normExpectedData[GL_UNSIGNED_BYTE] =
        GetExpectedData<GLubyte>(srcData, GL_UNSIGNED_BYTE, GL_TRUE);
    normExpectedData[GL_SHORT] = GetExpectedData<GLshort>(srcData, GL_SHORT, GL_TRUE);
    normExpectedData[GL_UNSIGNED_SHORT] =
        GetExpectedData<GLushort>(srcData, GL_UNSIGNED_SHORT, GL_TRUE);
    normExpectedData[GL_INT]          = GetExpectedData<GLint>(srcData, GL_INT, GL_TRUE);
    normExpectedData[GL_UNSIGNED_INT] = GetExpectedData<GLuint>(srcData, GL_UNSIGNED_INT, GL_TRUE);

    glEnableVertexAttribArray(mTestAttrib);
    glEnableVertexAttribArray(mExpectedAttrib);

    ASSERT_GL_NO_ERROR();

    auto getExpectedData = [&expectedData, &normExpectedData](const AttribData &data)
    {
        if (data.normalized)
        {
            return normExpectedData[data.type];
        }
        else
        {
            return expectedData[data.type];
        }
    };

    // for (int i = 0; i < 10; i++)
    {

        for (const auto &data : datas)
        {
            const std::vector<GLfloat> &expected = getExpectedData(data);

            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glVertexAttribPointer(mTestAttrib, data.size, data.type, data.normalized, data.stride,
                                  nullptr);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glVertexAttribPointer(mExpectedAttrib, data.size, GL_FLOAT, GL_FALSE,
                                  sizeof(GLfloat) * data.stride, expected.data());
            drawQuad(mProgram, "position", 0.5f);
            ASSERT_GL_NO_ERROR();
            EXPECT_PIXEL_EQ(getWindowWidth() / 2, getWindowHeight() / 2, 255, 255, 255, 255);
        }

        //   swapBuffers();
    }

    glDeleteBuffers(1, &buffer);
}

// Use this to select which configurations (e.g. which renderer, which GLES major version) these tests should be run against.
// D3D11 Feature Level 9_3 uses different D3D formats for vertex attribs compared to Feature Levels 10_0+, so we should test them separately.
ANGLE_INSTANTIATE_TEST(VertexAttributeTest,
                       ES2_D3D9(),
                       ES2_D3D11(),
                       ES2_D3D11_FL9_3(),
                       ES2_OPENGL(),
                       ES3_OPENGL(),
                       ES2_OPENGLES(),
                       ES3_OPENGLES());

} // namespace
