#ifndef TESTREPLAYDETECTOR_H
#define TESTREPLAYDETECTOR_H

#include <bits/stdc++.h>
#include <unordered_map>
#include "replay_detector.h"

#ifndef Q_UNUSED
#define Q_UNUSED(x) (void)x;
#endif
///
/// \brief The TestReplayDetector class contains functions by which you can test the
/// `ReplayDetector`
/// class.
///
class TestReplayDetector
{
public:
    TestReplayDetector();

	///
	/// \brief TestCase1 The aim of this function is detecting the `OutOfWindow` packets.
	/// \return true is returned whether the test is passed. Otherwise, false is returned.
	///
	bool  TestCase1();

	///
	/// \brief TestCase2 The aim of this function is checking a large amount of ordered packets.
	/// \return true is returned whether the test is passed. Otherwise, false is returned.
	///
	bool  TestCase2();

	///
	/// \brief TestCase3 The aim of this function is detecting the duplicated packets.
	/// \return true is returned whether the test is passed. Otherwise, false is returned.
	///
	bool  TestCase3();

	///
	/// \brief TestCase4 The aim of this function is checking a large amount of disordered packets.
	/// \return true is returned whether the test is passed. Otherwise, false is returned.
	///
	bool  TestCase4();

	bool  TestCase5();

	bool  TestCase6();

private:
	std::unordered_map<ReplayDetector::ResultTypes, const char *>  _resultsInterpreter;
};

#endif // TESTREPLAYDETECTOR_H
