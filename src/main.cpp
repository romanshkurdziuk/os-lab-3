#include <iostream>
#include "core.h"

int main() {
    int arraySize, markersCount;

    std::cout << "=== LAB 3: THREAD SYNCHRONIZATION ===\n";
    std::cout << "Enter array size: ";
    std::cin >> arraySize;
    
    std::cout << "Enter markers count: ";
    std::cin >> markersCount;

    try {
        // Создаем движок
        SyncController controller(arraySize, markersCount);
        // Запускаем
        controller.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    system("pause");
    return 0;
}