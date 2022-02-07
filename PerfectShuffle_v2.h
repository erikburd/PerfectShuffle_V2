#pragma once
#include <vector>
#include <algorithm>	// for std::shuffle
#include <chrono>		// for system clock 
#include <random>		// for default_random_engine

using namespace std;

enum class ShuffleType
{
	STL_SHUFFLE,
	FISHER_YATES,
	OUTSHUFFLE,
	INSHUFFLE,
	INV_OUTSHUFFLE,
	INV_INSHUFFLE,
	// original implementation, used for testing
	ORIG_OUTSHUFFLE,
	ORIG_INSHUFFLE,
	ORIG_INV_OUTSHUFFLE,
	ORIG_INV_INSHUFFLE
};

template <class T>
class CardShuffler
{
public:
	CardShuffler();
	~CardShuffler();

	// public member functions
	std::vector<T> GenerateDeck(size_t size);
	std::vector<T> GetDeck() { return m_deck; }
	void ResetDeck();
	void PerformShuffle(ShuffleType shuffle);
	unsigned int RestoreDeck(ShuffleType shuffle);
	bool IsDeckRestored();

private:
	// deck of cards
	std::vector<T> m_deck;

	// this is only used for the original implementation
	std::vector<T> m_DeckCopy;

	// Mersenne Twister algorithm for uniform random number generator
	std::mt19937_64 m_urng;

	// copy every nth item from a src vector into two destination vectors creating two halves
	typename void copy_every_n(typename std::vector<T>::iterator srcBegin, typename std::vector<T>::iterator srcEnd, 
							   typename std::vector<T>::iterator destVector1, typename std::vector<T>::iterator destVector2, size_t n);
};

template<class T>
CardShuffler<T>::CardShuffler()
{
	// init the Mersenne Twister random number generator
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	m_urng = std::mt19937_64(seed);
}

template<class T>
CardShuffler<T>::~CardShuffler()
{
}

template<class T>
void CardShuffler<T>::ResetDeck()
{
	// get the current size of the deck
	size_t nSize = m_deck.size();

	// generate a new card deck with the original deck size	
	GenerateDeck(nSize);
}

template<class T>
bool CardShuffler<T>::IsDeckRestored()
{
	// this assumes we're only using numbers instead of
	// a "real deck" with suits and A, J, Q, K cards
	return std::is_sorted(m_deck.begin(), m_deck.end());
}

template<class T>
std::vector<T> CardShuffler<T>::GenerateDeck(size_t size)
{

	// swap using Scott Meyer's trick to clear out the 
	// existing vector with an empty vector
	std::vector<T>().swap(m_deck);

	// using reserve and push_back is slightly faster
	// https://lemire.me/blog/2012/06/20/do-not-waste-time-with-stl-vectors/
	m_deck = std::vector<T>();
	m_deck.reserve(size);
	for (unsigned int i = 0; i < size; i++)
		m_deck.push_back(i);

	return m_deck;
}

// returns number of shuffles to restore a deck with a given shuffle type
template<class T>
unsigned int CardShuffler<T>::RestoreDeck(ShuffleType shuffle)
{
	unsigned int nShuffles = 0;

	// run this loop until the deck has been restored
	do
	{
		nShuffles++;
		PerformShuffle(shuffle);
	} while (IsDeckRestored() == false);

	// return number of shuffles to restore a deck
	return nShuffles;
}

// copy every nth element from a vector into two destination vectors, modified from this link
//https://stackoverflow.com/questions/30817563/copy-every-other-element-using-standard-algorithms-downsampling

template<class T>
typename void CardShuffler<T>::copy_every_n(typename std::vector<T>::iterator srcBegin, typename std::vector<T>::iterator srcEnd,
											typename std::vector<T>::iterator destVector1, typename std::vector<T>::iterator destVector2, size_t n)
{
	// increment by the value n specified
	const size_t nIncrement = n;

	// copy every nth item in the vector into the two destination vectors
	// indeces 0, 2, 4, ... go into destination vector 1
	// indeces 1, 3, 5, ... go into destination vector 2
	for (size_t i = distance(srcBegin, srcEnd) / nIncrement; i--; advance(srcBegin, nIncrement))
	{
		// copy the elements into the destination vectors
		*destVector1++ = *srcBegin;
		*destVector2++ = *(srcBegin + 1);
	}
}

template<class T>
void CardShuffler<T>::PerformShuffle(ShuffleType shuffleType)
{
	size_t uDeckSize = m_deck.size();
	if (uDeckSize < 2)
		return;

	// flag to indicate if the deck contains an odd or even number of items
	bool bIsDeckOdd = (uDeckSize % 2) == 1;
	unsigned int nIndex = 0;

	switch (shuffleType)
	{
		case ShuffleType::STL_SHUFFLE:
		{
			// use the standard STL shuffle the deck using the random number generator
			std::shuffle(m_deck.begin(), m_deck.end(), m_urng);
			break;
		}

		case ShuffleType::FISHER_YATES:
		{
			// standard Fisher-Yates shuffle algorithm
			for (size_t i = (uDeckSize - 1); i > 0; --i)
			{
				std::uniform_int_distribution<size_t> dist(0, i);
				const size_t rndIndex = dist(m_urng);
				std::swap(m_deck[i], m_deck[rndIndex]);
			}
			break;
		}

		// For the inverse outshuffle and inverse inshuffle
		case ShuffleType::INV_INSHUFFLE:
		case ShuffleType::INV_OUTSHUFFLE:
		{
			// calculate the two halves of the deck, and assign them accordingly
			// based on odd/even, which will change the number of cards to use for each half
			size_t half1, half2;
			if (bIsDeckOdd)
				half1 = (uDeckSize / 2);
			else
				half1 = ((uDeckSize + 1) / 2);

			// calc second half size, then take the minimum of 
			// the two numbers for interleaving cards
			half2 = (uDeckSize - half1);
			size_t minSize = min(half1, half2);

			// vectors for storing the two halves of the deck
			std::vector<int> vecFirstHalf(minSize);
			std::vector<int> vecSecondHalf(minSize);

			/*

			basic algorithm for inverse in and inverse out shuffles and interleaving the cards
			1) if the size of the deck (n) is even (inverse in OR inverse out shuffle)
			copy every other 2nd item in deck into 2 vectors A and B, starting at index = 0 in the deck
			A = indeces 0, 2, 4, 6, ...
			B = indeces 1, 3, 5, 7, ...

			2) if the size of the deck (n) is odd, AND inverse out shuffle,
			then copy every other 2nd item in deck into 2 vectors A and B, starting at index = 1 in the dexk
			A = indeces 1, 3, 5, 7, ...
			B = indeces 2, 4, 6, 8, ...

			3) if the size of the deck (n) is odd, AND inverse in shuffle,
			then copy every other 2nd item in deck into 2 vectors A and B, but stop at index = n-1 in the deck
			A = indeces 1, 3, 5, 7, ...
			B = indeces 2, 4, 6, 8, ...

			4) Copy vectors A and B into the deck and interleave them as follows:
				Inverse In, n odd  = BAy, where 'y' is index = n-1 in the deck (index at n-1 remains unchanged)
				Inverse In, n even = BA
			    Inverse Out, n odd  = xBA, where 'x' is index = 0 in the deck (index at 0 remains unchanged)
			    Inverse Out, n even = AB
			*/

			if (shuffleType == ShuffleType::INV_INSHUFFLE)
			{
				if (bIsDeckOdd)
				{
					// inverse in, n = odd
					// copy starting at index = 0, end at index = n - 1
					// last card is unchanged
					copy_every_n(m_deck.begin(), m_deck.end() - 1, vecFirstHalf.begin(), vecSecondHalf.begin(), 2);
					copy(vecSecondHalf.begin(), vecSecondHalf.end(), m_deck.begin());
					copy(vecFirstHalf.begin(), vecFirstHalf.end(), m_deck.begin() + half1);
				}
				else
				{
					// inverse in, n = even
					copy_every_n(m_deck.begin(), m_deck.end(), vecFirstHalf.begin(), vecSecondHalf.begin(), 2);
					copy(vecSecondHalf.begin(), vecSecondHalf.end(), m_deck.begin());
					copy(vecFirstHalf.begin(), vecFirstHalf.end(), m_deck.begin() + half1);
				}
			}

			if (shuffleType == ShuffleType::INV_OUTSHUFFLE)
			{
				if (bIsDeckOdd)
				{
					// inverse out, n = odd
					// copy starting from index = 1
					// first card is unchanged
					copy_every_n(m_deck.begin() + 1, m_deck.end(), vecFirstHalf.begin(), vecSecondHalf.begin(), 2);
					copy(vecSecondHalf.begin(), vecSecondHalf.end(), m_deck.begin() + 1);
					copy(vecFirstHalf.begin(), vecFirstHalf.end(), m_deck.begin() + (half1 + 1));
				}
				else
				{
					// inverse out, n = even
					copy_every_n(m_deck.begin(), m_deck.end(), vecFirstHalf.begin(), vecSecondHalf.begin(), 2);
					copy(vecFirstHalf.begin(), vecFirstHalf.end(), m_deck.begin());
					copy(vecSecondHalf.begin(), vecSecondHalf.end(), m_deck.begin() + half1);
				}
			}

			break;
		}

		// This is the improved version of the code to make shuffling faster by eliminating the need for copying the vector of cards at each iteration.
		case ShuffleType::OUTSHUFFLE:
		case ShuffleType::INSHUFFLE:
		{
			// calculate the two halves of the deck, and assign them accordingly based on odd/even, and type of shuffle which will change the
			// number of cards to use for each half
			size_t half1, half2;
			if (shuffleType == ShuffleType::INSHUFFLE)
				half1 = (uDeckSize / 2);
			else
				half1 = ((uDeckSize + 1) / 2);

			// calc second half size, then take the minimum of the two numbers for interleaving cards
			half2 = (uDeckSize - half1);
			size_t minSize = min(half1, half2);

			// get the two halves of the deck
			std::vector<T> vecFirstHalf(m_deck.begin(), m_deck.begin() + half1);
			std::vector<T> vecSecondHalf(m_deck.begin() + half1, m_deck.end());

			// iterate through both halves, interleaving them
			for (size_t nIndex = 0, i = 0; i < minSize; i++, nIndex += 2)
			{
				if (shuffleType == ShuffleType::INSHUFFLE)
				{
					m_deck[nIndex] = vecSecondHalf[i];
					m_deck[nIndex + 1] = vecFirstHalf[i];
				}

				if (shuffleType == ShuffleType::OUTSHUFFLE)
				{
					m_deck[nIndex] = vecFirstHalf[i];
					m_deck[nIndex + 1] = vecSecondHalf[i];
				}
			}

			// in the case of an odd deck, assign the last card
			// for an out shuffle, it's the last card of the first half
			// for an in shuffle, it's the last card of the second half
			if (bIsDeckOdd)
			{
				if (shuffleType == ShuffleType::INSHUFFLE)
					m_deck[uDeckSize - 1] = vecSecondHalf[half2 - 1];
				else
					m_deck[uDeckSize - 1] = vecFirstHalf[half1 - 1];
			}

			break;
		}

		// This is the original version of the code, which required
		// making a copy of the desk on each iteration. This makes
		// the overall operations much slower but it's the baseline.

		// for Faro shuffles (in/out, inverse in/out)
		case ShuffleType::ORIG_INV_INSHUFFLE:
		case ShuffleType::ORIG_INV_OUTSHUFFLE:
		case ShuffleType::ORIG_OUTSHUFFLE:
		case ShuffleType::ORIG_INSHUFFLE:
		{
			// make a copy of the deck
			m_DeckCopy = m_deck;

			size_t nIndex = 0;

			for (size_t i = 0; i < uDeckSize; i++)
			{
				if ((shuffleType == ShuffleType::ORIG_OUTSHUFFLE) || (shuffleType == ShuffleType::ORIG_INV_OUTSHUFFLE))
				{
					if (bIsDeckOdd)
						// O(p) = 2p mod n, n odd
						nIndex = ((2 * i) % uDeckSize);
					else
						// O(p) = 2p mod n-1, n even
						nIndex = ((2 * i) % (uDeckSize - 1));

					// No change in position
					if (nIndex == 0)
						nIndex = i;
				}

				if ((shuffleType == ShuffleType::ORIG_INSHUFFLE) || (shuffleType == ShuffleType::ORIG_INV_INSHUFFLE))
				{
					if (bIsDeckOdd)
						// I(p) = (2p + 1) mod n, n odd
						nIndex = ((2 * i + 1) % uDeckSize);
					else
						// I(p) = (2p + 1) mod n+1, n even
						nIndex = ((2 * i + 1) % (uDeckSize + 1));
				}

				// swap card positions
				if (nIndex != i)
				{
					if ((shuffleType == ShuffleType::ORIG_INV_INSHUFFLE) || (shuffleType == ShuffleType::ORIG_INV_OUTSHUFFLE))
						std::swap(m_deck[i], m_DeckCopy[nIndex]);
					else
						std::swap(m_deck[nIndex], m_DeckCopy[i]);
				}
			}

			break;
		}
	} //switch
}
