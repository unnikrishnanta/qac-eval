#include "Heap.h"
#include <iomanip>
#include <cassert>

namespace CGE
{
    Heap* Heap::mFirstHeap = NULL;
    Heap Heap::TotalHeap;

    Heap::Heap(const char* inName) : mName(inName), mBlocks(0), mBytes(0),
        mPeakBlocks(0), mPeakBytes(0), mSmallestBlock(0), mLargestBlock(0)
    {
        assert(mName && *mName);
        mNextHeap = mFirstHeap;
        mFirstHeap = this;
    }

    Heap::Heap() : mName("Total"), mBlocks(0), mBytes(0),
        mPeakBlocks(0), mPeakBytes(0), mSmallestBlock(0), mLargestBlock(0)
    {
    }

    Heap::~Heap()
    {
    }

    void Heap::allocate(size_t inBytes)
    {
        assert(inBytes > 0);

        ++mBlocks;
        mBytes += inBytes;

        if (mBlocks > mPeakBlocks) mPeakBlocks = mBlocks;
        if (mBytes > mPeakBytes) mPeakBytes = mBytes;
        if (inBytes > mLargestBlock) mLargestBlock = inBytes;

        if (mSmallestBlock == 0 || inBytes < mSmallestBlock)
            mSmallestBlock = inBytes;

        if (this != &TotalHeap) TotalHeap.allocate(inBytes);
    }

    void Heap::release(size_t inBytes)
    {
        assert(inBytes > 0);
        assert(mBytes >= inBytes);
        assert(mBlocks > 0);

        --mBlocks;
        mBytes -= inBytes;

        if (this != &TotalHeap) TotalHeap.release(inBytes);
    }

    static void showBar()
    {
        for (size_t i = 0; i < 70; ++i)
            std::cout << '-';

        std::cout << '\n';
    }

    void Heap::dump()
    {
        showBar();

        std::cout
            << std::setw(10) << "Name"
            << std::setw(10) << "Blocks"
            << std::setw(10) << "(Peak)"
            << std::setw(10) << "Bytes"
            << std::setw(10) << "(Peak)"
            << std::setw(10) << "Smallest"
            << std::setw(10) << "Biggest"
            << '\n';

        showBar();

        for (Heap* h = mFirstHeap; h; h = h->mNextHeap)
        {
            std::cout << *h << '\n';
        }

        showBar();

        std::cout << TotalHeap << '\n';

        showBar();
    }

    std::ostream& operator<<(std::ostream& inStream, const Heap& inHeap)
    {
        inStream << std::setw(10) << inHeap.mName
            << std::setw(10) << inHeap.mBlocks
            << std::setw(10) << inHeap.mPeakBlocks
            << std::setw(10) << inHeap.mBytes
            << std::setw(10) << inHeap.mPeakBytes
            << std::setw(10) << inHeap.mSmallestBlock
            << std::setw(10) << inHeap.mLargestBlock
            ;

        return inStream;
    }
}
