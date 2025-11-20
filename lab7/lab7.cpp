#include <cfenv>
#include <cmath>
#include <csignal>
#include <iostream>

float a = 0.1f, b = 0.2f, c = 0.0f;

void signalHandler(int signum) {
  std::cout << "division by zero! signal: " << signum << "\n";
  std::cout << "result: " << c << "\n";

  // Очистка исключений FPU
  asm {
        fclex
  }

  exit(signum);
}

int main() {
  std::cout << "1) fadd\n";
  asm {
    finit // Инициализация FPU
    fld dword ptr [a] // Загружаем a в стек FPU (st(0))
    fld dword ptr [b] // Загружаем b в стек FPU (теперь st(0)=b, st(1)=a)
    faddp st(1), st(0) // Складываем st(1) и st(0), результат в st(1), извлекаем стек
    fstp dword ptr [c] // Сохраняем вершину стека в c и извлекаем
  }
  std::cout << a << " + " << b << " = " << c << "\n\n";
  std::cout << "2) sqrt from negative\n";
  float f = -1.0f;
  // Квадратный корень из отрицательного числа
  asm {
        finit
        fld dword ptr [f]   
        fsqrt       
        fstp dword ptr [c]
  }
  std::cout << "NaN value: " << c << "\n\n";

  std::cout << "3) dividing by zero\n";
  signal(SIGFPE, signalHandler);
  feenableexcept(FE_ALL_EXCEPT);
  a = 0.1f;
  b = 0.0f;
  // Деление на ноль
  asm {
        finit
        fld dword ptr [a]
        fld dword ptr [b]
        fdivp st(1), st(0)
        fstp dword ptr [c]
  }
  ;

  return 0;
}
