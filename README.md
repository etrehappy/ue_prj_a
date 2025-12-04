# Сборка
1. git clone -b 'hw_5' --single-branch https://github.com/etrehappy/ue_prj_a.git
2. Выполнить Generate Visual Studio project files для  ".\ue_prj_a\Project_A\Project_A.uproject"
3. Открыть ".\ue_prj_a\Project_A\Project_A.sln"
4. Build Project_A
5. Открыть проект в UE

Важно: на github загружен только минимум, в Project_A.Build.cs можно закомментировать лишние пути, а в Project_A.uproject — отлкючить лишние плагины.

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
- [x]  В файле [TrainingTest.cpp](https://github.com/etrehappy/ue_prj_a/blob/hw2/Project_A/Source/Project_A/Tests/Test_0/TrainingTest.cpp) есть 4 теста: 
    - открытие карты — FOpenLevelTest — с использованием Latent-команд;
    - проверка позиции ассета — FGetBpActorPropertiesFromEditorTest — по заданию;
    - проверка контекста текущего мира — FShowWorldTypeTest (просто для практики, смысловой нагрузки нет)
    - отображение в логах всех контекстов — FShowWorldContextsTest (просто для практики, смысловой нагрузки нет)

<b> Дополнительно </b>
- [x]  Разобран синтаксис Spec-тестов
- [x] В [TrainingTest.spec.cpp](https://github.com/etrehappy/ue_prj_a/blob/hw2/Project_A/Source/Project_A/Tests/Test_0/TrainingTest.spec.cpp) добавлено несколько тестов для практики.
    <div style="text-align: center;">
        <img src="https://github.com/etrehappy/ue_prj_a/blob/hw2/img_for_readme/training_test_results.jpg" alt="Окружение" width="500" height="85
        ">   
    </div>

</details>

<details><summary>Задание 4. Создание персонажа<p></p></summary>

**Что сделано**:

<b>Основное</b>

- [x] Создан базовый класс персонажа ([BaseCharacter.h](./Project_A/Source/Project_A/Public/Character/BaseCharacter.h), [BaseCharacter.cpp](./Project_A/Source/Project_A/Private/Character/BaseCharacter.cpp)) на основе ACharacter, чтобы не продумывать самостоятельно физику движения (например, падение, когда нет поверхности).
- [x] Создан класс персонажа игрока на основе базового класса ([PlayerCharacter.h](./Project_A/Source/Project_A/Public/Character/PlayerCharacter.h), [PlayerCharacter.cpp](./Project_A/Source/Project_A/Private/Character/PlayerCharacter.cpp))
- [x] Настроена система управления персонажем ([CustomLocomotionComponent.h](./Project_A/Source/Project_A/Public/Character/CustomLocomotionComponent.h), [CustomLocomotionComponent.cpp](./Project_A/Source/Project_A/Private/Character/CustomLocomotionComponent.cpp)):
    - движение вперёд\назад\влево\вправо
    - вращение камерой
        [![Видео превью](https://drive.google.com/drive-viewer/AKGpihbudHO09CYJGqhP6e8ljxNxv-SnuUwiv2WmZ4Zxp1RVBrQ46OJ7_btCmHvqr81Eop3l2-yqvXz9m-mNZLh2aZYe_-7l8ME1Dw=w1920-h1080-k-rw-v1-pd)](https://drive.google.com/file/d/1J1mpeU7HeQCFdDL3ApcI94eRGixlNGR9/view?usp=sharing)


<b> Дополнительно</b>
- [x]  Добавлены собственные базовые элементы ([CustomPlayerController.h](./Project_A/Source/Project_A/Public/Character/CustomPlayerController.h), [GeneralGameMode.h](./Project_A/Source/Project_A/Public/Character/GeneralGameMode.h), [PlayerCameraComponent.h](./Project_A/Source/Project_A/Public/Character/PlayerCameraComponent.h))
- [x] Создан базовый класс Pawn, который может понадобиться в дальнейшем для NPC ([BasePawn.h](./Project_A/Source/Project_A/Public/BasePawn.h), [BasePawn.cpp](./Project_A/Source/Project_A/Private/BasePawn.cpp))
- [x] Для управления настроены InputActions ([IA_MoveDefault, IA_LookDefault](./Project_A/Content/Project_A_Root/Character/Input/Actions)) и InputMappingContext ([IMC_DefaultInput](./Project_A/Content/Project_A_Root/Character/Input))
- [x] Создан BP-класс персонажа [BP_PlayerCharacter](./Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacter.uasset), унаследованный от [PlayerCharacter.h](./Project_A/Source/Project_A/Public/Character/PlayerCharacter.h)
- [x] Добавлен simple-тест [FInputActionsAreSetTest](./Project_A/Source/Project_A_Tests/Private/InputTest.cpp) для проверки класса [BP_PlayerCharacter](./Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacter.uasset) на наличие установленных InputAction и InputMappingContext.

**Как сделано**:
1. Управление
    - В [CustomLocomotionComponent.h](./Project_A/Source/Project_A/Public/Character/CustomLocomotionComponent.h) добавлены массив <i>InputMappingContexs</i> и указатель <i>InputActionMove</i>, которые заполняются в настройках BP-класса. В методе <i>UCustomLocomotionComponent :: Initialize()</i> связываются <i>InputActionMove</i> с обработчиком движения <i>UCustomLocomotionComponent :: HandleMove()</i>.   Было бы легче всё настроить через BP, но такой подход выбран намеренно для практики в C++ и баланса между BP\C++. 
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw4/img_for_readme/locomotion_bp_move.jpg" alt="Движение" width="500" height="115">   
        </div>
    

</details>

<details><summary>Задание 5. Анимации персонажа<p></p></summary>

**Что сделано**:

<b>Основное</b>

- [x] Настроены анимации через Motion Matching.
    - вперёд\назад\влево\вправо\под углом (ходьба и бег)
    - прыжок\падение
    - sprint (кл. LShift)
    - crouch (кл. LCtrl)
    - idle (stand и crouch)

[ВИДЕО](https://drive.google.com/file/d/1NT0hmf1TbyDaenUUwnwRgEzPyXzf4z-6/view) демонстрация

<div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw5/img_for_readme/animation_abp_animgraph.jpg" alt="AnimGraph" width="500" height="229"> 
        <br>
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw5/img_for_readme/animation_abp_eventgraph.jpg" alt="EventGraph" width="500" height="259">   
        <br><br>
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw5/img_for_readme/animation_chooser_table.jpg" alt="ChooserTable" width="500" height="116">   
</div>

**Как сделано**:
1. Для анимации персонажа используются: 
    - персонаж с root-motion — ассет из [fab](https://www.fab.com/listings/94fd60a2-5659-4fc4-af1d-a8cdd2681c2e);
    - анимации из [Game Animation Sample](https://www.fab.com/listings/880e319a-a59e-4ed2-b268-b32dac7fa016)    
2. В [ABP_SimpleCharacter](./Project_A/Content/Project_A_Root/Character/Player/Animation/ABP_SimpleCharacter.uasset) обновляются переменные, которые используются в таблице [CHT_PSD_SimpleCharacter_D](./Project_A/Content/Project_A_Root/Character/Player/Animation/CHT_PSD_SimpleCharacter_D.uasset)
3. В [PSS_SimpleCharacter](./Project_A/Content/Project_A_Root/Character/Player/Animation/CHT_PSD_SimpleCharacter_D.uasset) добавлены Velocity Channels и Position Channel.
4. В [BP_PlayerCharacter](./Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacter.uasset) добавлены компоненты MotionWarping и CharacterTrajectory.
5. Добавлены состояния персонажа [CharacterMovementState](./Project_A/Source/Project_A/Public/CharacterStateEnums.h), которые меняются через [компонент движения](./Project_A/Source/Project_A/Public/Character/CustomLocomotionComponent.h)
6. Для нормализации добавлен [PNS_Locomotion](./Project_A/Content/Project_A_Root/Character/Player/Animation/PNS_Locomotion.uasset)
</details>
