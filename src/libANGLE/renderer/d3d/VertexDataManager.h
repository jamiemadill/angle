//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// VertexDataManager.h: Defines the VertexDataManager, a class that
// runs the Buffer translation process.

#ifndef LIBANGLE_RENDERER_D3D_VERTEXDATAMANAGER_H_
#define LIBANGLE_RENDERER_D3D_VERTEXDATAMANAGER_H_

#include "libANGLE/Constants.h"
#include "libANGLE/VertexAttribute.h"
#include "common/angleutils.h"

namespace gl
{
class State;
struct VertexAttribute;
struct VertexAttribCurrentValueData;
}

namespace rx
{
class BufferD3D;
class BufferFactoryD3D;
class StreamingVertexBufferInterface;
class VertexBuffer;

struct TranslatedAttribute
{
    TranslatedAttribute()
        : active(false),
          attribute(NULL),
          currentValueType(GL_NONE),
          offset(0),
          stride(0),
          vertexBuffer(NULL),
          storage(NULL),
          serial(0),
          divisor(0)
    {}

    bool active;

    const gl::VertexAttribute *attribute;
    GLenum currentValueType;
    unsigned int offset;
    unsigned int stride;   // 0 means not to advance the read pointer at all

    VertexBuffer *vertexBuffer;
    BufferD3D *storage;
    unsigned int serial;
    unsigned int divisor;
};

enum class VertexStorageType
{
    VERTEX_STORAGE_UNKNOWN,
    VERTEX_STORAGE_STATIC,         // Translate the vertex data once and re-use it.
    VERTEX_STORAGE_DYNAMIC,        // Translate the data every frame into a ring buffer.
    VERTEX_STORAGE_DIRECT,         // Bind a D3D buffer directly without any translation.
    VERTEX_STORAGE_CURRENT_VALUE,  // Use a single value for the attribute.
};

// Given a vertex attribute, return the type of storage it will use.
VertexStorageType ClassifyAttributeStorage(const gl::VertexAttribute &attrib);

class AttribStorageIndex final : angle::NonCopyable
{
  public:
    std::vector<size_t> *getIndexes(VertexStorageType storageType);

  private:
    std::vector<size_t> mStaticAttribIndexes;
    std::vector<size_t> mDynamicAttribIndexes;
    std::vector<size_t> mCurrentValueAttribIndexes;
    std::vector<size_t> mDirectAttribIndexes;
};

class VertexDataManager : angle::NonCopyable
{
  public:
    VertexDataManager(BufferFactoryD3D *factory);
    virtual ~VertexDataManager();

    gl::Error prepareVertexData(const gl::State &state,
                                GLint start,
                                GLsizei count,
                                std::vector<TranslatedAttribute> *translatedAttribs,
                                GLsizei instances);

  private:
    struct CurrentValueState
    {
        CurrentValueState();
        ~CurrentValueState();

        StreamingVertexBufferInterface *buffer;
        gl::VertexAttribCurrentValueData data;
        size_t offset;
    };

    gl::Error reserveSpaceForAttrib(const TranslatedAttribute &translatedAttrib,
                                    GLsizei count,
                                    GLsizei instances) const;

    gl::Error storeAttribute(TranslatedAttribute *translated,
                             GLint start,
                             GLsizei count,
                             GLsizei instances);

    gl::Error storeCurrentValue(const gl::VertexAttribCurrentValueData &currentValue,
                                TranslatedAttribute *translated,
                                CurrentValueState *cachedState);

    void hintUnmapAllResources(const std::vector<gl::VertexAttribute> &vertexAttributes);

    BufferFactoryD3D *const mFactory;

    StreamingVertexBufferInterface *mStreamingBuffer;
    std::vector<CurrentValueState> mCurrentValueCache;

    // Cache variables
    std::vector<TranslatedAttribute *> mActiveEnabledAttributes;
    std::vector<size_t> mActiveDisabledAttributes;
};

}  // namespace rx

#endif   // LIBANGLE_RENDERER_D3D_VERTEXDATAMANAGER_H_
