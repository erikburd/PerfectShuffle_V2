#pragma once
#include <random>
#include <chrono>

class RandomGenerator
{
public:
    static RandomGenerator& Instance() 
    {
        static RandomGenerator s;
        return s;
    }
    std::mt19937_64 get();

private:
    RandomGenerator();
    ~RandomGenerator() {}

    RandomGenerator(RandomGenerator const&) = delete;
    RandomGenerator& operator= (RandomGenerator const&) = delete;

    std::mt19937_64 mt;
};

RandomGenerator::RandomGenerator() {
    std::random_device rd;

    if (rd.entropy() != 0) {
        mt.seed(rd());
    }
    else {
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        mt.seed(seed);
    }
}

std::mt19937_64 RandomGenerator::get() {
    return mt;
}

