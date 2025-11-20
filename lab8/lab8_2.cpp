#include <csignal>
#include <iostream>
#include <ucontext.h>

void breakpointHandler(int signum, siginfo_t *info, void *context) {
  std::cout << "=== ТОЧКА ОСТАНОВА ===" << std::endl;

  ucontext_t *uc = (ucontext_t *)context;

  // Показываем регистры
  std::cout << "RAX = " << uc->uc_mcontext.gregs[REG_RAX] << std::endl;
  std::cout << "RBX = " << uc->uc_mcontext.gregs[REG_RBX] << std::endl;

  std::cout << "Нажмите Enter для продолжения...";
  std::cin.get();
}

int main() {
  struct sigaction sa;
  sa.sa_sigaction = breakpointHandler;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGTRAP, &sa, nullptr);

  int x = 10;
  int y = 20;

  std::cout << "Перед точкой останова" << std::endl;

  // Точка останова 1
  if (true) {
    asm {
      int 3
    }
  }

  x = x + 5;
  y = y - 3;

  std::cout << "x = " << x << ", y = " << y << std::endl;

  // Точка останова 2
  if (true) {
    asm {
      int 3
    }
  }

  int result = x * y;
  std::cout << "Результат: " << result << std::endl;

  return 0;
}
