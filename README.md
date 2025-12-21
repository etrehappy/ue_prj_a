# Сборка
1. git clone -b 'hw4' --single-branch https://github.com/etrehappy/ue_prj_a.git
2. Выполнить Generate Visual Studio project files для  ".\ue_prj_a\Project_A\Project_A.uproject"
3. Открыть ".\ue_prj_a\Project_A\Project_A.sln"
4. Build Project_A

Важно: на github загружен только минимум, в Project_A.Build.cs можно закомментировать лишние пути, а в Project_A.uproject — отлкючить лишние плагины.

# Запуск проекта
Проект разделён на клиент и сервер, поэтому есть bat-файлы для запуска.
1. В файлах Editor_StartGame и Editor_StartTwoServers исправить пути до UnrealEditor.exe
2. Запустить серверы через Editor_StartTwoServers.bat
3. Запустить клиент через Editor_StartGame.bat

[Здесь](./img_for_readme/start_game.jpg) можно посмотреть скриншоты того, как должно запускаться. 
<br> Анимация персонажа не настроена.
<br> [ВИДЕО](https://drive.google.com/file/d/1J1mpeU7HeQCFdDL3ApcI94eRGixlNGR9/view?usp=sharing)

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
- [x]  В файле [TrainingTest.cpp](https://github.com/etrehappy/ue_prj_a/blob/hw2/Project_A/Source/Project_A/Tests/Test_0/TrainingTest.cpp) есть 3 теста: 
    - проверка позиции ассета — FGetBpActorPropertiesFromEditorTest — по заданию;
    - проверка контекста текущего мира — FShowWorldTypeTest (просто для практики)
    - отображение в логах всех контекстов — FShowWorldContextsTest (просто для практики)

<b> Дополнительно </b>
- [x]  Разобран синтаксис Spec-тестов
- [x] В [TrainingTest.spec.cpp](https://github.com/etrehappy/ue_prj_a/blob/hw2/Project_A/Source/Project_A/Tests/Test_0/TrainingTest.spec.cpp) добавлен 1 тест для практики.
    <div style="text-align: center;">
        <img src="https://github.com/etrehappy/ue_prj_a/blob/hw2/img_for_readme/training_test_results.jpg" alt="Окружение" width="500" height="85
        ">   
    </div>

</details>

<details><summary>Задание 4. Создание персонажа<p></p></summary>

**Что сделано**:

<b>Основное</b>

- [x] Создан базовый класс персонажа ([BaseCharacter.h](./Project_A/Source/Project_A/Public/Character/BaseCharacter.h), [BaseCharacter.cpp](./Project_A/Source/Project_A/Private/Character/BaseCharacter.cpp)) на основе ACharacter, чтобы не продумывать самостоятельно физику движения (например, падение, когда нет поверхности).
- [x] Создан класс персонажа игрока на основе базового класса ([NetPlayerCharacter.h](./Project_A/Source/Project_A/Public/Character/NetPlayerCharacter.h), [NetPlayerCharacter.cpp](./Project_A/Source/Project_A/Private/Character/NetPlayerCharacter.cpp))
- [x] Настроена система управления персонажем ([CustomLocomotionComponent.h](./Project_A/Source/Project_A/Public/Character/CustomLocomotionComponent.h), [CustomLocomotionComponent.cpp](./Project_A/Source/Project_A/Private/Character/CustomLocomotionComponent.cpp)):
    - движение вперёд\назад\влево\вправо
    - вращение камерой      


<b> Дополнительно</b>
- [x]  Добавлены собственные классы Gameplay Framework:
    - [GameInstanceBase.h](./Project_A/Source/Project_A/Public/GameInstanceBase.h),
    - [CustomPlayerController.h](./Project_A/Source/Project_A/Public/Character/CustomPlayerController.h),
    - [GeneralGameMode.h](./Project_A/Source/Project_A/Public/GeneralGameMode.h), который является родителем для [HubGameMode.h](./Project_A/Source/Project_A/Public/HubGameMode.h) (на сервере для входа в мир) и [WorldGameMode.h](./Project_A/Source/Project_A/Public/WorldGameMode.h) (на сервере для основного игрового мира).
    
    
- [x] Добавлен [PlayerCameraComponent.h](./Project_A/Source/Project_A/Public/Character/PlayerCameraComponent.h)
- [x] Создан базовый класс Pawn, который может понадобиться в дальнейшем для NPC ([BasePawn.h](./Project_A/Source/Project_A/Public/BasePawn.h), [BasePawn.cpp](./Project_A/Source/Project_A/Private/BasePawn.cpp))
- [x] Для управления настроены InputActions ([IA_MoveDefault, IA_LookDefault](./Project_A/Content/Project_A_Root/Character/Input/Actions)) и InputMappingContext ([IMC_DefaultInput](./Project_A/Content/Project_A_Root/Character/Input))
- [x] Создан BP-класс персонажа [BP_PlayerCharacter](./Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacter.uasset), унаследованный от [NetPlayerCharacter.h](./Project_A/Source/Project_A/Public/Character/NetPlayerCharacter.h)
- [x] Для практики добавлен simple-тест [FInputActionsAreSetTest](./Project_A/Source/Project_A_Tests/Private/InputTest.cpp), чтобы проверить наличие установленных InputAction и InputMappingContext.

**Как сделано**:
1. Управление сделано через C++ по схеме шаблона ThirdPerson
    - В [CustomPlayerController.h](./Project_A/Source/Project_A/Public/Character/CustomPlayerController.h) добавляется MappingContext черезе интерфейс редактора. 
    - В персонаже [BP_PlayerCharacter](./Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacter.uasset) добавляются Actions, на которые нужно реагировать. 
    - Вся логика, связанная с движением (X\Y\прыжки\спринт\приседание) вынесена в [CustomLocomotionComponent.h](./Project_A/Source/Project_A/Public/Character/CustomLocomotionComponent.h)
        <div style="text-align: center;">
            <img src="./img_for_readme/locomotion_bp_move.jpg" alt="Inputs" width="500" height="211">   
        </div>
        <br>
        <div style="text-align: center;">
            <img src="./img_for_readme/locomotion_bp_playercontroller.jpg" alt="IMC" width="500" height="154">   
        </div>
    

</details>
