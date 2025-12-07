#pragma once
#include <windows.h>
#include <vector>

// RAII Wrapper для Критической секции
// Захватывает мьютекс при создании, отпускает при удалении
class ScopedLock {
public:
    explicit ScopedLock(CRITICAL_SECTION& cs) : cs_(cs) {
        EnterCriticalSection(&cs_);
    }
    ~ScopedLock() {
        LeaveCriticalSection(&cs_);
    }
    // Запрет копирования
    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;
private:
    CRITICAL_SECTION& cs_;
};

// Параметры, которые мы передаем в рабочий поток
struct MarkerThreadData {
    int id;                 // Порядковый номер потока
    int arraySize;          // Размер массива
    std::vector<int>* arr;  // Указатель на общий массив
    
    // Синхронизация
    HANDLE hStartEvent;
    HANDLE hBlockedEvent;
    HANDLE hResumeEvent;
    
    // Указатель на общую критическую секцию (чтобы поток мог её блокировать)
    CRITICAL_SECTION* cs;
    
    // Указатель на переменную "кого убить"
    int* thread_to_terminate_id; 
};