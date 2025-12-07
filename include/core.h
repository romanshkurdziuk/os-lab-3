#pragma once
#include <vector>
#include <windows.h>
#include "logic.h"

class SyncController {
public:
    // Конструктор инициализирует ресурсы
    SyncController(int arraySize, int markersCount);
    
    // Деструктор чистит ресурсы
    ~SyncController();

    // Главный метод: запускает симуляцию
    void run();

private:
    // Данные
    int arraySize;
    int markersCount;
    std::vector<int> arr;
    
    // Синхронизация
    CRITICAL_SECTION cs;
    HANDLE hStartEvent;

    // Списки ресурсов для каждого потока
    std::vector<HANDLE> hThreads;
    std::vector<int> markerIds;
    std::vector<HANDLE> hBlockedEvents;
    std::vector<HANDLE> hResumeEvents;
    
    // Флаг для коммуникации
    int thread_to_terminate_id;

    // Статическая функция потока (Windows API требует void*)
    static DWORD WINAPI markerRoutine(LPVOID params);
};