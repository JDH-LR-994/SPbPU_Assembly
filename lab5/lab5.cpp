#include <iostream>

void addReg();
void addStack(long long int, long long int, long long int &);
void addGlobal();

static const int gl_a = 50;
static const int gl_b = 60;

long gl_sum = 0;

int main() {

  long long int a = 10;
  long long int b = 20;

  long long int sum;
  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;

  if (true) {
    __asm {
      mov eax, a
      mov ecx, b
      lea rsi, sum
    }
  }

  addReg();

  std::cout << "Add Register Sum: " << sum << std::endl;

  if (true) {
    __asm {
      mov a, 20
      mov b, 30
    }
  }
  addStack(a, b, sum);

  std::cout << "a: " << a << std::endl;
  std::cout << "b: " << b << std::endl;

  std::cout << "Add Stack Sum: " << sum << std::endl;

  addGlobal();

  std::cout << "gl_a: " << gl_a << std::endl;
  std::cout << "gl_b: " << gl_b << std::endl;

  std::cout << "Add Global Sum: " << gl_sum << std::endl;
}

void addReg() {
  __asm {
    mov qword ptr [rsi], 0
    add [rsi], rax
    add [rsi], rcx
  }
}

void addStack(long long int a, long long int b, long long int &sum) {
  __asm {
    mov rax, rdi
    add rax, rsi
    mov [rdx], rax
  }
}

void addGlobal() {
  __asm {
    mov eax, gl_a
    add eax, gl_b
    mov gl_sum, eax
  }
}
