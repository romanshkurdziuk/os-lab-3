#include <iostream>
#include <vector>
#include <windows.h>

using namespace std;

CRITICAL_SECTION cs;

struct MarkerThreedParams
{
    int id;
    vector<int> *arr;
    HANDLE hStartEvent;
};

DWORD WINAPI marker_thread(LPVOID params)
{
    MarkerThreedParams *threadParams = (MarkerThreedParams*)params;
    int id = threadParams->id;
    vector<int>& arr = *(threadParams->arr);
    HANDLE hStartEvent = threadParams->hStartEvent;
    EnterCriticalSection(&cs);
    cout << "Marker thread # " << id << " It has been created and is waiting for a signal to start working." << endl;
    LeaveCriticalSection(&cs);
    WaitForSingleObject(hStartEvent, INFINITE);
    delete threadParams;
    return 0;
}

int main()
{   

    cout << "Enter the size of the array" << endl;
    int arraySize;
    cin >> arraySize;
    vector<int> arr(arraySize, 0);
    cout << "Enter the number of marker threads" << endl;
    int markersCount;
    cin >> markersCount;

    InitializeCriticalSection(&cs);
    vector<HANDLE> hMarkers(markersCount);
    HANDLE hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (hStartEvent == NULL) 
    {
        cerr << "Failed to create a start event." << endl;
        return 1;
    }
    for (int i = 0; i < markersCount; ++i)
    {
        MarkerThreedParams *params = new MarkerThreedParams();
        params->id = i;
        params->arr = &arr;
        params->hStartEvent = hStartEvent;
        hMarkers[i] = CreateThread(NULL, 0, marker_thread, params, 0, NULL);
        if (hMarkers[i] == NULL)
        {
            cerr << "Failed to create a thread # " << i << endl;
            return 1;
        }
    } 
    cout << "ALL threads created. Please press Enter to get started." << endl;
    cin.get();
    cin.get();
    cout << "\n--- START ---" << endl;
    SetEvent(hStartEvent);
    

    WaitForMultipleObjects(markersCount, hMarkers.data(), TRUE, INFINITE);
    cout << "\n--- THE WORK IS COMPLETED ---" << endl;

    CloseHandle(hStartEvent);
    for (int i = 0; i < markersCount; ++i) 
    {
        CloseHandle(hMarkers[i]);
    }
    DeleteCriticalSection(&cs);
    system("pause");
    return 0;
}
