#include <csetjmp>
#include <csignal>
#include <cstring>
#include <iostream>
#include <termios.h>
#include <unistd.h>

jmp_buf div_zero_env;
struct sigaction old_sigusr1, old_sigfpe;

void sigusr1_handler(int sig) {
  std::cout << "=== Обработчик SIGUSR1 вызван ===" << std::endl;
  std::cout << "Получен сигнал: " << sig << " (SIGUSR1)" << std::endl;
  std::cout << "PID процесса: " << getpid() << std::endl;
  std::cout << "Нажмите Enter для продолжения...";
  getchar();
  std::cout << "=== Выход из обработчика SIGUSR1 ===" << std::endl;
}

void sigfpe_handler(int sig, siginfo_t *info, void *context) {
  std::cout << "=== Обработчик SIGFPE вызван ===" << std::endl;
  std::cout << "Получен сигнал: " << sig << " (SIGFPE)" << std::endl;
  std::cout << "Код ошибки: " << info->si_code << std::endl;
  std::cout << "Адрес ошибки: " << info->si_addr << std::endl;

  // Расшифровка кода ошибки
  const char *error_desc = "Неизвестная ошибка";
  switch (info->si_code) {
  case FPE_INTDIV:
    error_desc = "Целочисленное деление на ноль";
    break;
  case FPE_INTOVF:
    error_desc = "Целочисленное переполнение";
    break;
  case FPE_FLTDIV:
    error_desc = "Деление floating point на ноль";
    break;
  case FPE_FLTOVF:
    error_desc = "Переполнение floating point";
    break;
  case FPE_FLTUND:
    error_desc = "Потеря точности floating point";
    break;
  case FPE_FLTRES:
    error_desc = "Некорректный результат floating point";
    break;
  case FPE_FLTINV:
    error_desc = "Некорректная операция floating point";
    break;
  }
  std::cout << "Описание: " << error_desc << std::endl;

  std::cout << "Выполняем восстановление через siglongjmp..." << std::endl;
  siglongjmp(div_zero_env, 1);
}

void first_task() {
  std::cout << "\n--- Задача 1: Генерация SIGUSR1 ---" << std::endl;
  std::cout << "Вызов raise(SIGUSR1)..." << std::endl;
  raise(SIGUSR1);
  std::cout << "Возврат из обработчика сигнала" << std::endl;
}

void second_task() {
  std::cout << "\n--- Задача 2: Деление на ноль ---" << std::endl;

  // Устанавливаем точку возврата
  int jmp_val = sigsetjmp(div_zero_env, 1);

  if (jmp_val == 0) {
    std::cout << "Первая попытка: готовимся к делению на ноль" << std::endl;
    int a = 5;
    int b = 0;
    std::cout << "Вычисляем: " << a << " / " << b << std::endl;
    int res = a / b; // Это вызовет SIGFPE
    // Сюда мы никогда не попадем при первой попытке
    std::cout << "Результат: " << res << std::endl;
  } else {
    std::cout << "Вторая попытка: вернулись через siglongjmp с кодом "
              << jmp_val << std::endl;
  }

  std::cout << "Продолжение после обработки деления на ноль" << std::endl;
}

int main() {
  std::cout << "=== Программа начала выполнение ===" << std::endl;
  std::cout << "PID: " << getpid() << std::endl;

  // Настройка обработчика SIGUSR1
  struct sigaction sa_usr1;
  sa_usr1.sa_handler = sigusr1_handler;
  sigemptyset(&sa_usr1.sa_mask);
  sa_usr1.sa_flags = 0;

  if (sigaction(SIGUSR1, &sa_usr1, &old_sigusr1) == -1) {
    std::cerr << "Ошибка установки обработчика SIGUSR1: " << strerror(errno)
              << std::endl;
    return 1;
  }

  // Настройка обработчика SIGFPE с дополнительной информацией
  struct sigaction sa_fpe;
  sa_fpe.sa_sigaction = sigfpe_handler;
  sigemptyset(&sa_fpe.sa_mask);
  sa_fpe.sa_flags = SA_SIGINFO;

  if (sigaction(SIGFPE, &sa_fpe, &old_sigfpe) == -1) {
    std::cerr << "Ошибка установки обработчика SIGFPE: " << strerror(errno)
              << std::endl;
    return 1;
  }

  // Выполнение задач
  first_task();
  second_task();

  std::cout << "\n=== Возврат в main ===" << std::endl;

  // Восстановление обработчиков
  sigaction(SIGUSR1, &old_sigusr1, NULL);
  sigaction(SIGFPE, &old_sigfpe, NULL);

  std::cout << "Оригинальные обработчики восстановлены" << std::endl;
  std::cout << "Нажмите Enter для выхода...";
  getchar();

  return 0;
}
