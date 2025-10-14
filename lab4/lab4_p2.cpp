// Компилировать (использовалось): clang++ -fasm-blocks lab4_p2.cpp
// Компилировать (альтернатива, которая не проверялась): gcc -masm=intel lab4_p2.cpp

#include <iostream>
#include <chrono>


# define iterations 10000000000LL
int main(){
  
  std::cout << "Замеры времени для разных способов адресации" << std::endl;

  auto start = std::chrono::steady_clock::now();
  for (volatile std::size_t i = 0; i < iterations; i++){
  }

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  auto empty_loop_time = elapsed.count();  
  std::cout << "Время, потраченное на 10 миллиардов итераций: " << elapsed.count() << std::endl;

  start = std::chrono::steady_clock::now();
  for (volatile std::size_t i = 0; i < iterations; i++){
    // mov eax, 100 нужно, чтобы компилятор не оптимизировал этот блок (иначе время может выйти со знаком -)
    __asm {
      mov eax, 100
      add eax, ebx
    }
  }

  end = std::chrono::steady_clock::now();
  elapsed = end - start;
  std::cout << "Время, потраченное на 10 миллиардов операций add eax, ebx: " << elapsed.count() - empty_loop_time  << std::endl;

  volatile int memory_array[10] = {0, 1, 2,3, 4,5, 6, 7, 8, 9};

  // Обёртка, чтобы работало автодополнение. Можно убрать (обёртку, а вставку оставить!!)
  if (true){
    __asm{
      lea rbx, memory_array
      mov rsi, 0
    }
  }
  start = std::chrono::steady_clock::now();
  for (volatile std::size_t i = 0; i < iterations; i++){
    // Ситуация с mov аналогична, только здесь отрицательных чисел не возникнет
    __asm {
      mov eax, 100
      add rax, [rbx + rsi + 20]
      }
  }
  end = std::chrono::steady_clock::now();
  elapsed = end - start;
  std::cout << "Время, потраченное на 10 миллиардов операций add rax, [rbx + rsi + 5]: " << elapsed.count() - empty_loop_time << std::endl;
  volatile int memory_value = 1;
  if (true){
    __asm {
      lea rbx, memory_value
    }
  }
  start = std::chrono::steady_clock::now();
  for (volatile std::size_t i = 0; i < iterations; i++){
    
    __asm {
      mov eax, 100
      add rax, [rbx]
      }
  }
  end = std::chrono::steady_clock::now();
  elapsed = end - start;
  std::cout << "Время, потраченное на 10 миллиардов операций add rax, [rbx]: " << elapsed.count() - empty_loop_time << std::endl;
  
  volatile long long memory_value1 = 1;
  if (true){
    __asm {
      lea rbx, memory_value1
    }
  }
  start = std::chrono::steady_clock::now();
  for (volatile std::size_t i = 0; i < iterations; i++){
    
    __asm {
      mov rax, 1
      add [rbx], rax
      }
  }
  end = std::chrono::steady_clock::now();
  elapsed = end - start;
  std::cout << "Время, потраченное на 10 миллиардов операций add [rbx], rax: " << elapsed.count() - empty_loop_time << std::endl;
  
}
