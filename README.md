# Overview 🤓
Репозиторий представляет из себя примитивную реализацию препроцессора языка СИ с поддержкой только директивы `#define`. На вход программе подаётся файл с кодом на языке СИ, на выходе мы получаем код с готовыми макроподстановками. При запуске программы обрабатываются *все* файлы из директории `data\originals`, готовые файлы загружаются в директорию `data\preprocessed`.

# It works!🎉
В текущей реализации обрабатываются следующие случаи:
- _object-like_ макросы
- _function-like_ макросы
- Многострочные макросы
- Whitespace в объявлении
- Рекурсивные подстановки
- Игнорирование страчных вхождений
- Использование последнего определённого макроса
- Поддержка `...` и `__VA_ARGS__`
- Не последовательный порядок подстановки
- Определения в любом месте кода

Все случаи можно найти в тестах.

# Реализация🛠
## Считывание файлов
Так как файлы с кодом могут быть достаточно большие, то если хранить весь текст из файла целиком, то мы можем упереться в лимит памяти, поэтому в реализации используются символьные потоки: мы последовательно обрабатываем каждую строку из входного файла и записывем в выходной. Это не безопасно с точки зрения файловой системы, зато быстро и не ограничено в памяти.

_P.S. В коде не обрабатываются исключения, связанные с чтением файлов, но это в планах :)_

## Хранение макросов
Так как у нас существует 2 вида макросов: объекты и функции, то для хранения мы используем словарь из имени макроса в его подстановку, где сам макрос описывает абстрактный класс `MasterToken` в файле _token.h_. Это обеспечивает доступ к элементам за O(1) и корректную подстановку в случае с функцией.

## Порядок подстановки
Сначала был реализован inline подход к подстановкам, то есть к моменту обработки строки мы используем актуальную на данный момент словарь. Однако согласно спецификации, использоваться долхно последнее переопределённое значение.

## Некорректные данные
Так как не совсем описано, что должно происходить в случае некорректных макросов, то в ходе исполнения, при обнаружении макроса, который определён некорректно (например, не реализован аргумент функции или ... стоит не в конце), то мы просто прекращаем обрабатывать макрос, не добавляя его в словарь и не возвращая в файл.

_P.S. Ну и так как не определено поведение при нахожении неподдерживаемого макроса, то и поведение программы __пока__ не определено :)_

# TODO📚
- Подстановка функций без параметров
Согласно спецификации, при использовании `g` из примера ниже, мы должны принимать её за функцию с 1 параметром идентично `f`.

_Пример:_
```cpp
#define f(a) ((a) + 1)
#define g f
```
- Обработка комментариев.
- \# и \##
- #undef
