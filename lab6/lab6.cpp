#include <csetjmp>
#include <csignal>
#include <iostream>
#include <termios.h>
#include <unistd.h>

jmp_buf
    div_zero_env; // массив, в котором лежит информация для восстановления среды
struct sigaction old_sigusr1, old_sigfpe; // изменение действий процесса, при
                                          // получении соответствующего сигнала

// Обработчик для пользовательского сигнала (аналог int 0xF0)
void sigusr1_handler(int sig) {
  std::cout << "my interrupt\n";
  getchar();
}

// Обработчик для деления на ноль (сигнал 8, информация о сигнале, контекст
// (регистры, состояние процессора)) FPE - floating - point exception
void sigfpe_handler(int sig, siginfo_t *info, void *context) {
  std::cout << "my div by zero\n";

  // Восстанавливаем контекст, сохранённый в div_zero_env (1 - параметр, который
  // вернёт sigsetjmp)
  siglongjmp(div_zero_env, 1);
}

void first_task() {
  // Генерируем пользовательский сигнал вместо int 0xF0
  raise(SIGUSR1);
}

void second_task() {
  // Вернёт 0 при первом вызове
  if (sigsetjmp(div_zero_env, 1) == 0) {
    int a = 5;
    int b = 0;
    int res = a / b; // Вызовет SIGFPE  (уходим в sigfpe_handler)
  }

  // Вернулись из sigfpe_handler

  std::cout << "return from div by zero interrupt\n";
}

int main() {
  system("clear"); // Аналог clrscr()

  // Настройка обработчика для SIGUSR1
  struct sigaction new_sigusr1;
  new_sigusr1.sa_handler = sigusr1_handler; // указатель на функцию
  sigemptyset(&new_sigusr1.sa_mask); // очищаем маску блокируемых сигналов
  new_sigusr1.sa_flags = 0;          // Без специальных флагов
  sigaction(SIGUSR1, &new_sigusr1, &old_sigusr1); // ставим обработчик

  // Настройка обработчика для SIGFPE (деление на ноль)
  struct sigaction new_sigfpe;
  new_sigfpe.sa_sigaction = sigfpe_handler; // Расширенный обработчик
  sigemptyset(&new_sigfpe.sa_mask);
  new_sigfpe.sa_flags = SA_SIGINFO; // Для доп информации
  sigaction(SIGFPE, &new_sigfpe, &old_sigfpe);

  first_task();
  second_task();

  std::cout << "return main\n";

  // Восстановление оригинальных обработчиков
  sigaction(SIGUSR1, &old_sigusr1, NULL);
  sigaction(SIGFPE, &old_sigfpe, NULL);

  getchar();
  return 0;
}
