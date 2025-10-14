format ELF64

section '.data' writeable
    array   dd  100, 200, 300, 400
    base    dd  array
    value   dd  1234

section '.text' executable
public _start

_start:
    ; Инициализация регистров 
    mov     ebx, [value]      
    lea     edi, [array]      
    mov     ecx, 2            
    lea     ebx, [array]      

    ; 1. Регистровая адресация
    mov     ax, bx          

    ; 2. Косвенная регистровая адресация
    mov     esi, [edi]        
                              

    ; 3. Индексная/базовая адресация (база - константа)  
    mov     edx, [array + ecx*4]  
                                  

    ; 4. Базово-индексная адресация (база - регистер)
    xor     edi, edi          
    mov     edi, 1            
    mov     r8,  [ebx + edi*4]    
                                  

    ; 5. Базово-индексная со смещением (база + индекс + константа)
    mov     r9,  [ebx + ecx*8] 

    ; Альтернативный вариант базово-индексной со смещением
    mov     r10, [ebx + ecx*4 - 4] 

    ; Завершение программы
    mov     rax, 60           
    xor     rdi, rdi          
    syscall
