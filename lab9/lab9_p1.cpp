#include <errno.h>
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Функция для поиска клавиатурного устройства
int find_keyboard_device(char *device_path) {
  FILE *fp;
  char line[256];
  char event_path[32];
  char name[256];
  int found = 0;

  // Читаем устройства из /proc/bus/input/devices
  fp = fopen("/proc/bus/input/devices", "r");
  if (!fp) {
    perror("Не удалось открыть /proc/bus/input/devices");
    return -1;
  }

  while (fgets(line, sizeof(line), fp)) {
    if (strstr(line, "Handlers") && strstr(line, "kbd")) {
      // Нашли клавиатуру, извлекаем eventX
      char *handler = strstr(line, "event");
      if (handler) {
        sscanf(handler, "event%d", &found);
        break;
      }
    }
  }
  fclose(fp);

  if (found) {
    snprintf(device_path, 32, "/dev/input/event%d", found);
    return 0;
  }

  return -1;
}

// Функция для получения имени клавиши по коду
const char *get_key_name(int code) {
  switch (code) {
  case KEY_ESC:
    return "ESC";
  case KEY_1:
    return "1";
  case KEY_2:
    return "2";
  case KEY_3:
    return "3";
  case KEY_4:
    return "4";
  case KEY_5:
    return "5";
  case KEY_6:
    return "6";
  case KEY_7:
    return "7";
  case KEY_8:
    return "8";
  case KEY_9:
    return "9";
  case KEY_0:
    return "0";
  case KEY_Q:
    return "Q";
  case KEY_W:
    return "W";
  case KEY_E:
    return "E";
  case KEY_R:
    return "R";
  case KEY_T:
    return "T";
  case KEY_Y:
    return "Y";
  case KEY_U:
    return "U";
  case KEY_I:
    return "I";
  case KEY_O:
    return "O";
  case KEY_P:
    return "P";
  case KEY_A:
    return "A";
  case KEY_S:
    return "S";
  case KEY_D:
    return "D";
  case KEY_F:
    return "F";
  case KEY_G:
    return "G";
  case KEY_H:
    return "H";
  case KEY_J:
    return "J";
  case KEY_K:
    return "K";
  case KEY_L:
    return "L";
  case KEY_Z:
    return "Z";
  case KEY_X:
    return "X";
  case KEY_C:
    return "C";
  case KEY_V:
    return "V";
  case KEY_B:
    return "B";
  case KEY_N:
    return "N";
  case KEY_M:
    return "M";
  case KEY_SPACE:
    return "Space";
  case KEY_ENTER:
    return "Enter";
  case KEY_BACKSPACE:
    return "Backspace";
  case KEY_TAB:
    return "Tab";
  case KEY_LEFTSHIFT:
    return "Left Shift";
  case KEY_RIGHTSHIFT:
    return "Right Shift";
  case KEY_LEFTCTRL:
    return "Left Ctrl";
  case KEY_RIGHTCTRL:
    return "Right Ctrl";
  case KEY_LEFTALT:
    return "Left Alt";
  case KEY_RIGHTALT:
    return "Right Alt";
  case KEY_CAPSLOCK:
    return "Caps Lock";
  default:
    return "Unknown";
  }
}

int main() {
  int fd;
  struct input_event ev;
  char device_path[32];

  printf("Поиск клавиатурного устройства...\n");

  // Находим клавиатурное устройство
  if (find_keyboard_device(device_path) != 0) {
    printf("Клавиатура не найдена! Пытаемся использовать event0...\n");
    strcpy(device_path, "/dev/input/event0");
  }

  printf("Открываем устройство: %s\n", device_path);

  // Открываем устройство клавиатуры
  fd = open(device_path, O_RDONLY);
  if (fd == -1) {
    perror("Ошибка открытия устройства");
    printf("Попробуйте запустить с sudo\n");
    return 1;
  }

  printf("Клавиатура обнаружена. Нажимайте клавиши (Ctrl+C для выхода)...\n\n");

  // Основной цикл чтения событий
  while (1) {
    ssize_t n = read(fd, &ev, sizeof(ev));
    if (n == -1) {
      if (errno == EINTR)
        continue;
      perror("Ошибка чтения");
      break;
    }

    if (n != sizeof(ev)) {
      fprintf(stderr, "Неполное чтение события\n");
      continue;
    }

    // Обрабатываем только события клавиш
    if (ev.type == EV_KEY && ev.value == 1) { // value=1 - нажатие клавиши
      printf("=== НАЖАТА КЛАВИША ===\n");
      printf("Скан-код: 0x%04x (%d)\n", ev.code, ev.code);
      printf("Код клавиши: %s\n", get_key_name(ev.code));
      printf("Время события: %ld сек %ld мкс\n", ev.time.tv_sec,
             ev.time.tv_usec);
      printf("Тип события: %d (EV_KEY)\n", ev.type);
      printf("Адрес структуры события: %p\n", (void *)&ev);
      printf("Размер структуры input_event: %lu байт\n", sizeof(ev));
      printf("\n");
    }
  }

  close(fd);
  return 0;
}
