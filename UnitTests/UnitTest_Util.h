
#include <limits>
#include <random>

static std::default_random_engine g_randomEngine;

template<bool isIntegral, typename T>
struct Random;

template<typename T>
struct Random<true, T>
{
    static T value() {return distribution(g_randomEngine); }
    static std::uniform_int_distribution<T> distribution;
};

template <typename T>
std::uniform_int_distribution<T> Random<true, T>::distribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());

template<typename T>
struct Random<false, T>
{
    static T value() { return distribution(g_randomEngine); }
    static std::uniform_real_distribution<T> distribution;
};

template <typename T>
std::uniform_real_distribution<T> Random<false, T>::distribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());

template<typename T>
struct TestUtil
{
    static T random()
    {
        return Random<std::is_integral<T>::value, T>::value();
    }
};
