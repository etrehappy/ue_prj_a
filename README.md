# Сборка
1. git clone -b 'v_2' --single-branch https://github.com/etrehappy/ue_prj_a.git
2. Выполнить Generate Visual Studio project files для  ".\ue_prj_a\Project_A\Project_A.uproject"
3. Открыть ".\ue_prj_a\Project_A\Project_A.sln"
4. Build Project_A
5. Открыть проект в UE

# Что и как сделано
<details><summary>Задание 1. Создание проекта<p></p></summary>

**Что сделано**:
<b>Основное</b>
- [x]  Установить
    - Git
    - Visual Studio
- [x] Сделать fork репозиторий [Unreal Engine](https://github.com/EpicGames/UnrealEngine)
- [x] Скачать Unreal Engine.
- [x] Собрать Unreal Engine (в том числе и Dedicated server).
- [x] Создать новый проект, используя шаблон Third Person.
- [x] Создать репозиторий для своего проекта (GitHub/GitLab).
- [x] Поместить свой проект в репозиторий.


<b>Дополнительное</b>

 - подготовлен [Vision](https://github.com/etrehappy/ue_prj_a/wiki/Vision) (как я его понимаю)
 - подготовлено окружение для дальнейшей работы (предварительный вариант)  
 Важно: на github загружен только минимум
    <div style="text-align: center;">
        <img src="https://github.com/etrehappy/ue_prj_a/blob/hw1/img_for_readme/landscape_1.jpg" alt="Окружение" width="500" height="302">
        <p></p>
        <img src="https://github.com/etrehappy/ue_prj_a/blob/hw1/img_for_readme/map_1.jpg" alt="Карта" width="500" height="348">    
    </div>
    <br>
- выбраны ассеты для [игроков](https://www.fab.com/listings/94fd60a2-5659-4fc4-af1d-a8cdd2681c2e) и врагов ( [гоблины](https://www.fab.com/listings/0c7313ce-2bf8-4987-848b-6b38dbc38ee5) , [животные](https://www.fab.com/listings/8e208ab4-667d-4a86-870a-f0c3e9b0e080) )

 - подобраны главные игровые объекты — кристаллы и обелиск

<div style="text-align: center;">
    <img src="https://github.com/etrehappy/ue_prj_a/blob/hw1/img_for_readme/ex_crystal_obelisk.gif" alt="Окружение" width="302" height="720">
</div>

</details>


<details><summary>Задание 2. Инструменты тестирования<p></p></summary>

**Что сделано**:

<b>Основное</b>

- [x]  Разобраны автотесты.
- [x]  В файле [TrainingTest.cpp](https://github.com/etrehappy/ue_prj_a/blob/v_2/Project_A/Source/Project_A/Tests/Test_0/TrainingTest.cpp) есть 4 теста: 
    - открытие карты — FOpenLevelTest — с использованием Latent-команд;
    - проверка позиции ассета — FGetBpActorPropertiesFromEditorTest — по заданию;
    - проверка контекста текущего мира — FShowWorldTypeTest (просто для практики, смысловой нагрузки нет)
    - отображение в логах всех контекстов — FShowWorldContextsTest (просто для практики, смысловой нагрузки нет)

<b> Дополнительно </b>
- [x]  Разобран синтаксис Spec-тестов
- [x] В [TrainingTest.spec.cpp](https://github.com/etrehappy/ue_prj_a/blob/v_2/Project_A/Source/Project_A/Tests/Test_0/TrainingTest.spec.cpp) добавлено несколько тестов для практики.
    <div style="text-align: center;">
        <img src="./img_for_readme/training_test_results.jpg" alt="Окружение" width="500" height="85
        ">   
    </div>

</details>