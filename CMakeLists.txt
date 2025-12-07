cmake_minimum_required(VERSION 3.14)
project(OperatingSystemsLab3 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Подключение системных зависимостей ---
find_package(Threads REQUIRED)

# --- 1. Сборка библиотеки утилит (ArrayUtils) ---
# Эта библиотека содержит вспомогательные функции вывода
add_library(ArrayUtils src/ArrayUtils.cpp)

# Указываем, что заголовки (.h) лежат в папке include
# PUBLIC означает, что любой, кто подключит эту библиотеку, тоже увидит папку include
target_include_directories(ArrayUtils PUBLIC include)

# --- 2. Сборка основной программы (Lab3_Sync) ---
# Включаем main.cpp и core.cpp (наш контроллер)
add_executable(Lab3_Sync 
    src/main.cpp 
    src/core.cpp
)

# Подключаем библиотеку ArrayUtils и системные потоки
# Благодаря связи с ArrayUtils, программа автоматически видит заголовки в папке include
target_link_libraries(Lab3_Sync ArrayUtils Threads::Threads)

# --- 3. Настройка Тестов (GTest) ---
enable_testing()

find_package(GTest)

# Если GTest не найден в системе, скачиваем его из GitHub
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/heads/main.zip
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    # Настройка для Windows: использовать общую Runtime библиотеку (как в основной программе)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Собираем исполняемый файл тестов
add_executable(UnitTests tests/ArrayUtils_test.cpp)

# Подключаем тестируемую библиотеку (ArrayUtils) и сам GTest
target_link_libraries(UnitTests ArrayUtils GTest::gtest_main)

# Регистрация тестов в системе CTest
include(GoogleTest)
gtest_discover_tests(UnitTests)