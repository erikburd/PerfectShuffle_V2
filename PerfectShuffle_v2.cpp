// PerfectShuffle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <assert.h>
#include "PerfectShuffle_v2.h"
#include "Timer.h"

using namespace std;

void TestPerfectShuffle()
{
	CardShuffler<int> cs, cs1;
	CardShuffler<int> cs2, cs3;
	int cycles, cycles2;

	// test for decks up to 500 cards
	// verify that the "new" shuffles generate the
	// same shuffle at each iteration as the original
	// algorithm, and also verify that it takes the
	// same number of cycles to restore each deck
	for (int j = 3; j <= 500; j++)
	{
		cout << "Deck size: " << j << "\n";
		cs.GenerateDeck(j);
		cs1.GenerateDeck(j);
		cycles = cs.RestoreDeck(ShuffleType::INV_INSHUFFLE);
//		cout << "\tCycles: " << cycles << " Inverse in\n";
		for (int i = 0; i < cycles; i++)
		{
			cs.PerformShuffle(ShuffleType::INV_INSHUFFLE);
			cs1.PerformShuffle(ShuffleType::ORIG_INV_INSHUFFLE);
			assert(cs.GetDeck() == cs1.GetDeck());
		}

		cs2.GenerateDeck(j);
		cs3.GenerateDeck(j);
		cycles2 = cs2.RestoreDeck(ShuffleType::INSHUFFLE);
//		cout << "\tCycles: " << cycles << " In\n";
		for (int i = 0; i < cycles2; i++)
		{
			cs2.PerformShuffle(ShuffleType::INSHUFFLE);
			cs3.PerformShuffle(ShuffleType::ORIG_INSHUFFLE);
			assert(cs2.GetDeck() == cs3.GetDeck());
		}

		cs.GenerateDeck(j);
		cs1.GenerateDeck(j);
		cycles = cs.RestoreDeck(ShuffleType::INV_OUTSHUFFLE);
		//cout << "\tCycles: " << cycles << " Inverse out\n";
		for (int i = 0; i < cycles; i++)
		{
			cs.PerformShuffle(ShuffleType::INV_OUTSHUFFLE);
			cs1.PerformShuffle(ShuffleType::ORIG_INV_OUTSHUFFLE);
			assert(cs.GetDeck() == cs1.GetDeck());
		}

		cs2.GenerateDeck(j);
		cs3.GenerateDeck(j);
		cycles2 = cs2.RestoreDeck(ShuffleType::OUTSHUFFLE);
		//cout << "\tCycles: " << cycles << " Out\n";
		for (int i = 0; i < cycles2; i++)
		{
			cs2.PerformShuffle(ShuffleType::OUTSHUFFLE);
			cs3.PerformShuffle(ShuffleType::ORIG_OUTSHUFFLE);
			assert(cs2.GetDeck() == cs3.GetDeck());
		}
	}
}


// code from RosettaCode
// buggy - only works with even numbered decks
int pShuffle(int t)
{
	std::vector<int> v, o, r;

	for (int x = 0; x < t; x++) {
		o.push_back(x + 1);
	}

	r = o;
	int t2 = t / 2 - 1, c = 1;

	while (true) {
		v = r;
		r.clear();

		for (int x = t2; x > -1; x--) {
			r.push_back(v[x + t2 + 1]);
			r.push_back(v[x]);
		}

		std::reverse(r.begin(), r.end());

		if (std::equal(o.begin(), o.end(), r.begin())) return c;
		c++;
	}
}

void Test()
{
	/*	input(deck size) 	output(number of shuffles required)
			8 	3
			24 	11
			52 	8
			100 	30
			1020 	1018
			1024 	10
			10000 	300
	*/
	CardShuffler<int> cs;

	int decks[] = { 8, 24, 52, 100, 1020, 1024, 10000 };
	int expOutShuffles[] = { 3, 11, 8, 30, 1018, 10, 300 };
	for (int i = 0; i < sizeof(decks) / sizeof(decks[0]); i++)
	{
		cout << "Generating deck of " << decks[i] << " cards\n";
		cout << "Expected #/OutShuffles: " << expOutShuffles[i] << "\n";
		cs.GenerateDeck(decks[i]);
		int n = cs.RestoreDeck(ShuffleType::OUTSHUFFLE);
		assert(n == expOutShuffles[i]);
		cout << "Number of OutShuffles: " << n << "\n\n";
	}
}

void SpeedTest()
{
	CardShuffler<int> cs;
	Timer t1, t2;

	double d1 = 0, d2 = 0;

	// the "pShuffle" function from RosetteCode only works on even number decks
	for (int i = 4; i <= 10000; i += 2)
	{
		t1.Start();
		cs.GenerateDeck(i);
		int n = cs.RestoreDeck(ShuffleType::OUTSHUFFLE);
		t1.Stop();
		d1 += t1.GetElaspedTime();

		t2.Start();
		int j = pShuffle(i);
		t2.Stop();
		d2 += t2.GetElaspedTime();

		cout << "Cards: " << i << "\nMethod A time: " << d1 << "\nMethod B time: " << d2 << "\n\n";

	}
}

int main()
{
	CardShuffler<int> c;
	c.GenerateDeck(4);
	c.PerformShuffle(ShuffleType::INSHUFFLE);
	SpeedTest();

	return 0;

	// perform timing for old method and new one
	CardShuffler<int> cs1, cs2;
	Timer t1, t2;
	unsigned int nCount = 0;

	for (int i = 3; i <= 100; i++)
	{
		cs1.GenerateDeck(i);
		cs2.GenerateDeck(i);
		t1.Start();
		//nCount = cs1.RestoreDeck(ShuffleType::ORIG_INSHUFFLE);
		//cs1.PerformShuffle(ShuffleType::FISHER_YATES);
		t1.Stop();
		//t2.Start();
		//cs2.RestoreDeck(ShuffleType::ORIG_INV_INSHUFFLE);
		//t2.Stop();
		//cout << "Deck: " << i << "\n";
		//cout << "\t    In: " << t1.GetElaspedTime() << "\n";
		//cout << "\tInv In: " << t2.GetElaspedTime() << "\n";
		//cout << "\t Count: " << nCount << "\n";
	}

	// test function for verifying perfect shuffle algorithm functionality
	TestPerfectShuffle();
}
