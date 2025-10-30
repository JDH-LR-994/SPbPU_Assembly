#include <algorithm>
#include <iostream>
#include <iterator>

# define ROWS 5
# define COLS 4

unsigned short array[ROWS][COLS] = {0};


int main(){
  // Обычный способ:
  if (true) {
    __asm {
      lea rbx, array
      mov rcx, (ROWS + 1 ) / 2
      mov rsi, 0
      mov rax, 0
      cld

      loop1:
      mov [rbx + rsi * 8 + rax*2 ], 1
      add rax, 2

      add rsi, 2
      
      loop loop1
    }  
  }
  
  std::for_each(std::begin(array), std::end(array), [](auto row){
                  std::copy(std::begin(row), std::end(row), std::ostream_iterator<int>(std::cout, " "));
                  std::cout << std::endl;
                });
}
