
#include "UnitTest_Util.h"
#include "Vector.h"
#include "gtest/gtest.h"
#include <vector>

template<typename T>
class VectorTest : public testing::Test
{
protected:
    void pushBackRandomly(size_t num)
    {
        for (size_t i = 0; i < num; ++i) {
            T num = TestUtil<T>::random();
            this->m_myVector.push_back(num);
            this->m_stdVector.push_back(num);
        }
    }

    my::Vector<T> m_myVector;
    std::vector<T> m_stdVector;
};

using testing::Types;
typedef Types<int, unsigned, float, double> TypesForTest;
TYPED_TEST_CASE(VectorTest, TypesForTest);

TYPED_TEST(VectorTest, DefaultConstructor)
{
    EXPECT_EQ(0, this->m_myVector.size());
    EXPECT_EQ(0, this->m_myVector.capacity());
    EXPECT_EQ(nullptr, this->m_myVector.data());
    EXPECT_EQ(nullptr, this->m_myVector.begin());
    EXPECT_EQ(nullptr, this->m_myVector.end());
}

TYPED_TEST(VectorTest, ResizeSimple)
{
    this->m_myVector.resize(5);
    this->m_stdVector.resize(5);
    EXPECT_EQ(this->m_stdVector.size(), this->m_myVector.size());

    for (size_t i = 0; i < this->m_myVector.size(); ++i)
        EXPECT_EQ(this->m_stdVector[i], this->m_myVector[i]) << "at index " << i;
}

TYPED_TEST(VectorTest, ResizeDown)
{
    this->pushBackRandomly(20);

    for (size_t i = 0; i < this->m_myVector.size(); ++i)
        EXPECT_EQ(this->m_stdVector[i], this->m_myVector[i]) << "at index " << i;

    this->m_myVector.resize(10);
    this->m_stdVector.resize(10);
    EXPECT_EQ(this->m_stdVector.size(), this->m_myVector.size());

    for (size_t i = 0; i < this->m_myVector.size(); ++i)
        EXPECT_EQ(this->m_stdVector[i], this->m_myVector[i]) << "at index " << i;
}

TYPED_TEST(VectorTest, ResizeUp)
{
    this->pushBackRandomly(10);

    for (size_t i = 0; i < this->m_myVector.size(); ++i)
        EXPECT_EQ(this->m_stdVector[i], this->m_myVector[i]) << "at index " << i;

    this->m_myVector.resize(20);
    this->m_stdVector.resize(20);
    EXPECT_EQ(this->m_stdVector.size(), this->m_myVector.size());

    for (size_t i = 0; i < this->m_myVector.size(); ++i)
        EXPECT_EQ(this->m_stdVector[i], this->m_myVector[i]) << "at index " << i;
}

TYPED_TEST(VectorTest, ReserveCapacity)
{
    this->m_myVector.reserve(16);
    this->m_stdVector.reserve(16);

    EXPECT_EQ(this->m_stdVector.size(), this->m_myVector.size());
    EXPECT_EQ(this->m_stdVector.capacity(), this->m_myVector.capacity());

    this->pushBackRandomly(8);

    this->m_myVector.reserve(30);
    this->m_stdVector.reserve(30);

    EXPECT_EQ(this->m_stdVector.size(), this->m_myVector.size());
    EXPECT_EQ(this->m_stdVector.capacity(), this->m_myVector.capacity());

    for (size_t i = 0; i < this->m_myVector.size(); ++i)
        EXPECT_EQ(this->m_stdVector[i], this->m_myVector[i]) << "at index " << i;
}
