//
// Copyright 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// StateChangeBits:
//   Tracks state changes compactly, to notify the Renderer/Impl layer.
//

#include "libANGLE/StateChangeBits.h"

#include "common/platform.h"

namespace gl
{

uint64_t StateChangeBits::getNextBit() const
{
    unsigned long firstBitIndex = 0ul;
    // FIXME: other platforms: GCC, Clang
    unsigned char ret = _BitScanForward64(&firstBitIndex, mDirtyBits);

    if (ret == 0)
        return 0ull;

    return ANGLE_BIT(firstBitIndex);
}

uint64_t StateChangeBits::getAndClearNextBit()
{
    uint64_t bitValue = getNextBit();
    markClean(bitValue);
    return bitValue;
}

void StateChangeBits::clearNextBit()
{
    markClean(getNextBit());
}

} // gl
