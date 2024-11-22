### Действия перед сдачей заданий

Для успешного локального тестирования своих решений вам необходимо установить следующие пакеты (инструкции для debian-based систем):

* git (`sudo apt install git`)

* g++ (`sudo apt install build-essential`)

* cmake (`sudo apt install cmake`)

* clang-tidy (`sudo apt install clang-tidy`)

* clang-format (`sudo apt install clang-format`)

Клонируйте данный репозиторий на свой компьютер (`git clone https://gitlab.com/ibr11/cpp_psami_base_2023.git`). В его корне создайте папку `build` (`mkdir build`).

### Инструкция по сдаче задач

1. Убедитесь, что вы используете последнюю версию тестов и линтера. Для этого выполните:
```
git stash
git pull
git stash pop
```

2. Реализуйте все, что требуется в задании.

3. После этого перейдите в директорию `build` (`cd build`) и скомпилируйте ваше решение и тесты:

```
cmake ..
make <название задачи>_public_test
```

В случае успеха вы увидите примерно следующее:

```
-- The C compiler identification is GNU 9.3.0
-- The CXX compiler identification is GNU 9.3.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found GTest: /usr/lib/x86_64-linux-gnu/libgtest.a  
-- Configuring done
-- Generating done
-- Build files have been written to: /home/vsm/Desktop/OOP_2021/public_testing/sum/build
Scanning dependencies of target sum_public_test
[ 33%] Building CXX object CMakeFiles/sum_public_test.dir/home/vsm/Desktop/cpp/sum/sum.cpp.o
[ 66%] Building CXX object CMakeFiles/sum_public_test.dir/sum_public_test.cpp.o
[100%] Linking CXX executable sum_public_test
[100%] Built target sum_public_test
```
Решение собралось под всеми флагами, это уже что-то.

4. Далее запускаем тестирование.

```
./<название задачи>_public_test
```

```
===============================================================================
All tests passed (18 assertions in 6 test cases)
``` 

Все тесты пройдены, ого. Вы что, задачу решили? (нет, все только начинается)

5. Начинаем проверку кодстайла.

```
cd .. (возвращаемся в корневую папку)
./codestyle_checker.sh <название задачи/папки с задачей>
```

```
81 warnings generated.
47141 warnings generated.
Suppressed 47141 warnings (47141 in non-user code).
Use -header-filter=.* to display errors from all non-system headers. Use -system-headers to display errors from system headers as well.
```

Видим только какие-то предупреждения, не обращаем на них внимания. Если есть ошибки стиля вы увидите строки `error`. Их надо будет исправить.

### Тестирование в контесте

Для автоматической проверки решений мы используем систему Я.Контест.

Откройте страницу с задачей в Я.Контест. Внимательно прочтите условие, что необходимо прислать в систему - текстовый файл или zip архив.

В случае, если в качестве решения ожидается набор файлов с кодом, присылайте `zip` архив. Не `rar`, не `7z` и даже не `tar.gz`. Ваш код должен располагаться **в корне** архива, то есть сжатию должен быть подвергнут набор файлов, а не папка, в которой содержится код. Название архива должно содержать только латинские буквы (без пробелов и точек посередине имени). Если тестирующая система не может найти какой-то из файлов или просто прекращает работу без проверки кода, скорее всего, не выполнен какой-либо из пунктов выше, или названия и структура присланных файлов не удовлетворяет требованиям условия.

После отправки решения над вашим кодом производятся действия описанные в предыдущих пунктах + вместо публичных тестов используются приватные. При возникновении проблем на этапе сборки и проверки стиля вы получите ошибку `CE`. Если код не проходит тесты - `RE` или `WA`. Недостаточно эффективные решения падают с вердиктом `TL` или `ML`. В случае успеха вы получите вердикт `OK` или `AC` в зависимости от того, подразумевает ли данная задача ручную проверку кода семинаристом.
