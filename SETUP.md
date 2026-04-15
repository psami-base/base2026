### Действия перед сдачей заданий

Для успешного локального тестирования своих решений вам необходимо установить следующие пакеты (инструкции для debian-based систем):

* git (`sudo apt install git`)

* g++ (`sudo apt install build-essential`)

* cmake (`sudo apt install cmake`)

* clang-tidy (`sudo apt install clang-tidy`)

* clang-format (`sudo apt install clang-format`)

Клонируйте данный репозиторий на свой компьютер (`git clone https://gitlab.com/ibr11/base_2024.git`). В его корне создайте папку `build` (`mkdir build`).

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
make <название задачи>_test
```

В случае успеха вы увидите примерно следующее:

```
-- The C compiler identification is GNU 13.3.0
-- The CXX compiler identification is GNU 13.3.0
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
-- Configuring done (0.2s)
-- Generating done (0.0s)
-- Build files have been written to: /home/ibr/Documents/cpp_projects/build

[ 33%] Building CXX object sum/CMakeFiles/sum_test.dir/sum.cpp.o
[ 66%] Building CXX object sum/CMakeFiles/sum_test.dir/sum_test.cpp.o
[100%] Linking CXX executable ../sum_test
[100%] Built target sum_test
```
Решение собралось под всеми флагами, это уже что-то.

4. Далее запускаем тестирование.

```
./<название задачи>_test
```

```
===============================================================================
All tests passed (18 assertions in 6 test cases)
``` 

Все тесты пройдены, ого. Вы что, задачу решили? (нет, все только начинается)

5. Начинаем проверку кодстайла.

Из папки `build`:
```
../codestyle_checker.sh ../<название задачи>
```

Либо из корневой папки:
```
./codestyle_checker.sh ./<название задачи>
```

```
[1/2] Processing file /home/ibr/Documents/cpp_projects/sum/sum.cpp.
76 warnings generated.
[2/2] Processing file /home/ibr/Documents/cpp_projects/sum/sum_test.cpp.
10846 warnings generated.
Suppressed 10846 warnings (10846 in non-user code).
Use -header-filter=.* to display errors from all non-system headers. Use -system-headers to display errors from system headers as well.
```

Видим только какие-то предупреждения, не обращаем на них внимания. Если есть ошибки стиля вы увидите строки `error`. Их надо будет исправить.

### Тестирование в контесте

Для автоматической проверки решений мы используем систему Я.Контест.

Откройте страницу с задачей в Я.Контест. Внимательно прочтите условие, что необходимо прислать в систему - текстовый файл или zip архив.

В случае, если в качестве решения ожидается набор файлов с кодом, присылайте `zip` архив. Не `rar`, не `7z` и даже не `tar.gz`. Ваш код должен располагаться **в корне** архива, то есть сжатию должен быть подвергнут набор файлов, а не папка, в которой содержится код. Название архива должно содержать только латинские буквы (без пробелов и точек посередине имени). Если тестирующая система не может найти какой-то из файлов или просто прекращает работу без проверки кода, скорее всего, не выполнен какой-либо из пунктов выше, или названия и структура присланных файлов не удовлетворяет требованиям условия.

После отправки решения над вашим кодом производятся действия описанные в предыдущих пунктах. При возникновении проблем на этапе сборки и проверки стиля вы получите ошибку `CE`. Если код не проходит тесты - `RE` или `WA`. Недостаточно эффективные решения падают с вердиктом `TL` или `ML`. В случае успеха вы получите вердикт `OK` или `AC` в зависимости от того, подразумевает ли данная задача ручную проверку кода семинаристом.
