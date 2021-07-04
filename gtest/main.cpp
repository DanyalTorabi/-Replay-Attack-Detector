#include <gtest/gtest.h>
#include "replay_detector.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sys/param.h>


static bool  verbose(false);
void  checkStatistics(const ReplayDetector &rd, uint64_t acceptedPacketCount, uint64_t
                      missedPacketCount,
                      uint64_t unfilledPacketCount, uint64_t startBlockNumber, uint64_t
                      startPacketNumber)
{
	EXPECT_EQ(acceptedPacketCount, rd.acceptedPacketCount());
	EXPECT_EQ(missedPacketCount,   rd.missedPacketCount());
	EXPECT_EQ(unfilledPacketCount, rd.unfilledPacketCount());
	EXPECT_EQ(startBlockNumber,    rd.startBlockNumber());
	EXPECT_EQ(startPacketNumber,   rd.startPacketNumber());
}

/// First Block: Started from 1, incompleted,
/// Other Blocks: Incompleted.
/// Pull the window from end as much as 1 block.
TEST(ReplayDetector, ForwardWindow_FromEnd_1)
{
        ReplayDetector  rd(1000ULL, 1ULL);

	SCOPED_TRACE("A");
	checkStatistics(rd, 0ULL, 0ULL, 1088ULL, 1ULL, 1ULL);

        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(0ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(1ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(12ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(1000ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(1024ULL));

	SCOPED_TRACE("B");
	checkStatistics(rd, 4ULL, 0ULL, 1084ULL, 1ULL, 1ULL);

        EXPECT_EQ(ReplayDetector::RT_Duplicate, rd.checkPacketNumber(12ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,      rd.checkPacketNumber(1089ULL));

	SCOPED_TRACE("C");
	checkStatistics(rd, 5ULL, 62ULL, 1085ULL, 2ULL, 65ULL);
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(1ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(12ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(64ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(65ULL));

	SCOPED_TRACE("D");
	checkStatistics(rd, 6ULL, 62ULL, 1084ULL, 2ULL, 65ULL);
}

/// First Block: Started from 129, incompleted,
/// Other Blocks: Incompleted.
/// Pull the window from end as much as 1 block.
TEST(ReplayDetector, ForwardWindow_FromEnd_2)
{
        ReplayDetector  rd(1000ULL, 129ULL);

	checkStatistics(rd, 0ULL, 0ULL, 1088ULL, 3ULL, 129ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(100ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(129ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(131ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(1000ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(1216ULL));

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 4ULL, 0ULL, 1084ULL, 3ULL, 129ULL);

        EXPECT_EQ(ReplayDetector::RT_Duplicate, rd.checkPacketNumber(131ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,      rd.checkPacketNumber(1217ULL));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 5ULL, 62ULL, 1085ULL, 4ULL, 193ULL);
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(129ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(131ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(192ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(193ULL));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 6ULL, 62ULL, 1084ULL, 4ULL, 193ULL);
}

/// First Block: Started from 10, incompleted,
/// Other Blocks: Incompleted.
/// Pull the window from end as much as 1 block.
TEST(ReplayDetector, ForwardWindow_FromEnd_3)
{
        ReplayDetector  rd(1000ULL, 10ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

	checkStatistics(rd, 0ULL, 0ULL, 1079ULL, 1ULL, 10ULL);
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(0ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(1ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(12ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(1000ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(1088ULL));

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 3ULL, 0ULL, 1076ULL, 1ULL, 10ULL);
        EXPECT_EQ(ReplayDetector::RT_Duplicate, rd.checkPacketNumber(12ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,      rd.checkPacketNumber(1089ULL));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 4ULL, 54ULL, 1085ULL, 2ULL, 65ULL);
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(1ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(2ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(64ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(65ULL));

	if (verbose)
	{
		std::cout << "Stat 4:" << std::endl;
	}

	checkStatistics(rd, 5ULL, 54ULL, 1084ULL, 2ULL, 65ULL);
}

/// First Block: Started from 160, incompleted,
/// Other Blocks: Incompleted.
/// Pull the window from end as much as 1 block.
TEST(ReplayDetector, ForwardWindow_FromEnd_4)
{
        ReplayDetector  rd(1000ULL, 160ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

	checkStatistics(rd, 0ULL, 0ULL, 1057ULL, 3ULL, 160ULL);
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(10ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(159ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(160ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(1000ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(1216ULL));

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 3ULL, 0ULL, 1054ULL, 3ULL, 160ULL);
        EXPECT_EQ(ReplayDetector::RT_Duplicate, rd.checkPacketNumber(160ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,      rd.checkPacketNumber(1217ULL));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 4ULL, 32ULL, 1085ULL, 4ULL, 193ULL);
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(159ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(160ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown, rd.checkPacketNumber(192ULL));
        EXPECT_EQ(ReplayDetector::RT_Save,    rd.checkPacketNumber(193ULL));

	if (verbose)
	{
		std::cout << "Stat 4:" << std::endl;
	}

	checkStatistics(rd, 5ULL, 32ULL, 1084ULL, 4ULL, 193ULL);
}

/// First Block: Started from 1, completed,
/// Other Blocks: Incompleted.
/// Push the window from start as much as 1 block.
TEST(ReplayDetector, ForwardWindow_FromStart_5)
{
	uint64_t       startPktNo(1ULL);
        ReplayDetector  rd(1000ULL, startPktNo);

	checkStatistics(rd, 0ULL, 0ULL, 1088ULL, 1ULL, 1ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

	while (startPktNo <= 63ULL)
	{
                EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo++));
	}

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 63ULL, 0ULL, 1025ULL, 1ULL, 1ULL);

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 64ULL, 0ULL, 1088ULL, 2ULL, 65ULL);
}

/// First Block: Started from 129, completed,
/// Other Blocks: Incompleted.
/// Push the window from start as much as 1 block.
TEST(ReplayDetector, ForwardWindow_FromStart_6)
{
	uint64_t       startPktNo(129ULL);
        ReplayDetector  rd(1000ULL, startPktNo);

	checkStatistics(rd, 0ULL, 0ULL, 1088ULL, 3ULL, 129ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

	while (startPktNo <= 191ULL)
	{
                EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo++));
	}

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 63ULL, 0ULL, 1025ULL, 3ULL, 129ULL);

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 64ULL, 0ULL, 1088ULL, 4ULL, 193ULL);
}

/// First Block: Started from 10, completed,
/// Other Blocks: Incompleted.
/// Push the window from start as much as 1 block.
TEST(ReplayDetector, ForwardWindow_FromStart_7)
{
	uint64_t       startPktNo(10ULL);
        ReplayDetector  rd(1000ULL, startPktNo);

	checkStatistics(rd, 0ULL, 0ULL, 1079ULL, 1ULL, 10ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

	while (startPktNo <= 63ULL)
	{
                EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo++));
	}

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 54ULL, 0ULL, 1025ULL, 1ULL, 10ULL);

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 55ULL, 0ULL, 1088ULL, 2ULL, 65ULL);
}

/// First Block: Started from 160, completed,
/// Other Blocks: Incompleted.
/// Push the window from start as much as 1 block.
TEST(ReplayDetector, ForwardWindow_FromStart_8)
{
	uint64_t       startPktNo(160ULL);
        ReplayDetector  rd(1000ULL, startPktNo);

	checkStatistics(rd, 0ULL, 0ULL, 1057ULL, 3ULL, 160ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

	while (startPktNo <= 191ULL)
	{
                EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo++));
	}

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 32ULL, 0ULL, 1025ULL, 3ULL, 160ULL);

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 33ULL, 0ULL, 1088ULL, 4ULL, 193ULL);
}

/// First Block: Started from 1, incompleted,
/// Second Block: Completed.
/// Third Block: Completed.
/// Other Blocks: Incompleted
/// Pull the window from end as much as 2 blocks.
/// Auto push the window from start as much as 1 block.
TEST(ReplayDetector, ForwardWindow_FromEnd_9)
{
        ReplayDetector  rd(1000ULL, 1ULL);
	uint64_t       startPktNo(65ULL);

	checkStatistics(rd, 0ULL, 0ULL, 1088ULL, 1ULL, 1ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(1ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(2ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(3ULL));

	while (startPktNo <= 192ULL)
	{
                EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo++));
	}

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 131ULL, 0ULL, 957ULL, 1ULL, 1ULL);

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(1153ULL));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 132ULL, 61ULL, 1023ULL, 3ULL, 129ULL);
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(1088ULL));

	if (verbose)
	{
		std::cout << "Stat 4:" << std::endl;
	}

	checkStatistics(rd, 133ULL, 61ULL, 1086ULL, 4ULL, 193ULL);
}

/// First Block: Started from 21, incompleted,
/// Second Block: Completed.
/// Third Block: Completed.
/// Other Blocks: Incompleted
/// Pull the window from end as much as 2 blocks.
/// Auto push the window from start as much as 1 block.
TEST(ReplayDetector, ForwardWindow_FromEnd_10)
{
        ReplayDetector  rd(1000ULL, 21ULL);
	uint64_t       startPktNo(65ULL);

	checkStatistics(rd, 0ULL, 0ULL, 1068ULL, 1ULL, 21ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(21ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(22ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(23ULL));

	while (startPktNo <= 192ULL)
	{
                EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo++));
	}

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 131ULL, 0ULL, 937ULL, 1ULL, 21ULL);

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(1153ULL));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 132ULL, 41ULL, 1023ULL, 3ULL, 129ULL);
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(1088ULL));

	if (verbose)
	{
		std::cout << "Stat 4:" << std::endl;
	}

	checkStatistics(rd, 133ULL, 41ULL, 1086ULL, 4ULL, 193ULL);
}

/// First Block: Started from 129, incompleted,
/// Second Block: Completed.
/// Third Block: Completed.
/// Other Blocks: Incompleted
/// Pull the window from end as much as 2 blocks.
/// Auto push the window from start as much as 1 block.
TEST(ReplayDetector, ForwardWindow_FromEnd_11)
{
        ReplayDetector  rd(1000ULL, 129ULL);
	uint64_t       startPktNo(193ULL);

	checkStatistics(rd, 0ULL, 0ULL, 1088ULL, 3ULL, 129ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(131ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(132ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(133ULL));

	while (startPktNo <= 320ULL)
	{
                EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo++));
	}

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 131ULL, 0ULL, 957ULL, 3ULL, 129ULL);

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(1281ULL));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 132ULL, 61ULL, 1023ULL, 5ULL, 257ULL);
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(1088));

	if (verbose)
	{
		std::cout << "Stat 4:" << std::endl;
	}

	checkStatistics(rd, 133ULL, 61ULL, 1086ULL, 6ULL, 321ULL);
}

/// First Block: Started from 149, incompleted,
/// Second Block: Completed.
/// Third Block: Completed.
/// Other Blocks: Incompleted
/// Pull the window from end as much as 2 blocks.
/// Auto push the window from start as much as 1 block.
TEST(ReplayDetector, ForwardWindow_FromEnd_12)
{
        ReplayDetector  rd(1000ULL, 149ULL);
	uint64_t       startPktNo(193ULL);

	checkStatistics(rd, 0ULL, 0ULL, 1068ULL, 3ULL, 149ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(151ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(152ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(153ULL));

	while (startPktNo <= 320ULL)
	{
                EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo++));
	}

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 131ULL, 0ULL, 937ULL, 3ULL, 149ULL);

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(1281ULL));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 132ULL, 41ULL, 1023ULL, 5ULL, 257ULL);
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(1088ULL));

	if (verbose)
	{
		std::cout << "Stat 4:" << std::endl;
	}

	checkStatistics(rd, 133ULL, 41ULL, 1086ULL, 6ULL, 321ULL);
}

/// First Block: Started from 1, incompleted,
/// Second Block: Completed.
/// Third Block: Completed.
/// Other Blocks: Incompleted
/// Pull the window from end as much as twice the size of blocks in window.
TEST(ReplayDetector, ForwardWindow_FromEnd_13)
{
        ReplayDetector  rd(1000ULL, 1ULL);
	uint64_t       startPktNo(65ULL);

	checkStatistics(rd, 0ULL, 0ULL, 1088ULL, 1ULL, 1ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(1ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(2ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(3ULL));

	while (startPktNo <= 192ULL)
	{
                EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo++));
	}

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 131ULL, 0ULL, 957ULL, 1ULL, 1ULL);

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(3136ULL));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 132, 1917, 1087, 33, 2049ULL);
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(2049ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(3071ULL));

	if (verbose)
	{
		std::cout << "Stat 4:" << std::endl;
	}

	checkStatistics(rd, 134ULL, 1917ULL, 1085ULL, 33ULL, 2049ULL);
}

/// First Block: Started from 21, incompleted,
/// Second Block: Completed.
/// Third Block: Completed.
/// Other Blocks: Incompleted
/// Pull the window from end as much as twice the size of blocks in window.
TEST(ReplayDetector, ForwardWindow_FromEnd_14)
{
        ReplayDetector  rd(1000ULL, 21ULL);
	uint64_t       startPktNo(65ULL);

	checkStatistics(rd, 0ULL, 0ULL, 1068ULL, 1ULL, 21ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(21ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(22ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(23ULL));

	while (startPktNo <= 192ULL)
	{
                EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo++));
	}

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 131ULL, 0ULL, 937ULL, 1ULL, 21ULL);

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(3136));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 132ULL, 1897ULL, 1087ULL, 33ULL, 2049ULL);
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(2049ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(3071ULL));

	if (verbose)
	{
		std::cout << "Stat 4:" << std::endl;
	}

	checkStatistics(rd, 134ULL, 1897ULL, 1085ULL, 33ULL, 2049ULL);
}

/// First Block: Started from 129, incompleted,
/// Second Block: Completed.
/// Third Block: Completed.
/// Other Blocks: Incompleted
/// Pull the window from end as much as twice the size of blocks in window.
TEST(ReplayDetector, ForwardWindow_FromEnd_15)
{
        ReplayDetector  rd(1000, 129ULL);
	uint64_t       startPktNo(193ULL);

	checkStatistics(rd, 0ULL, 0ULL, 1088ULL, 3ULL, 129ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(131ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(132ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(133ULL));

	while (startPktNo <= 320ULL)
	{
                EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo++));
	}

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 131ULL, 0ULL, 957ULL, 3ULL, 129ULL);

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(3136ULL));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 132ULL, 1789ULL, 1087ULL, 33ULL, 2049ULL);
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(2049ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(3071ULL));

	if (verbose)
	{
		std::cout << "Stat 4:" << std::endl;
	}

	checkStatistics(rd, 134ULL, 1789ULL, 1085ULL, 33ULL, 2049ULL);
}

/// First Block: Started from 149, incompleted,
/// Second Block: Completed.
/// Third Block: Completed.
/// Other Blocks: Incompleted
/// Pull the window from end as much as twice the size of blocks in window.
TEST(ReplayDetector, ForwardWindow_FromEnd_16)
{
        ReplayDetector  rd(1000ULL, 149ULL);
	uint64_t       startPktNo(193ULL);

	checkStatistics(rd, 0ULL, 0ULL, 1068ULL, 3ULL, 149ULL);

	if (verbose)
	{
		std::cout << "Stat 1:" << std::endl;
	}

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(151ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(152ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(153ULL));

	while (startPktNo <= 320ULL)
	{
                EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(startPktNo++));
	}

	if (verbose)
	{
		std::cout << "Stat 2:" << std::endl;
	}

	checkStatistics(rd, 131ULL, 0ULL, 937ULL, 3ULL, 149ULL);

        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(3136));

	if (verbose)
	{
		std::cout << "Stat 3:" << std::endl;
	}

	checkStatistics(rd, 132ULL, 1769ULL, 1087ULL, 33ULL, 2049ULL);
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(2049ULL));
        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(3071ULL));

	if (verbose)
	{
		std::cout << "Stat 4:" << std::endl;
	}

	checkStatistics(rd, 134ULL, 1769ULL, 1085ULL, 33ULL, 2049ULL);
}

/// Generating random packet numbers in the half range of window.
/// Packets: 131K
TEST(ReplayDetector, RandomPacketNumber_17)
{
	uint64_t       windowSize(1024ULL);
	uint64_t       pktNumberStart(1ULL);
	uint64_t       pktNumberRange(512ULL);
	uint64_t       pktNumberMax(0x20000ULL);
        ReplayDetector  rd(windowSize, pktNumberStart);

	while (pktNumberStart < pktNumberMax)
	{
		std::srand(unsigned(std::time(0)));
		std::vector<uint64_t>  packetNumbers;

		for (uint64_t i = pktNumberStart; i < pktNumberStart + pktNumberRange; ++i)
		{
			packetNumbers.push_back(i);
		}

		std::random_shuffle(packetNumbers.begin(), packetNumbers.end());

		for (uint64_t pktNumber : packetNumbers)
		{
                        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(pktNumber));
		}

		pktNumberStart += pktNumberRange;
	}

	checkStatistics(rd, 0x20000ULL, 0ULL, 1088ULL, 0x801ULL, 0x20001ULL);
}
/// Generating random packet numbers in the half range of window.
/// Packets: 2M
TEST(ReplayDetector, RandomPacketNumber_18)
{
	uint64_t       windowSize(1024ULL);
	uint64_t       pktNumberStart(1ULL);
	uint64_t       pktNumberRange(512ULL);
	uint64_t       pktNumberMax(0x200000ULL);
        ReplayDetector  rd(windowSize, pktNumberStart);

	while (pktNumberStart < pktNumberMax)
	{
		std::srand(unsigned(std::time(0)));
		std::vector<uint64_t>  packetNumbers;

		for (uint64_t i = pktNumberStart; i < pktNumberStart + pktNumberRange; ++i)
		{
			packetNumbers.push_back(i);
		}

		std::random_shuffle(packetNumbers.begin(), packetNumbers.end());

		for (uint64_t pktNumber : packetNumbers)
		{
                        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(pktNumber));
		}

		pktNumberStart += pktNumberRange;
	}

	checkStatistics(rd, 0x200000ULL, 0ULL, 1088ULL, 0x8001ULL, 0x200001ULL);
}
/// Generating random packet numbers in the half range of window.
/// Packets: 33M
TEST(ReplayDetector, RandomPacketNumber_19)
{
	uint64_t       windowSize(1024ULL);
	uint64_t       pktNumberStart(1ULL);
	uint64_t       pktNumberRange(512ULL);
	uint64_t       pktNumberMax(0x2000000ULL);
        ReplayDetector  rd(windowSize, pktNumberStart);

	std::srand(unsigned(std::time(0)));
	std::vector<uint64_t>  packetNumbers;

	for (uint64_t i = pktNumberStart; i < pktNumberStart + pktNumberRange; ++i)
	{
		packetNumbers.push_back(i);
	}

	std::random_shuffle(packetNumbers.begin(), packetNumbers.end());

	while (pktNumberStart < pktNumberMax)
	{
		for (uint64_t pktNumber : packetNumbers)
		{
			// std::cout << "pktNumber:" << pktNumber << "  pktNumberStart:" << pktNumberStart <<
			// std::endl;
                        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(pktNumber + pktNumberStart - 1));
		}

		pktNumberStart += pktNumberRange;
	}

	checkStatistics(rd, 0x2000000ULL, 0ULL, 1088ULL, 0x80001ULL, 0x2000001ULL);
}
/// Generating random packet numbers in the half range of window.
/// Packets: 530M
TEST(ReplayDetector, RandomPacketNumber_20)
{
	uint64_t       windowSize(1024ULL);
	uint64_t       pktNumberStart(1ULL);
	uint64_t       pktNumberRange(512ULL);
	uint64_t       pktNumberMax(0x20000000ULL);
        ReplayDetector  rd(windowSize, pktNumberStart);

	std::srand(unsigned(std::time(0)));
	std::vector<uint64_t>  packetNumbers;

	for (uint64_t i = pktNumberStart; i < pktNumberStart + pktNumberRange; ++i)
	{
		packetNumbers.push_back(i);
	}

	std::random_shuffle(packetNumbers.begin(), packetNumbers.end());

	while (pktNumberStart < pktNumberMax)
	{
		for (uint64_t pktNumber : packetNumbers)
		{
                        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(pktNumber + pktNumberStart - 1));
		}

		pktNumberStart += pktNumberRange;
	}

	checkStatistics(rd, 0x20000000ULL, 0ULL, 1088ULL, 0x800001ULL, 0x20000001ULL);
}


/*
	 /// Generating random packet numbers in the half range of window.
	 /// Packets: 8.5G
         TEST(ReplayDetector, RandomPacketNumber_21)
	 {
	 uint64_t       windowSize(1024ULL);
	 uint64_t       pktNumberStart(1ULL);
	 uint64_t       pktNumberRange(512ULL);
	 uint64_t       pktNumberMax(0x200000000ULL);
         ReplayDetector  rd(windowSize, pktNumberStart);

	 std::srand(unsigned(std::time(0)));
	 std::vector<uint64_t>  packetNumbers;

	 for (uint64_t i = pktNumberStart; i < pktNumberStart + pktNumberRange; ++i)
	 {
		packetNumbers.push_back(i);
	 }

	 std::random_shuffle(packetNumbers.begin(), packetNumbers.end());

	 while (pktNumberStart < pktNumberMax)
	 {
		for (uint64_t pktNumber : packetNumbers)
		{
                        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(pktNumber + pktNumberStart - 1));
		}

		pktNumberStart += pktNumberRange;
	 }

	 checkStatistics(rd, 0x200000000ULL, 0ULL, 1088ULL, 0x8000001ULL, 0x200000001ULL);
	 }
 */


/// Generating random packet numbers in the range of window.
/// Packets: 131K
TEST(ReplayDetector, RandomPacketNumber_22)
{
	uint64_t       windowSize(1024ULL);
	uint64_t       pktNumberStart(1ULL);
	uint64_t       pktNumberRange(1024ULL);
	uint64_t       pktNumberMax(0x20000ULL);
        ReplayDetector  rd(windowSize, pktNumberStart);

	while (pktNumberStart < pktNumberMax)
	{
		std::srand(unsigned(std::time(0)));
		std::vector<uint64_t>  packetNumbers;

		for (uint64_t i = pktNumberStart; i < pktNumberStart + pktNumberRange; ++i)
		{
			packetNumbers.push_back(i);
		}

		std::random_shuffle(packetNumbers.begin(), packetNumbers.end());

		for (uint64_t pktNumber : packetNumbers)
		{
                        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(pktNumber));
		}

		pktNumberStart += pktNumberRange;
	}

	checkStatistics(rd, 0x20000ULL, 0ULL, 1088ULL, 0x801ULL, 0x20001ULL);
}

/// Generating random packet numbers in the range of window.
/// Packets: 33M
TEST(ReplayDetector, RandomPacketNumber_23)
{
	uint64_t       windowSize(1024ULL);
	uint64_t       pktNumberStart(1ULL);
	uint64_t       pktNumberRange(1024ULL);
	uint64_t       pktNumberMax(0x2000000ULL);
        ReplayDetector  rd(windowSize, pktNumberStart);

	while (pktNumberStart < pktNumberMax)
	{
		std::srand(unsigned(std::time(0)));
		std::vector<uint64_t>  packetNumbers;

		for (uint64_t i = pktNumberStart; i < pktNumberStart + pktNumberRange; ++i)
		{
			packetNumbers.push_back(i);
		}

		std::random_shuffle(packetNumbers.begin(), packetNumbers.end());

		for (uint64_t pktNumber : packetNumbers)
		{
                        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(pktNumber));
		}

		pktNumberStart += pktNumberRange;
	}

	checkStatistics(rd, 0x2000000ULL, 0ULL, 1088ULL, 0x80001ULL, 0x2000001ULL);
}

/// Generating random packet numbers in more than range of window.
/// Performance Test
/// Packets: 131K
/*TEST(ReplayDetector, RandomPacketNumber_24)
{
	uint64_t       windowSize(1024ULL);
	uint64_t       pktNumberStart(1ULL);
	uint64_t       pktNumberRange(2048ULL);
	uint64_t       pktNumberMax(0x20001ULL);
        ReplayDetector  rd(windowSize, pktNumberStart);

	while (pktNumberStart < pktNumberMax)
	{
		std::srand(unsigned(std::time(0)));
		std::vector<uint64_t>  packetNumbers;

		for (uint64_t i = pktNumberStart; i < pktNumberStart + pktNumberRange; ++i)
		{
			packetNumbers.push_back(i);
		}

		std::random_shuffle(packetNumbers.begin(), packetNumbers.end());

		for (uint64_t pktNumber : packetNumbers)
		{
                        EXPECT_NE(ReplayDetector::RT_Duplicate, rd.checkPacketNumber(pktNumber));
		}

		pktNumberStart += pktNumberRange;
	}

	checkStatistics(rd, 0x20000ULL, 0ULL, 1088ULL, 0x801ULL, 0x20001ULL);
}*/

/// Generating random packet numbers in the range of window.
/// Window location is about MAX_INT(2 power 32)
/// Packets: 34M
TEST(ReplayDetector, RandomPacketNumber_28)
{
	uint64_t       windowSize(1024ULL);
	uint64_t       pktNumberStart(0xFFF00001ULL);
	uint64_t       pktNumberRange(1024ULL);
	uint64_t       pktNumberMax(0x102000001ULL);
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
                        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(pktNumber));
		}

		pktNumberStart += pktNumberRange;
	}

	checkStatistics(rd, 0x2100000ULL, 0ULL, 1088ULL, 0x4080001ULL, 0x102000001ULL);
}

/// Generating random packet numbers in the range of window.
/// Window location is about MAX_INT(2 power 63)
/// Packets: 34M
TEST(ReplayDetector, RandomPacketNumber_29)
{
	uint64_t       windowSize(1024ULL);
	uint64_t       pktNumberStart(0x7FFFFFFFFFF00001ULL);
	uint64_t       pktNumberRange(1024ULL);
	uint64_t       pktNumberMax(0x8000000002000001ULL);
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
                        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(pktNumber));
		}

		pktNumberStart += pktNumberRange;
	}

	checkStatistics(rd, 0x2100000ULL, 0ULL, 1088ULL, 0x200000000080001ULL, 0x8000000002000001ULL);
}
/// Generating random packet numbers in the range of window.
/// Window location is about MAX_INT(2 power 64)
/// Packets: 65K
TEST(ReplayDetector, RandomPacketNumber_30)
{
	uint64_t       windowSize(1024ULL);
	uint64_t       pktNumberStart(0xFFFFFFFFFFF00001ULL);
	uint64_t       pktNumberRange(1024ULL);
	uint64_t       pktNumberMax(0xFFFFFFFFFFFFFFFFULL);
        ReplayDetector  rd(windowSize, pktNumberStart);

	while (pktNumberStart < pktNumberMax)
	{
		std::srand(unsigned(std::time(0)));
		std::vector<uint64_t>  packetNumbers;

		if (pktNumberStart + pktNumberRange < pktNumberStart)
		{
			pktNumberRange = 0xFFFFFFFFFFFFFFFF - pktNumberStart;
		}

		uint64_t  max(MIN(pktNumberStart + pktNumberRange, pktNumberMax));

		for (uint64_t i = pktNumberStart; i < max; ++i)
		{
			packetNumbers.push_back(i);
		}

		std::random_shuffle(packetNumbers.begin(), packetNumbers.end());

		for (uint64_t pktNumber : packetNumbers)
		{
			// std::cout << "pktNumber:" << pktNumber << std::endl;
                        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(pktNumber));
		}

		pktNumberStart += pktNumberRange;
	}

        EXPECT_EQ(ReplayDetector::RT_Save,     rd.checkPacketNumber(pktNumberMax));
        EXPECT_EQ(ReplayDetector::RT_Unknown,  rd.checkPacketNumber(pktNumberMax + 1));
        EXPECT_EQ(ReplayDetector::RT_Overflow, rd.checkPacketNumber(pktNumberMax + 2));
        EXPECT_EQ(ReplayDetector::RT_Overflow, rd.checkPacketNumber(pktNumberMax + 3));


	checkStatistics(rd, 0xFFFFFULL, 0ULL, 1025ULL, 0x400000000000000ULL, 0xFFFFFFFFFFFFFFC1ULL);
}

/// Overflow checking
TEST(ReplayDetector, RandomPacketNumber_31)
{
	uint64_t       windowSize(1024ULL);
	uint64_t       pktNumberStart1(0xFFFFFFFFFFF00001ULL);
	uint64_t       pktNumberStart2(0x00FFFFFFFFF00001ULL);
        ReplayDetector  rd1(windowSize, pktNumberStart1);
        ReplayDetector  rd2(windowSize, pktNumberStart2);

        EXPECT_EQ(ReplayDetector::RT_Unknown,  rd1.checkPacketNumber(1ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown,  rd1.checkPacketNumber(2ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown,  rd1.checkPacketNumber(3ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown,  rd1.checkPacketNumber(4ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown,  rd1.checkPacketNumber(5ULL));

        EXPECT_EQ(ReplayDetector::RT_Save,     rd1.checkPacketNumber(0xFFFFFFFFFFF00001ULL));
        EXPECT_EQ(ReplayDetector::RT_Overflow, rd1.checkPacketNumber(1ULL));
        EXPECT_EQ(ReplayDetector::RT_Overflow, rd1.checkPacketNumber(2ULL));
        EXPECT_EQ(ReplayDetector::RT_Overflow, rd1.checkPacketNumber(3ULL));
        EXPECT_EQ(ReplayDetector::RT_Overflow, rd1.checkPacketNumber(4ULL));
        EXPECT_EQ(ReplayDetector::RT_Overflow, rd1.checkPacketNumber(5ULL));

        EXPECT_EQ(ReplayDetector::RT_Save,     rd2.checkPacketNumber(0x00FFFFFFFFF00001ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown,  rd2.checkPacketNumber(1ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown,  rd2.checkPacketNumber(2ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown,  rd2.checkPacketNumber(3ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown,  rd2.checkPacketNumber(4ULL));
        EXPECT_EQ(ReplayDetector::RT_Unknown,  rd2.checkPacketNumber(5ULL));
}

/// Generating random packet numbers in the range of window.
/// Window size is 33M
/// Packets: 370M
TEST(ReplayDetector, RandomPacketNumber_32)
{
	uint64_t       windowSize(0x2000000ULL);
	uint64_t       pktNumberStart(0x1ULL);
	uint64_t       pktNumberRange(0x2000000ULL);
	uint64_t       pktNumberMax(0x16000001ULL);
        ReplayDetector  rd(windowSize, pktNumberStart);

	std::srand(unsigned(std::time(0)));
	std::vector<uint64_t>  packetNumbers;

	for (uint64_t i = 1; i <= pktNumberRange; ++i)
	{
		packetNumbers.push_back(i);
	}

	std::cout << "Packet pushed" << std::endl;

	std::random_shuffle(packetNumbers.begin(), packetNumbers.end());
	std::cout << "Packet shuffled" << std::endl;

	while (pktNumberStart < pktNumberMax)
	{
		for (uint64_t pktNumber : packetNumbers)
		{
			// std::cout << "pktNumber:" << pktNumber << std::endl;
                        EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(pktNumber + pktNumberStart - 1));
		}

		pktNumberStart += pktNumberRange;
		// std::cout << "pktNumberStart:" << pktNumberStart << "/" << pktNumberMax << std::endl;
	}

	checkStatistics(rd, 0x16000000ULL, 0ULL, 0x2000000ULL + 64, 0x580001ULL, 0x16000001ULL);
}

/// Generating random packet numbers in the range of window.
/// Window size is 33M
/// Perfomance Test
/// Packets: 370M
TEST(ReplayDetector, RandomPacketNumber_33)
{
	uint64_t       windowSize(0x2000000ULL);
	uint64_t       pktNumberStart(0x1ULL);
	uint64_t       pktNumberRange(0x2000000ULL);
	uint64_t       pktNumberMax(0x16000001ULL);
        ReplayDetector  rd(windowSize, pktNumberStart);


	for (uint64_t pktNumber = pktNumberRange; pktNumber > 1ULL; --pktNumber)
	{
                EXPECT_EQ(ReplayDetector::RT_Save, rd.checkPacketNumber(pktNumber));
	}

	pktNumberStart += pktNumberRange;

	EXPECT_EQ(rd.duplicatedPacketCount(), 0);

// std::cout << "Packet pushed" << std::endl;
	uint64_t  duplicated(0);

	while (pktNumberStart < pktNumberMax)
	{
                EXPECT_EQ(ReplayDetector::RT_Duplicate, rd.checkPacketNumber(2));
		++pktNumberStart;
		++duplicated;
	}

	EXPECT_EQ(rd.duplicatedPacketCount(), duplicated);
	checkStatistics(rd, 0x2000000ULL - 1, 0ULL, 65ULL, 0x1ULL, 0x1ULL);
}
int  main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);

	if ((argc >= 2) && !strcasecmp(argv[1], "-v"))
	{
		verbose = true;
	}

	return RUN_ALL_TESTS();
}
