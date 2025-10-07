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
    ; Получаем число из массива
    movzx eax, byte [rbx]  ; загружаем число в EAX

    ; Вызываем процедуру преобразования и вывода числа
    call print_number

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

; Процедура преобразования и вывода числа
; Вход: EAX = число для вывода (0-255)
print_number:
    push rbx
    push rcx
    push rdx
    push rsi

    mov ecx, 10         ; делитель
    lea rsi, [char_buffer+3] ; указываем на конец буфера +1
    mov byte [rsi], 0   ; терминатор

    ; Специальная обработка для числа 0
    test eax, eax
    jnz .convert_loop
    ; Если число 0, просто выводим '0'
    mov byte [char_buffer+2], '0'
    mov rsi, char_buffer+2
    mov rdx, 1
    jmp .output

.convert_loop:
    xor edx, edx
    div ecx             ; EDX = остаток, EAX = частное
    add dl, '0'
    dec rsi
    mov [rsi], dl
    test eax, eax
    jnz .convert_loop

    ; Вычисляем длину строки
    mov rdx, char_buffer+3
    sub rdx, rsi        ; длина строки

.output:
    mov rax, 1
    mov rdi, 1
    syscall

    pop rsi
    pop rdx
    pop rcx
    pop rbx
    ret

segment readable writable
    ; Массив с числами разной разрядности
    A db 5, 12, 255, 3, 100, 0, 99, 10, 1, 200
    B db 10 dup(0)
    char_buffer db 0, 0, 0, 0  ; увеличенный буфер для 3 цифр + терминатор
    space db ' '
    newline db 10
