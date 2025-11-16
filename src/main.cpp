#include "Utils/ArrayUtils.h"
#include <iostream>
#include <vector>
#include <windows.h>
#include <sstream>

using namespace std;

CRITICAL_SECTION cs;

struct MarkerThreadParams
{
    int id;
    std::vector<int> *arr;
    int arraySize;
    HANDLE hStartEvent;
    HANDLE hBlockedEvent;
    HANDLE hResumeEvent;
    int*   thread_to_terminate_id;
};

DWORD WINAPI marker_thread(LPVOID params)
{
    MarkerThreadParams* p = static_cast<MarkerThreadParams*>(params);
    int id = p->id;
    vector<int>& arr = *(p->arr);
    int arraySize = p->arraySize;

    {
        stringstream ss;
        ss << "Marker thread #" << id << " created and waiting for start signal.\n";
        cout << ss.str();
    }

    WaitForSingleObject(p->hStartEvent, INFINITE);

    srand(id);
    int markedCount = 0;

    while (true)
    {
        int index = rand() % arraySize;
        bool isBlocked = false;

        EnterCriticalSection(&cs);
        if (arr[index] == 0) 
        {
            arr[index] = id;
            markedCount++;
        } else 
        {
            isBlocked = true;
        }
        LeaveCriticalSection(&cs);

        if (isBlocked)
        {
            {
                stringstream ss;
                EnterCriticalSection(&cs);
                ss << "Thread #" << id << ": cannot mark element " << index << ". Marked elements: " << markedCount << ". Waiting...\n";
                cout << ss.str();
                LeaveCriticalSection(&cs);
            }

            SetEvent(p->hBlockedEvent);
            WaitForSingleObject(p->hResumeEvent, INFINITE);

            EnterCriticalSection(&cs);
            bool should_terminate = (*(p->thread_to_terminate_id) == id);
            LeaveCriticalSection(&cs);

            if (should_terminate) 
            {
                EnterCriticalSection(&cs);
                cout << "Thread #" << id << " received terminate signal, cleaning up." << endl;
                for (int i = 0; i < arraySize; ++i) 
                {
                    if (arr[i] == id) 
                    {
                        arr[i] = 0;
                    }
                }
                LeaveCriticalSection(&cs);
                break;
            }
        }
        else
        {
            Sleep(5);
            Sleep(5);
        }
    }

    cout << "Thread #" << id << " is finishing its work." << endl;
    delete p;
    return 0;
}

int main()
{
    int arraySize;
    cout << "Enter the size of the array: ";
    cin >> arraySize;

    int markersCount;
    cout << "Enter the number of marker threads: ";
    cin >> markersCount;

    vector<int> arr(arraySize, 0);
    InitializeCriticalSection(&cs);

    vector<HANDLE> hMarkers(markersCount);
    vector<int> marker_ids(markersCount);
    vector<HANDLE> hBlockedEvents(markersCount);
    vector<HANDLE> hResumeEvents(markersCount);
    int thread_to_terminate_id = -1;

    HANDLE hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    for (int i = 0; i < markersCount; ++i) 
    {
        marker_ids[i] = i;
        hBlockedEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        hResumeEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    for (int i = 0; i < markersCount; ++i) 
    {
        MarkerThreadParams *params = new MarkerThreadParams();
        params->id = marker_ids[i];
        params->arr = &arr;
        params->arraySize = arraySize;
        params->hStartEvent = hStartEvent;
        params->hBlockedEvent = hBlockedEvents[i];
        params->hResumeEvent = hResumeEvents[i];
        params->thread_to_terminate_id = &thread_to_terminate_id;
        hMarkers[i] = CreateThread(NULL, 0, marker_thread, params, 0, NULL);
    }

    EnterCriticalSection(&cs);
    cout << "All threads created. Press Enter to start." << endl;
    LeaveCriticalSection(&cs);

    cin.ignore();
    cin.get();
    cout << "--- START ---" << endl;
    SetEvent(hStartEvent);

    int activeMarkersCount = markersCount;
    while (activeMarkersCount > 0)
    {
        WaitForMultipleObjects(activeMarkersCount, hBlockedEvents.data(), TRUE, INFINITE);
        for(int i = 0; i < activeMarkersCount; ++i) 
        {
            ResetEvent(hBlockedEvents[i]);
        }
        
        cout << "\nAll active threads are blocked. Array state:" << endl;
        Utils::printArray(arr, cs);
        int terminate_id_input = -1;
        int terminate_idx = -1;
        bool id_found = false;
        
        while (!id_found) 
        {
            cout << "Enter ID of thread to terminate: ";
            cin >> terminate_id_input;
            for (int i = 0; i < activeMarkersCount; i++) 
            {
                if (marker_ids[i] == terminate_id_input) 
                {
                    terminate_idx = i;
                    id_found = true;
                    break;
                }
            }
            if (!id_found) 
            {
                cout << "Invalid ID. Please enter an active thread ID." << endl;
            }
        }

        EnterCriticalSection(&cs);
        thread_to_terminate_id = terminate_id_input;
        LeaveCriticalSection(&cs);

        SetEvent(hResumeEvents[terminate_idx]);
        WaitForSingleObject(hMarkers[terminate_idx], INFINITE);
        
        cout << "Thread #" << terminate_id_input << " has terminated. Array state after cleanup:" << endl;
        Utils::printArray(arr, cs);

        CloseHandle(hMarkers[terminate_idx]);
        CloseHandle(hBlockedEvents[terminate_idx]);
        CloseHandle(hResumeEvents[terminate_idx]);

        activeMarkersCount--;
        if (terminate_idx < activeMarkersCount) 
        {
            hMarkers[terminate_idx] = hMarkers[activeMarkersCount];
            marker_ids[terminate_idx] = marker_ids[activeMarkersCount];
            hBlockedEvents[terminate_idx] = hBlockedEvents[activeMarkersCount];
            hResumeEvents[terminate_idx] = hResumeEvents[activeMarkersCount];
        }

        if (activeMarkersCount > 0) 
        {
            cout << "Press Enter to resume remaining threads..." << endl;
            cin.ignore();
            cin.get();
            
            EnterCriticalSection(&cs);
            thread_to_terminate_id = -1;
            LeaveCriticalSection(&cs);
            for(int i = 0; i < activeMarkersCount; ++i) 
            {
                SetEvent(hResumeEvents[i]);
            }
        }
    }

    cout << "\n--- ALL WORK IS COMPLETED ---" << std::endl;

    CloseHandle(hStartEvent);
    DeleteCriticalSection(&cs);

    system("pause");
    return 0;
}