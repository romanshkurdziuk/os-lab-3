# 🚦 Thread Synchronization & Deadlock Recovery

![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6.svg?style=flat&logo=windows)
![Build](https://img.shields.io/badge/Build-CMake-064F8C.svg?style=flat&logo=cmake)
![Tests](https://img.shields.io/badge/Tests-GoogleTest-green.svg?style=flat&logo=google)
![License](https://img.shields.io/badge/License-MIT-green.svg)

A high-performance multithreaded application demonstrating **process synchronization**, **shared memory protection**, and **deadlock resolution** using native Windows API primitives.

The project implements a **Controller-Worker** architecture where multiple "Marker" threads compete to fill a shared array. The system automatically detects deadlocks (when all threads are blocked) and allows the user to resolve them interactively.

---

## 🚀 Key Features

*   **Clean Architecture:**
    *   **Core Logic:** Separated into `SyncController` class (`core.cpp`), keeping `main.cpp` clean.
    *   **RAII Idiom:** Custom `ScopedLock` wrapper for Critical Sections to prevent deadlocks and ensure exception safety.
    *   **Standard Layout:** Professional folder structure (`include`, `src`, `tests`).
*   **Robust Synchronization:**
    *   **Critical Sections:** For atomic access to shared memory and console output.
    *   **Events (`HANDLE`):** Complex signaling protocol (Start -> Blocked -> Resume/Terminate).
*   **Modern C++:** Usage of `<random>` for high-quality distribution (replacing legacy `rand()`) and smart pointers.
*   **Quality Assurance:** Integrated **GoogleTest** via CMake `FetchContent` for automated unit testing.

---

## 📂 Project Structure

```text
OS-LAB-3/
├── CMakeLists.txt       # Build configuration & dependency management
├── include/             # Public Headers
│   ├── core.h           # Controller logic definition
│   ├── logic.h          # RAII wrappers and shared structures
│   └── ArrayUtils.h     # Helper functions
├── src/                 # Implementation
│   ├── core.cpp         # Main orchestration & Thread routine
│   ├── main.cpp         # Entry point
│   └── ArrayUtils.cpp   # Formatting utilities
└── tests/               # Unit Tests
    └── ArrayUtils_test.cpp
```

---

## 🛠️ How it Works

### 1. The Controller (Main Thread)
Acts as the arbiter. It:
1.  Spawns `N` **Marker Threads**.
2.  Signals the global start.
3.  **Deadlock Detection:** Waits until *ALL* active threads signal they are "Blocked" (cannot find an empty slot in the array).
4.  **Resolution:** Prompts the user to select a thread ID to terminate.
5.  **Cleanup:** Signals the chosen thread to exit (clearing its marked cells) and wakes up the survivors.

### 2. The Marker Threads (Workers)
Each thread has a unique ID.
1.  Generates a random index using `std::mt19937`.
2.  **Atomic Check:**
    *   If `array[index] == 0`: Mark it with own ID, sleep, and repeat.
    *   If `array[index] != 0`: Signal **"Blocked"** event to Controller and wait.
3.  **On Resume:** Checks if it was targeted for termination.
    *   If yes: Clear all owned cells (`0`) and exit.
    *   If no: Continue the race.

---

## ⚙️ Building & Running

### Prerequisites
*   Windows OS (Required for `<windows.h>`)
*   CMake (3.14+)
*   C++ Compiler (MSVC or MinGW)

### Compilation Steps

1.  Clone the repository:
    ```bash
    git clone https://github.com/YourUsername/os-lab-3.git
    cd os-lab-3
    ```

2.  Create a build directory:
    ```bash
    mkdir build
    cd build
    ```

3.  Compile using CMake (this will automatically download GTest):
    ```bash
    cmake ..
    cmake --build .
    ```

---

## 🎮 Usage

Run the executable from the build folder:

```powershell
./Lab3_Sync.exe
```

**Interaction Flow:**
1.  **Setup:** Enter the array size (e.g., `10`) and number of threads (e.g., `3`).
2.  **Simulation:** Watch threads mark the array in real-time.
3.  **Deadlock:** When all threads stop with `--- DEADLOCK DETECTED ---`, the system waits for input.
4.  **Resolution:** Enter the **ID** of the thread you want to remove (e.g., `1`).
5.  **Loop:** The simulation continues until all threads are terminated.

---

## 🧪 Running Tests

The project uses GoogleTest to ensure the reliability of utility components.

```powershell
./UnitTests.exe
```

**Expected Output:**
```text
[==========] Running 3 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 3 tests from ArrayFormatting
[ RUN      ] ArrayFormatting.HandlesEmptyVector
[       OK ] ArrayFormatting.HandlesEmptyVector (0 ms)
...
[  PASSED  ] 3 tests.
```

---

## 🧩 Code Snippet (RAII Synchronization)

A peek into how we safely handle concurrency using our `ScopedLock` wrapper:

```cpp
// Inside Marker Thread Loop
{
    ScopedLock lock(*(p->cs)); // Auto-locks Critical Section
    
    if ((*p->arr)[index] == 0) {
        (*p->arr)[index] = id; // Mark the spot
        markedCount++;
    } else {
        isBlocked = true;
    }
} // Auto-unlocks here (even if exception occurs)

if (isBlocked) {
    SetEvent(p->hBlockedEvent); // Signal Controller
    WaitForSingleObject(p->hResumeEvent, INFINITE); // Wait for decision
}
```

---

## 👨‍💻 Author

Student of BSU FAMCS
Project for "Operating Systems" Course (Lab 3).