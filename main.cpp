#include <iostream>
#include <vector>
#include <windows.h>

using namespace std;

int main()
{   
    cout << "Enter the size of the array" << endl;
    int size_of_initial_array;
    cin >> size_of_initial_array;
    vector<int> initial_array(size_of_initial_array, 0);
    cout << "Enter the number of marker threads" << endl;
    int number_of_marker_threads;
    cin >> number_of_marker_threads;
    return 0;
}
