//
// Copyright 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// StateChangeBits:
//   Tracks state changes compactly, to notify the Renderer/Impl layer.
//

#include <stdint.h>

namespace gl
{

class StateChangeBits
{
  public:
    StateChangeBits() : mDirtyBits(0ull) {}

    void markDirty(uint64_t bits) { mDirtyBits |= bits; }
    void markClean(uint64_t bits) { mDirtyBits &= ~bits; }
    bool isDirty() const { return mDirtyBits != 0ull; }
    void reset() { mDirtyBits = 0ull; }

    uint64_t getNextBit() const;

    // Gets the next nonzero bit and returns it. Masks off the bit afterwards.
    // Returns 0 if there's no next bit.
    uint64_t getAndClearNextBit();

    // Masks off the next bit
    void clearNextBit();

    // This is a mutable iterator! It clears the bits as it traverses.
    class Iterator final
    {
      public:
        Iterator(StateChangeBits *bits)
            : mBits(bits),
              mCurrentBit(bits ? bits->getNextBit() : 0ull)
        {
        }

        Iterator &operator++()
        {
            mBits->markClean(mCurrentBit);
            mCurrentBit = mBits->getNextBit();
            return *this;
        }
        bool operator!=(const Iterator &other) const
        {
            return getBits() != other.getBits();
        }
        const uint64_t operator*() { return mCurrentBit; }

      private:
        uint64_t getBits() const { return mBits == nullptr ? 0ull : mBits->mDirtyBits; }

        StateChangeBits *mBits;
        uint64_t mCurrentBit;
    };

    Iterator begin() { return Iterator(this); }
    Iterator end() { return Iterator(nullptr); }

  private:
    uint64_t mDirtyBits;
};

#define ANGLE_BIT(X) (1ull << X)

} // gl
