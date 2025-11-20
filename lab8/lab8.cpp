#include <csignal>
#include <iostream>
#include <ucontext.h>

volatile bool step_done = false;

void signalHandler(int signum, siginfo_t *info, void *context) {
  ucontext_t *uc = (ucontext_t *)context;

  // Получаем младшие 16 бит регистров RAX и RBX
  unsigned short ax_data = uc->uc_mcontext.gregs[REG_RAX] & 0xFFFF;
  unsigned short bx_data = uc->uc_mcontext.gregs[REG_RBX] & 0xFFFF;

  std::cout << "ax = " << ax_data << ", bx = " << bx_data << '\n';

  std::cin.get();
  step_done = true;
}

int main() {
  struct sigaction sa;
  sa.sa_sigaction = signalHandler;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGTRAP, &sa, nullptr);

  // Установка флага TF (Trap Flag) = 1
  if (true) {
    asm {
        pushfq // Сохраняем RFLAGS в стек
        pop rax // Загружаем RFLAGS в RAX
        or rax, 0x100 // Устанавливаем 8-й бит (TF) в 1
        push rax // Кладем модифицированные флаги в стек
        popfq // Восстанавливаем RFLAGS из стека
    }
  }

  // Серия инструкций, которые будут выполняться в пошаговом режиме
  if (true) {
    asm {
      mov ax, 10 // ax = 10
      mov bx, 20 // bx = 20
      add ax,5 // ax = 15
      add bx, 7 // bx = 27
      add ax, 3 // ax = 18
      add bx, 1 // bx = 28
    }
  }

  // Сброс флага TF (Trap Flag) = 0
  // Используем правильную 64-битную маску
  if (true) {
    asm {
      pushfq // Сохраняем RFLAGS в стек
      pop rax // Загружаем RFLAGS в RAX
      mov rbx,        0xFFFFFFFFFFFFFEFF // 64-битная маска для сброса TF
      and rax, rbx // Сбрасываем 8-й бит (TF) в 0
      push rax // Кладем модифицированные флаги в стек
      popfq // Восстанавливаем RFLAGS из стека
    }
  }

  return 0;
}
