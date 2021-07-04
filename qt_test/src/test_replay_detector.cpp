#include "test_replay_detector.h"
#include "replay_detector.h"
#include <sys/param.h>
TestReplayDetector::TestReplayDetector()
{
	_resultsInterpreter[ReplayDetector::RT_Duplicate] = "Duplicate";
	_resultsInterpreter[ReplayDetector::RT_Unknown]   = "Unknown";
	_resultsInterpreter[ReplayDetector::RT_Save]      = "Save";
}

bool  TestReplayDetector::TestCase1()
{
	std::cout << "Test Case 1 ------Out of Window------------------------->>>" << std::endl;
	/// 9 at the 6th index is out of window
	unsigned int   arr[] { 1, 2, 3, 4, 5, 7, 9, 8, 6, 10, 11, 12, 13, 14, 15 };
	ReplayDetector  detector(3, 1);

	for (unsigned int pktNo : arr)
	{
		ReplayDetector::ResultTypes  result(detector.checkPacketNumber(pktNo));

		if ((ReplayDetector::RT_Unknown == result) && (9U == pktNo))
		{
			std::cout << "Test Succeeds." << std::endl;

			return true;
		}
	}

	std::cout << "Test failed." << std::endl;

	return false;
}

bool  TestReplayDetector::TestCase2()
{
	std::cout << "Test Case 2 --------normal----------------------->>>" << std::endl;
	int           size(100000000);
	unsigned int  arr[size];

	for (int inx(0); inx < size; ++inx)
	{
		arr[inx] = inx + 1;
	}

	ReplayDetector  detector(3, 1);

	for (unsigned int pktNo : arr)
	{
		ReplayDetector::ResultTypes  result(detector.checkPacketNumber(pktNo));

		if (ReplayDetector::RT_Save != result)
		{
			std::cout << "Test failed." << std::endl;

			return false;
		}
	}

	std::cout << "Test Succeeds." << std::endl;

	return true;
}

bool  TestReplayDetector::TestCase3()
{
	std::cout << "Test Case 3 ------Duplicate------------------------->>>" << std::endl;
	/// 2 at the 6th index is duplicated.
	/// 8 at the 11th index is duplicated.
	unsigned int   arr[] { 1, 2, 3, 4, 5, 6, 2, 7, 8, 10, 11, 8, 9, 12, 13, 14, 15 };
	ReplayDetector  detector(3, 1);
	int            duplicateCounter(0), inx(0);

	for (unsigned int pktNo : arr)
	{
		ReplayDetector::ResultTypes  result(detector.checkPacketNumber(pktNo));

		if ((ReplayDetector::RT_Duplicate == result) && (((2U == pktNo) && (6 == inx)) || ((8U == pktNo)
		                                                                                  && (11 == inx))))
		{
			++duplicateCounter;
		}

		++inx;
	}

	if (2 == duplicateCounter)
	{
		std::cout << "Test Succeeds." << std::endl;

		return true;
	}

	std::cout << "Test failed." << std::endl;

	return false;
}

bool  TestReplayDetector::TestCase4()
{
	std::cout << "Test Case 2 --------normal Full Buffer----------------------->>>" << std::endl;
	int           size(100000000);
	unsigned int  arr[size];

	for (int inx(0); inx < size; ++inx)
	{
		arr[inx] = (63 - (inx % 64)) + 10 + (inx / 64 * 64);
	}

	ReplayDetector  detector(64, 10);

	for (unsigned int pktNo : arr)
	{
		ReplayDetector::ResultTypes  result(detector.checkPacketNumber(pktNo));

		if (ReplayDetector::RT_Save != result)
		{
			std::cout << "Test failed." << std::endl;

			return false;
		}
	}

	std::cout << "Test Succeeds." << std::endl;

	return true;
}

bool  TestReplayDetector::TestCase5()
{
	ReplayDetector  rd1(1000, 1);

	std::cout << "Stat 1:" << std::endl;
	rd1.checkPacketNumber(   0);
	rd1.checkPacketNumber(   1);
	rd1.checkPacketNumber(  12);
	rd1.checkPacketNumber(1000);
	rd1.checkPacketNumber(1024);
	// checkStatistics(rd1, 0, 0, 0, 16);

	rd1.checkPacketNumber(  12);
	rd1.checkPacketNumber(1088);
	// checkStatistics(rd1, 0, 1, 0, 16);
	rd1.checkPacketNumber(   1);
	rd1.checkPacketNumber(  12);
	rd1.checkPacketNumber(  64);
	rd1.checkPacketNumber(  65);

	return 0;
}

bool  TestReplayDetector::TestCase6()
{
	uint64_t       windowSize(0x2000000ULL);
	uint64_t       pktNumberStart(0x1ULL);
	uint64_t       pktNumberRange(0x2000000ULL);
	uint64_t       pktNumberMax(0x15000001ULL);
	ReplayDetector  rd(windowSize, pktNumberStart);

	while (pktNumberStart < pktNumberMax)
	{
		std::srand(unsigned(std::time(0)));
		std::vector<uint64_t>  packetNumbers;

		uint64_t  max(MIN(pktNumberStart + pktNumberRange, pktNumberMax));

		for (uint64_t i = pktNumberStart; i < max; ++i)
		{
			packetNumbers.push_back(i);
		}

		std::random_shuffle(packetNumbers.begin(), packetNumbers.end());

		for (uint64_t pktNumber : packetNumbers)
		{
			// std::cout << "pktNumber:" << pktNumber << std::endl;
			rd.checkPacketNumber(pktNumber);
		}

		pktNumberStart += pktNumberRange;
		std::cout << "pktNumberStart:" << pktNumberStart << "/" << pktNumberMax << std::endl;
	}

// checkStatistics(rd, 0x2100000ULL, 0ULL, 1088ULL, 0x23FFFFFFC080001ULL, 0x8FFFFFFF02000001ULL);
	return false;
}
