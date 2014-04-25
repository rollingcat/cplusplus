
#include "Vector.h"
#include "gtest/gtest.h"
#include <random>
#include <vector>

class VectorTest : public testing::Test
{
protected:
    my::Vector<int> m_myVector;
    std::vector<int> m_stdVector;
};

TEST_F(VectorTest, DefaultConstructor)
{
    EXPECT_EQ(0, m_myVector.size());
    EXPECT_EQ(0, m_myVector.capacity());
    EXPECT_EQ(nullptr, m_myVector.data());
    EXPECT_EQ(nullptr, m_myVector.begin());
    EXPECT_EQ(nullptr, m_myVector.end());
}

TEST_F(VectorTest, ResizeSimple)
{
    m_myVector.resize(5);
    m_stdVector.resize(5);
    EXPECT_EQ(m_stdVector.size(), m_myVector.size());

    for (size_t i = 0; i < m_myVector.size(); ++i)
        EXPECT_EQ(m_stdVector[i], m_myVector[i]) << "at index " << i;
}

TEST_F(VectorTest, ResizeDown)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;
    auto random = std::bind(distribution, generator);

    for (size_t i = 0; i < 20; ++i) {
        int num = random();
        m_myVector.push_back(num);
        m_stdVector.push_back(num);
    }

    for (size_t i = 0; i < m_myVector.size(); ++i)
        EXPECT_EQ(m_stdVector[i], m_myVector[i]) << "at index " << i;

    m_myVector.resize(10);
    m_stdVector.resize(10);
    EXPECT_EQ(m_stdVector.size(), m_myVector.size());

    for (size_t i = 0; i < m_myVector.size(); ++i)
        EXPECT_EQ(m_stdVector[i], m_myVector[i]) << "at index " << i;
}

TEST_F(VectorTest, ResizeUp)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;
    auto random = std::bind(distribution, generator);

    for (size_t i = 0; i < 10; ++i) {
        int num = random();
        m_myVector.push_back(num);
        m_stdVector.push_back(num);
    }

    for (size_t i = 0; i < m_myVector.size(); ++i)
        EXPECT_EQ(m_stdVector[i], m_myVector[i]) << "at index " << i;

    m_myVector.resize(20);
    m_stdVector.resize(20);
    EXPECT_EQ(m_stdVector.size(), m_myVector.size());

    for (size_t i = 0; i < m_myVector.size(); ++i)
        EXPECT_EQ(m_stdVector[i], m_myVector[i]) << "at index " << i;
}

TEST_F(VectorTest, ReserveCapacity)
{
    m_myVector.reserve(16);
    m_stdVector.reserve(16);

    EXPECT_EQ(m_stdVector.size(), m_myVector.size());
    EXPECT_EQ(m_stdVector.capacity(), m_myVector.capacity());
}
