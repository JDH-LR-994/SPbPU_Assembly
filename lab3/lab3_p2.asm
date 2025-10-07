format elf64 executable

segment readable executable

entry _start

_start:
    ; Копирование из A в B с использованием строковых команд
    cld
    lea rsi, [A]
    lea rdi, [B]
    mov rcx, 10
    rep movsb

    ; Вывод массива B (скопированного)
    lea rbx, [B]        ; используем rbx как указатель на массив
    mov r12, 10         ; используем r12 как счетчик

print_array:
    ; Преобразование числа в ASCII
    mov al, [rbx]
    add al, '0'
    mov [char_buffer], al
    
    ; Вывод одного символа
    mov rax, 1
    mov rdi, 1
    mov rsi, char_buffer
    mov rdx, 1
    syscall
    
    ; Вывод пробела между числами
    mov rax, 1
    mov rdi, 1
    mov rsi, space
    mov rdx, 1
    syscall
    
    inc rbx             ; следующий элемент
    dec r12             ; уменьшаем счетчик
    jnz print_array     ; если не ноль - продолжаем

    ; Вывод символа новой строки
    mov rax, 1
    mov rdi, 1
    mov rsi, newline
    mov rdx, 1
    syscall
    
    ; Завершение программы
    mov rax, 60
    xor edi, edi
    syscall

segment readable writable
    A db 0,1,2,3,4,5,6,7,8,9
    B db 10 dup(0)
    char_buffer db 0
    space db ' '
    newline db 10
