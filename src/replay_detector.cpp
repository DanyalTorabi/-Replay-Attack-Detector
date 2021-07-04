#include "replay_detector.h"
#include <bits/stdc++.h>
ReplayDetector::ReplayDetector(uint64_t windowPacketSize, uint64_t startPacketNumber):
    _windowBlockCount(windowPacketSize >> REPLAY_DETECTOR_SHIFT),
    //! Convert to zero-base
    _startPacketNumber(startPacketNumber - 1ULL),
    _startBlockNumber(_startPacketNumber >> REPLAY_DETECTOR_SHIFT),
    _lastBlockNumber(0),
    _acceptedPacketCount(0),
    _missedPacketCount(0),
    _unfilledPacketCount(0),
    _duplicatedPacketCount(0),
    _maximumReceivedPacketNumber(0)
{
        if (windowPacketSize & REPLAY_DETECTOR_MASK)
	{
		++_windowBlockCount;
	}

	/// Next packet number might be in a range of last packet to last packet plus window size. So, if
	/// last packet number is 10, it is expected to receive 11 to 1034. It means the first block
	/// number is one and last block number is 17. Window should have 17 blocks.
	++_windowBlockCount;

    _unfilledPacketCount = (_windowBlockCount << REPLAY_DETECTOR_SHIFT)
                           - (_startPacketNumber & REPLAY_DETECTOR_MASK);

	assert(windowPacketSize > 0ULL);
	assert(startPacketNumber > 0ULL);
}

ReplayDetector::ResultTypes  ReplayDetector::checkPacketNumber(uint64_t packetNumber)
{
#ifdef REPLAY_DETECTOR_DEBUG
	std::cout << "packetNumber" << packetNumber << std::endl;
#endif

	if (0ULL == packetNumber)
	{
		return RT_Unknown;
	}

	//! Convert one-based number to zero-based
	--packetNumber;

	if (packetNumber < _startPacketNumber)
	{
		if ((0 != (_maximumReceivedPacketNumber & 0xFF00000000000000ULL))
		    && (0ULL == (packetNumber & 0xFF00000000000000ULL)))
		{
			return RT_Overflow;
		}

		return RT_Unknown;
	}

        uint64_t  blockNumber(packetNumber >> REPLAY_DETECTOR_SHIFT);
        //! Location of packet's bit in its block. (packetNumber & REPLAY_DETECTOR_MASK)
	//! Mask of packet's bit
        uint64_t  packetMask(0x1ULL << (packetNumber & REPLAY_DETECTOR_MASK));
#ifdef REPLAY_DETECTOR_DEBUG
	std::cout << "blockNumber:" << blockNumber << std::endl;
	std::cout << "packetMask:" << packetMask << std::endl;
#endif

	std::unordered_map<uint64_t, uint64_t>::iterator  it(_blocks.find(blockNumber));

	if (it == _blocks.end())
	{
#ifdef REPLAY_DETECTOR_DEBUG
		std::cout << "Block is created" << std::endl;
#endif
		_blocks[blockNumber] = packetMask;
#ifdef REPLAY_DETECTOR_DEBUG
		std::cout << "Block Count:" << _blocks.size() << std::endl;
#endif
	}
	else
	{
#ifdef REPLAY_DETECTOR_DEBUG
		std::cout << "Block is used" << std::endl;
#endif

		if (packetMask & it->second)
		{
			++_duplicatedPacketCount;

			return RT_Duplicate;
		}

		it->second |= packetMask;
	}

	if (blockNumber > _lastBlockNumber)
	{
		_lastBlockNumber = blockNumber;
	}

	++_acceptedPacketCount;
	--_unfilledPacketCount;

	checkBlocks();

	if (_maximumReceivedPacketNumber < packetNumber)
	{
		_maximumReceivedPacketNumber = packetNumber;
	}

	return RT_Save;
}

uint64_t  ReplayDetector::startPacketNumber() const
{
	return _startPacketNumber + 1;
}

uint64_t  ReplayDetector::startBlockNumber() const
{
	return _startBlockNumber + 1;
}

uint64_t  ReplayDetector::acceptedPacketCount() const
{
	return _acceptedPacketCount;
}

uint64_t  ReplayDetector::missedPacketCount() const
{
	return _missedPacketCount;
}

uint64_t  ReplayDetector::unfilledPacketCount() const
{
	return _unfilledPacketCount;
}

uint64_t  ReplayDetector::duplicatedPacketCount() const
{
	return _duplicatedPacketCount;
}

void  ReplayDetector::removeExceptionalFirstBlock(const std::unordered_map<uint64_t,
                                                                          uint64_t>::iterator &it)
{
#ifdef REPLAY_DETECTOR_DEBUG
    std::cout << "_startPacketNumber:" << _startPacketNumber << std::endl;
    std::cout << "_startBlockNumber:" << _startBlockNumber << std::endl;
#endif

    if (_blocks.end() != it)
    {
        //! Number of bits are not important for filling the first block.
        uint64_t  freeBits(REPLAY_DETECTOR_MASK & _startPacketNumber);

        if (((UINT64_MAX >> freeBits) << freeBits) == it->second)
        {
#ifdef REPLAY_DETECTOR_DEBUG
            std::cout << "1-Move out first Block: _startPacketNumber:" << _startPacketNumber
                      << std::endl;
            std::cout << "	: _startBlockNumber:"<< _startBlockNumber << std::endl;
#endif
            _blocks.erase(it);
            _unfilledPacketCount += REPLAY_DETECTOR_BLOCK_BIT_COUNT;
            ++_startBlockNumber;
            _startPacketNumber = _startBlockNumber << REPLAY_DETECTOR_SHIFT;
        }
    }
}

void  ReplayDetector::removeFirstBlock(std::unordered_map<uint64_t, uint64_t>::iterator  &it)
{
    while (_blocks.end() != it && UINT64_MAX == it->second)
    {
#ifdef REPLAY_DETECTOR_DEBUG
        std::cout << "2-Move out first Block: _startPacketNumber:" << _startPacketNumber << std::endl;
        std::cout << "	: _startBlockNumber:"<< _startBlockNumber << std::endl;
#endif
        _blocks.erase(it);
        _unfilledPacketCount += REPLAY_DETECTOR_BLOCK_BIT_COUNT;
        ++_startBlockNumber;
        it = _blocks.find(_startBlockNumber);
    }

    _startPacketNumber = _startBlockNumber << REPLAY_DETECTOR_SHIFT;
}

void  ReplayDetector::moveWindowForward(const uint64_t &currentWindowBlockCount)
{
    uint64_t  diffBlockCount(currentWindowBlockCount - _windowBlockCount);
    int64_t   totallyMissedBlocks(
        static_cast<int64_t>(diffBlockCount) - static_cast<int64_t>(_windowBlockCount));

#ifdef REPLAY_DETECTOR_DEBUG
    std::cout << "currentWindowBlockCount:" << currentWindowBlockCount << std::endl;
    std::cout << "_windowBlockCount:" << _windowBlockCount << std::endl;
    std::cout << "1 diffBlockCount:" << diffBlockCount << std::endl;
#endif

    if (0LL < totallyMissedBlocks)
    {
        _missedPacketCount += REPLAY_DETECTOR_BLOCK_BIT_COUNT
                              * static_cast<uint64_t>(totallyMissedBlocks);
        diffBlockCount -= totallyMissedBlocks;
    }
    else
    {
        totallyMissedBlocks = 0;
    }

#ifdef REPLAY_DETECTOR_DEBUG
    std::cout << "diffBlockCount:" << diffBlockCount << std::endl;
    std::cout << "_missedPacketCount:" << _missedPacketCount << std::endl;
#endif

    while (0 < diffBlockCount)
    {
        uint8_t  validBits(REPLAY_DETECTOR_BLOCK_BIT_COUNT);

        if (REPLAY_DETECTOR_MASK & _startPacketNumber)
        {
            validBits = REPLAY_DETECTOR_BLOCK_BIT_COUNT - (REPLAY_DETECTOR_MASK & _startPacketNumber);
        }

        std::unordered_map<uint64_t, uint64_t>::iterator  it(_blocks.find(_startBlockNumber));

        if (it != _blocks.end())
        {
            uint64_t  blockValue(it->second);

            if (UINT64_MAX != blockValue)
            {
                std::bitset<REPLAY_DETECTOR_BLOCK_BIT_COUNT>  val(blockValue);
                uint8_t                                      unfilled(validBits - val.count());
                _missedPacketCount += unfilled;
#ifdef REPLAY_DETECTOR_DEBUG
                std::cout << "val.count():" << val.count() << std::endl;
                std::cout << "unfilled:" << (int)unfilled << std::endl;
                std::cout << "_unfilledPacketCount:" << _unfilledPacketCount << std::endl;
#endif
                _unfilledPacketCount -= unfilled;
            }

            _blocks.erase(it);
        }
        else
        {
            _missedPacketCount   += validBits;
            _unfilledPacketCount -= validBits;
        }

#ifdef REPLAY_DETECTOR_DEBUG
        std::cout << "3-Move out first Block: _startPacketNumber:" << _startPacketNumber
                  << std::endl;
        std::cout << "	: _startBlockNumber:"<< _startBlockNumber << std::endl;
#endif
        ++_startBlockNumber;
        _startPacketNumber    = _startBlockNumber << REPLAY_DETECTOR_SHIFT;
        _unfilledPacketCount += REPLAY_DETECTOR_BLOCK_BIT_COUNT;
        --diffBlockCount;
    }

    _startBlockNumber += totallyMissedBlocks;
    _startPacketNumber = _startBlockNumber << REPLAY_DETECTOR_SHIFT;
}

void  ReplayDetector::checkBlocks()
{
	std::unordered_map<uint64_t, uint64_t>::iterator  it(_blocks.find(_startBlockNumber));

	/// Check the first exceptional block.
        if (REPLAY_DETECTOR_MASK & _startPacketNumber)
	{
        removeExceptionalFirstBlock(it);
	}
	else
	{
        removeFirstBlock(it);
	}

	if (_blocks.size())
	{
		uint64_t  currentWindowBlockCount(_lastBlockNumber - _startBlockNumber + 1);

		if (currentWindowBlockCount > _windowBlockCount)
        {
            //! Window should move forward. Because of receiving new packets that cause increasing in the
            //! size of window.
            moveWindowForward(currentWindowBlockCount);
		}
	}
}
