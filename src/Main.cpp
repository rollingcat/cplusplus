
#define USE(FEATURE) (defined USE_##FEATURE && USE_##FEATURE)

#define USE_MY_VECTOR 1

#include <iostream>
using namespace std;

#if USE(MY_VECTOR)
#include "Vector.h"
using namespace my;
#else
#include <vector>
template<class T>
using Vector = vector<T>;
#endif

int main()
{
    cout << "Hello Vector<int>" << endl;
    Vector<int> vecInt;

    vecInt.push_back(10);
    vecInt.push_back(12);
    vecInt.push_back(14);

    for (size_t i = 0; i < vecInt.size(); ++i)
        cout << i << " : " << vecInt[i] << endl;

	return 0;
}
