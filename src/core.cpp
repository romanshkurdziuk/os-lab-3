#include "core.h"
#include "ArrayUtils.h" 
#include <iostream>
#include <random>
#include <memory> 

using namespace std;

SyncController::SyncController(int size, int count) 
    : arraySize(size), markersCount(count), thread_to_terminate_id(-1) 
{
    arr.resize(arraySize, 0);
    InitializeCriticalSection(&cs);

    hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    markerIds.resize(markersCount);
    hThreads.resize(markersCount);
    hBlockedEvents.resize(markersCount);
    hResumeEvents.resize(markersCount);

    for (int i = 0; i < markersCount; ++i) 
    {
        markerIds[i] = i + 1;
        hBlockedEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        hResumeEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
}

SyncController::~SyncController()
{
    CloseHandle(hStartEvent);
    for (auto h : hBlockedEvents) CloseHandle(h);
    for (auto h : hResumeEvents) CloseHandle(h);
    DeleteCriticalSection(&cs);
}

DWORD WINAPI SyncController::markerRoutine(LPVOID params) 
{
    std::unique_ptr<MarkerThreadData> p(static_cast<MarkerThreadData*>(params));

    int id = p->id;
    std::mt19937 gen(id * 1000 + GetTickCount());
    std::uniform_int_distribution<> dist(0, p->arraySize - 1);

    {
        ScopedLock lock(*(p->cs));
        cout << "Marker thread #" << id << " ready.\n";
    }

    WaitForSingleObject(p->hStartEvent, INFINITE);

    int markedCount = 0;

    while (true) 
    {
        int index = dist(gen);
        bool isBlocked = false;

        {
            ScopedLock lock(*(p->cs));
            if ((*p->arr)[index] == 0) 
            {
                (*p->arr)[index] = id;
                markedCount++;
            } else 
            {
                isBlocked = true;
            }
        }

        if (isBlocked) 
        {
            {
                ScopedLock lock(*(p->cs));
                cout << "Thread #" << id << " blocked at index " << index 
                     << ". Marked: " << markedCount << "\n";
            }
            SetEvent(p->hBlockedEvent);

            WaitForSingleObject(p->hResumeEvent, INFINITE);

            bool terminate = false;
            {
                ScopedLock lock(*(p->cs));
                terminate = (*(p->thread_to_terminate_id) == id);
            }

            if (terminate) 
            {
                ScopedLock lock(*(p->cs));
                cout << "Thread #" << id << " terminating. Cleaning up...\n";
                for (int& val : *(p->arr)) 
                {
                    if (val == id) val = 0;
                }
                break;
            }
        } else 
        {
            Sleep(5);
        }
    }
    return 0;
}

void SyncController::run() 
{
    // 1. Запуск потоков
    for (int i = 0; i < markersCount; ++i) 
    {
        MarkerThreadData* data = new MarkerThreadData();
        data->id = markerIds[i];
        data->arraySize = arraySize;
        data->arr = &arr;
        data->hStartEvent = hStartEvent;
        data->hBlockedEvent = hBlockedEvents[i];
        data->hResumeEvent = hResumeEvents[i];
        data->cs = &cs;
        data->thread_to_terminate_id = &thread_to_terminate_id;

        hThreads[i] = CreateThread(NULL, 0, markerRoutine, data, 0, NULL);
    }

    cout << "All threads launched. Press Enter to START RACE.";
    cin.ignore(); cin.get();
    
    SetEvent(hStartEvent);

    int activeCount = markersCount;
    
    while (activeCount > 0) 
    {
        WaitForMultipleObjects(activeCount, hBlockedEvents.data(), TRUE, INFINITE);

        for (int i = 0; i < activeCount; ++i) ResetEvent(hBlockedEvents[i]);

        cout << "\n--- DEADLOCK DETECTED ---\n";
        Utils::printArray(arr, cs);

        int targetId = -1;
        int targetIdx = -1;
        bool found = false;

        while (!found) 
        {
            cout << "Enter ID to terminate: ";
            if (!(cin >> targetId)) 
            { 
                cin.clear(); cin.ignore(1000, '\n'); 
                continue; 
            }
            
            for (int i = 0; i < activeCount; ++i) 
            {
                if (markerIds[i] == targetId) 
                {
                    targetIdx = i;
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Invalid ID. Try again.\n";
        }

        {
            ScopedLock lock(cs);
            thread_to_terminate_id = targetId;
        }

        SetEvent(hResumeEvents[targetIdx]);
        WaitForSingleObject(hThreads[targetIdx], INFINITE);

        cout << "Thread " << targetId << " removed.\n";
        Utils::printArray(arr, cs);

        CloseHandle(hThreads[targetIdx]);
        CloseHandle(hBlockedEvents[targetIdx]);
        CloseHandle(hResumeEvents[targetIdx]);

        activeCount--;
        if (targetIdx < activeCount) 
        {
            hThreads[targetIdx] = hThreads[activeCount];
            markerIds[targetIdx] = markerIds[activeCount];
            hBlockedEvents[targetIdx] = hBlockedEvents[activeCount];
            hResumeEvents[targetIdx] = hResumeEvents[activeCount];
        }

        if (activeCount > 0) {
            cout << "Press Enter to resume remaining threads...";
            cin.ignore(); cin.get();

            {
                ScopedLock lock(cs);
                thread_to_terminate_id = -1;
            }
            for (int i = 0; i < activeCount; ++i) 
            {
                SetEvent(hResumeEvents[i]);
            }
        }
    }

    cout << "\n=== SIMULATION FINISHED ===\n";
}