//
// Copyright 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// IterableBitSetTest:
//   Test the IterableBitSet class.
//

#include <gtest/gtest.h>

#include "common/IterableBitSet.h"

using namespace angle;

namespace
{
class IterableBitSetTest : public testing::Test
{
  protected:
    IterableBitSet<40> stateBits;
};

TEST_F(IterableBitSetTest, Iterator)
{
    std::set<size_t> originalValues;
    originalValues.insert(2);
    originalValues.insert(6);
    originalValues.insert(8);
    originalValues.insert(35);

    for (size_t value : originalValues)
    {
        stateBits.set(value);
    }

    std::set<size_t> readValues;
    for (size_t bit : stateBits)
    {
        EXPECT_EQ(1u, originalValues.count(bit));
        EXPECT_EQ(0u, readValues.count(bit));
        readValues.insert(bit);
    }

    EXPECT_EQ(originalValues.size(), readValues.size());
}

}  // anonymous namespace
