# Шаблон проекта программы для МК

## Требуемые инструментальные средства:

- arm-none-eabi-gcc
- gdb-multiarch (arm-none-eabi-gdb)
- make (make + ninja-build)
- cmake
- openocd
- doxygen (для документирования программы)


## Рекомендуемый редактор (IDE)

Visual Studio Code c плагинами ():

- C/C++;
- CMake Tools;
- Cortex-Debug.

## Настройка и сборка проекта

### Настройка

В терминале, открытом в корневой папке проекта:

`cmake -B build`

### Сборка проекта

В том же терминале, в котором выполнялась настройка:

`cmake --build build`
