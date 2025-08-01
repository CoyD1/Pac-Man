Pac-Man
Реализация игры Pac-Man в консольном приложении

Структура проекта

```
PAC-MAN/
├── third_party/ # Зависимости проекта
│ └── PDCurses/ # Библиотека PDCurses
├── include/ # Заголовочные файлы
│ ├── Game.hpp # Управление игровым процессом
│ ├── Ghost.hpp # ИИ призраков
│ ├── Menu.hpp # Система меню
├── src/ # Исходный код
│ ├── main.cpp
│ ├── Game.cpp # Реализация игрового цикла
│ ├── Ghost.cpp # Поведение призраков
│ └── Menu.cpp # Навигация по меню
├── assets/ # Ресурсы игры
│ └── levels/ # Файлы уровней
│ ├── level_one.txt
│ ├── level_two.txt
│ └── ...
├── build/ # Каталог для сборки
├── CMakeLists.txt # Конфигурация CMake (основной)
└── README.md # Этот файл
```

Системные требования

- **Операционная система:** Windows, Linux или macOS
- **Компилятор C++:** GCC (Linux/macOS) или MinGW (Windows)
- **Библиотека:** PDCurses
- **Дополнительно:** CMake (версия 3.10+)

## Сборка и запуск

### Установка зависимостей

**Windows:**

1. Скачайте PDCurses: [https://github.com/wmcbrine/PDCurses](https://github.com/wmcbrine/PDCurses)
2. Скопируйте файлы `pdcurses.a` и `curses.h` в папку `third_party/PDCurses/`

### Сборка проекта

1. Перейдите в папку src проекта
2. Выполните команду:
   **Windows (MinGW):**  
   `g++ -I../Include *.cpp -lpdcurses -o game.exe`

### Запуск игры

**Windows:**  
`.\src\game.exe`

## Управление в игре

| Действие        | Игрок 1   | Игрок 2   |
| --------------- | --------- | --------- |
| Движение вверх  | W         | Стрелка ↑ |
| Движение вниз   | S         | Стрелка ↓ |
| Движение влево  | A         | Стрелка ← |
| Движение вправо | D         | Стрелка → |
| Пауза           | Esc       | Esc       |
| Выход из игры   | q         | q         |
| Выбор уровня    | Цифры 0-9 | Цифры 0-9 |

## Формат файлов уровней

Файлы уровней хранятся в `assets/levels/`. Пример (`level_one.txt`):

```
############################
#S...........##...........T#
#.####.#####.##.#####.####.#
#o####.#####.##.#####.####o#
#.####.#####.##.#####.####.#
#..........................#
#.####.##.########.##.####.#
#.####.##.########.##.####.#
#......##....##....##......#
######.##### ## #####.######
     #.##### ## #####.#
     #.##          ##.#
     #.## ###  ### ##.#
######.## #G    G# ##.######
      .   #G    G#   .
######.## ######## ##.######
     #.##          ##.#
     #.## ######## ##.#
     #.## ######## ##.#
######.## ######## ##.######
#............##............#
#.####.#####.##.#####.####.#
#o..##................##..o#
###.##.##.########.##.##.###
#......##....##....##......#
#.##########.##.##########.#
#..........................#
############################
```

`S` - Место спавна первого игрока
`T` - Место спавна второго игрока
**Обозначения символов во время игрового процесса:**

- `#` - Стена (синий)
- `.` - Точка (белый)
- `o` - Энерджайзер (белый)
- `P` - Pac-Man первый игрок(желтый)
- `p` - Pac-Man второй игрок(розовый)
- `G` - Призрак (разные цвета)
- ` ` - Пустое пространство
