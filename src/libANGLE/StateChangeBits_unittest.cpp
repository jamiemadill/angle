//
// Copyright 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// StateChangeBits:
//   Tracks state changes compactly, to notify the Renderer/Impl layer.
//

#include <gtest/gtest.h>

#include "libANGLE/StateChangeBits.h"

using namespace gl;

// TODO(jmadill)
namespace
{

class StateChangeBitsTest : public testing::Test
{
  protected:
    StateChangeBits stateBits;
};

TEST_F(StateChangeBitsTest, Iterator)
{
    stateBits.markDirty(0x004);
    stateBits.markDirty(0x040);
    stateBits.markDirty(0x400);

    std::set<uint64_t> values;

    for (uint64_t bit : stateBits)
    {
        EXPECT_TRUE(bit == 0x400 || bit == 0x040 || bit == 0x004);
        EXPECT_EQ(0u, values.count(bit));
        values.insert(bit);
    }
    EXPECT_EQ(3u, values.size());
}

} // anonymous namespace
