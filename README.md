# Polynomial Solver

Навчальний проєкт на C++17 для обчислення значення полінома, його похідної
та пошуку дійсних коренів. Математичні функції зібрані в динамічну бібліотеку
`SHARED` і доступні через C-сумісний інтерфейс (`extern "C"`).

## Вимоги

- Windows;
- компілятор GCC із `g++` (наприклад, MinGW-w64 або MSYS2) для команд нижче;
- для збирання через CMake — встановлений CMake або CMake, налаштований в IDE.

Перевірити наявність інструментів у PowerShell:

```powershell
g++ --version
cmake --version
```

## Запуск у VS Code через PowerShell

Відкрийте папку проєкту у VS Code, виберіть **Terminal → New Terminal** і
виконайте команди з кореневої папки проєкту.

Вказати напрямок папки:

```powershell
Set-Location "***********"
```

Зберіть DLL та тестову програму:

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -DPOLYNOMIAL_SOLVER_BUILD -shared Code.cpp '-Wl,--out-implib,libpolynomial_solver.dll.a' -o polynomial_solver.dll

g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror main.cpp -L. -lpolynomial_solver -o polynomial_solver_test.exe
```

Запустіть тестову програму:

```powershell
.\polynomial_solver_test.exe
```

Файл `polynomial_solver.dll` має залишатися в тій самій папці, що й
`polynomial_solver_test.exe`, щоб Windows знайшла бібліотеку під час запуску.

### Збирання і запуск через CMake у терміналі

Якщо `cmake --version` спрацювала, а генератор MinGW доступний:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\polynomial_solver_test.exe
```

Якщо CMake не може знайти компілятор або генератор, налаштуйте шлях до MinGW
у параметрах CMake чи IDE або скористайтеся прямими командами `g++` вище.

## Запуск у CLion

1. Відкрийте в CLion саме папку проєкту, в якій лежить `CMakeLists.txt`.
2. Якщо CLion запропонує завантажити або перезавантажити CMake-проєкт,
   погодьтеся.
3. У **Settings → Build, Execution, Deployment → Toolchains** виберіть
   налаштований компілятор C++ (наприклад, MinGW із GCC).
4. Виберіть CMake-профіль, що використовує цей Toolchain.
5. У списку конфігурацій запуску виберіть `polynomial_solver_test` і натисніть
   **Run**. Якщо конфігурації ще немає, створіть її типу **CMake Application**
   з target `polynomial_solver_test`.

CLion використовує `CMakeLists.txt` для збирання DLL і тестового застосунку.
Компілятор і CMake мають бути сумісними та налаштованими в одному Toolchain.
Після успішного запуску результати з'являться у вікні **Run**. Під час
звичайної роботи з CLion окремо вводити команди в терміналі не потрібно.

## Приклад результату

```text
Polynomial: x^3 - 6x^2 + 11x - 6
polynomial_evaluate(p, 2) = 0
polynomial_derivative(p), evaluated at 2 = -1
polynomial_newton_root(p, 2.8) = 3
polynomial_find_roots(p) = 1, 2, 3

Polynomial: (x - 1)^2 * (x + 2)
polynomial_find_roots(p) = -2, 1

Polynomial: x^2 + 1
polynomial_find_roots(p) = no real roots

Polynomial: 2x - 4
polynomial_find_roots(p) = 2

All polynomial solver checks passed.
```

## Файли проєкту

| Файл | Призначення |
| --- | --- |
| `CMakeLists.txt` | Описує C++17-проєкт, динамічну бібліотеку `polynomial_solver` і тестовий застосунок. |
| `PolynomialSolver.h` | Публічний C-сумісний заголовок: оголошення функцій, статусів і макрос експорту/імпорту DLL. |
| `Code.cpp` | Реалізує алгоритми поліномів і C-ABI функції-обгортки бібліотеки. |
| `main.cpp` | Тестовий застосунок: перевіряє функції та друкує приклади результатів. |
| `polynomial_solver.dll` | Динамічна бібліотека Windows, яку тестовий застосунок використовує під час запуску. Створюється під час збирання. |
| `polynomial_solver_test.exe` | Виконуваний тестовий застосунок. Створюється під час збирання. |

## Інтерфейс бібліотеки

У `PolynomialSolver.h` оголошені функції:

- `polynomial_evaluate` — обчислює значення полінома в заданій точці;
- `polynomial_derivative` — записує коефіцієнти похідної в переданий буфер;
- `polynomial_newton_root` — шукає корінь методом Ньютона;
- `polynomial_find_roots` — знаходить дійсні корені полінома.

Коефіцієнти передаються масивом, починаючи з вільного члена: для
`x^3 - 6x^2 + 11x - 6` це `{-6, 11, -6, 1}`. Усі функції повертають
`PolynomialStatus`; `POLYNOMIAL_STATUS_OK` означає успіх. Для похідної та
коренів додатково передаються буфер результату, його місткість і покажчик,
у який функція записує необхідну кількість елементів.

Функції C-ABI не передають C++-винятки через межу бібліотеки: помилки
повертаються як коди статусу.
