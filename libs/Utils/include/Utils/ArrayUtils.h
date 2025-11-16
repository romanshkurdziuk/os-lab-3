#include <vector>
#include <string>
#include <windows.h>

using namespace std;

namespace Utils 
{
    string formatArrayToString(const vector<int> &arr);
    void printArray(const vector<int> &arr, CRITICAL_SECTION& cs);
}