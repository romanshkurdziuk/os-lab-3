#include "Utils/ArrayUtils.h"
#include <sstream>
#include <iostream>
namespace Utils
{
    string formatArrayToString(const vector<int> &arr)
    {
        stringstream ss;
        ss << "[ ";
        for (size_t i = 0; i < arr.size(); ++i)
        {
            ss << arr[i] << " ";
        }
        ss << "]";
        return ss.str();
    }

    void printArray(const vector<int> &arr, CRITICAL_SECTION& cs)
    {
        EnterCriticalSection(&cs);
        cout << formatArrayToString(arr) << endl;
        LeaveCriticalSection(&cs);
    }
}