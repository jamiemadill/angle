//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "SimpleBenchmark.h"

struct UniformUpdatesParams : public BenchmarkParams
{
    unsigned int iterations;
    unsigned int numUniforms;

    virtual std::string name() const;
};

class UniformUpdatesBenchmark : public SimpleBenchmark
{
public:
    UniformUpdatesBenchmark(const UniformUpdatesParams &params);

    virtual bool initializeBenchmark();
    virtual void destroyBenchmark();
    virtual void beginDrawBenchmark();
    virtual void drawBenchmark();

private:
    DISALLOW_COPY_AND_ASSIGN(UniformUpdatesBenchmark);

    GLuint mProgram;
    GLuint mBuffer;
    GLint mUniformLocationBase;

    const UniformUpdatesParams mParams;
};
