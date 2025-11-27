#include <csignal>
#include <iostream>
#include <sys/time.h>
#include <ucontext.h>
#include <unistd.h>

#define STACK_SIZE 4096
#define TASK_COUNT 3

// Здесь храним информацию о задачах
struct Task {
  ucontext_t context;
  char stack[STACK_SIZE];
};

static Task tasks[TASK_COUNT];
static volatile sig_atomic_t current_task = 0;
static volatile sig_atomic_t task_switch_requested = 0;

void schedule();
void setup_timer();
void signal_handler(int);

// Обработчик прерываний таймера
void signal_handler(int) { task_switch_requested = 1; }

// Проверяем необходимость переключения задачи
void schedule() {
  if (!task_switch_requested)
    return;
  task_switch_requested = 0;

  int previous = current_task;
  current_task = (current_task + 1) % TASK_COUNT;

  std::cout << "\n[switch task " << current_task << "]" << std::endl;

  // Сохраняем контекст и устанавливаем новый
  swapcontext(&tasks[previous].context, &tasks[current_task].context);
}

// Установка таймера, каждые 500мс вызывает SIGALRM
void setup_timer() {
  struct itimerval timer;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 500000;
  timer.it_value = timer.it_interval;
  setitimer(ITIMER_REAL, &timer, NULL);
}

// Сами задачи
void task(int i, int delay) {
  while (true) {
    std::cout << i << std::flush;
    usleep(delay); // Задержка
    schedule();    // Проверяем необходимость переключения задачи
  }
}

int main() {
  // Настройка и установка обработчика прерываний по таймеру
  struct sigaction sa;
  sa.sa_handler = signal_handler; // Функция-обработчик
  sigemptyset(&sa.sa_mask);       // Очищаем маску сигналов
  sa.sa_flags = 0;                // Флаги по умолчанию
  sigaction(SIGALRM, &sa, NULL);  // Установка обработчика
  setup_timer();                  // Запуск таймера

  // Массив с функциями задач
  void (*funcs[TASK_COUNT])() = {[]() { task(0, 10000); },
                                 []() { task(1, 25000); },
                                 []() { task(2, 50000); }};

  // Настройка контекстов
  for (int i = 0; i < TASK_COUNT; ++i) {
    getcontext(&tasks[i].context);                    // Инициализация контекста
    tasks[i].context.uc_stack.ss_sp = tasks[i].stack; // Отдаем стек для задачи
    tasks[i].context.uc_stack.ss_size = sizeof(tasks[i].stack); // ...
    tasks[i].context.uc_link =
        nullptr; // Устанавливаем действие при завершении задачи (nullptr значит
                 // завершение программы)
    makecontext(&tasks[i].context, funcs[i],
                0); // Связываем контекст с функцией
  }

  setcontext(&tasks[0].context); // Запустили задачу 0
}
