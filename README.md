# Сборка
1. git clone -b 'hw6' --single-branch https://github.com/etrehappy/ue_prj_a.git
2. Скачать [архив (249 MB)](https://drive.google.com/file/d/1jqInCOkrUjLgV-TSPY3i7e7eu1KmhRHx/view?usp=sharing) и распаковать в .\ue_prj_a\ 
3. Выполнить Generate Visual Studio project files для  ".\ue_prj_a\Project_A\Project_A.uproject"
4. Открыть ".\ue_prj_a\Project_A\Project_A.sln"
5. Build Project_A

Важно: на github загружен только минимум, в Project_A.Build.cs можно закомментировать лишние пути, а в Project_A.uproject — отлкючить лишние плагины.

# Запуск проекта
Проект разделён на клиент и сервер, поэтому есть bat-файлы для запуска.
1. В файлах Editor_StartGame и Editor_StartTwoServers исправить пути до UnrealEditor.exe
2. Запустить серверы через Editor_StartTwoServers.bat
3. Запустить клиент через Editor_StartGame.bat

[Здесь](https://github.com/etrehappy/ue_prj_a/blob/hw4/img_for_readme/start_game.jpg) можно посмотреть скриншоты того, как должно запускаться. 



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
<br> [ВИДЕО](https://drive.google.com/file/d/1J1mpeU7HeQCFdDL3ApcI94eRGixlNGR9/view?usp=sharing)

- [x] Создан базовый класс персонажа ([BaseCharacter.h](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Public/Character/BaseCharacter.h), [BaseCharacter.cpp](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Private/Character/BaseCharacter.cpp)) на основе ACharacter, чтобы не продумывать самостоятельно физику движения (например, падение, когда нет поверхности).
- [x] Создан класс персонажа игрока на основе базового класса ([NetPlayerCharacter.h](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Public/Character/NetPlayerCharacter.h), [NetPlayerCharacter.cpp](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Private/Character/NetPlayerCharacter.cpp))
- [x] Настроена система управления персонажем ([CustomLocomotionComponent.h](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Public/Character/CustomLocomotionComponent.h), [CustomLocomotionComponent.cpp](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Private/Character/CustomLocomotionComponent.cpp)):
    - движение вперёд\назад\влево\вправо
    - вращение камерой      


<b> Дополнительно</b>
- [x]  Добавлены собственные классы Gameplay Framework:
    - [GameInstanceBase.h](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Public/GameInstanceBase.h),
    - [CustomPlayerController.h](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Public/Character/CustomPlayerController.h),
    - [GeneralGameMode.h](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Public/GeneralGameMode.h), который является родителем для [HubGameMode.h](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Public/HubGameMode.h) (на сервере для входа в мир) и [WorldGameMode.h](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Public/WorldGameMode.h) (на сервере для основного игрового мира).
    
    
- [x] Добавлен [PlayerCameraComponent.h](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Public/Character/PlayerCameraComponent.h)
- [x] Создан базовый класс Pawn, который может понадобиться в дальнейшем для NPC ([BasePawn.h](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Public/BasePawn.h), [BasePawn.cpp](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Private/BasePawn.cpp))
- [x] Для управления настроены InputActions ([IA_MoveDefault, IA_LookDefault](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Content/Project_A_Root/Character/Input/Actions)) и InputMappingContext ([IMC_DefaultInput](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Content/Project_A_Root/Character/Input))
- [x] Создан BP-класс персонажа [BP_PlayerCharacter](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacter.uasset), унаследованный от [NetPlayerCharacter.h](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Public/Character/NetPlayerCharacter.h)
- [x] Для практики добавлен simple-тест [FInputActionsAreSetTest](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A_Tests/Private/InputTest.cpp), чтобы проверить наличие установленных InputAction и InputMappingContext.

**Как сделано**:
1. Управление сделано через C++ по схеме шаблона ThirdPerson
    - В [CustomPlayerController.h](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Public/Character/CustomPlayerController.h) добавляется MappingContext черезе интерфейс редактора. 
    - В персонаже [BP_PlayerCharacter](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacter.uasset) добавляются Actions, на которые нужно реагировать. 
    - Вся логика, связанная с движением (X\Y\прыжки\спринт\приседание) вынесена в [CustomLocomotionComponent.h](https://github.com/etrehappy/ue_prj_a/blob/hw4/Project_A/Source/Project_A/Public/Character/CustomLocomotionComponent.h)
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw4/img_for_readme/locomotion_bp_move.jpg" alt="Inputs" width="500" height="211">   
        </div>
        <br>
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw4/img_for_readme/locomotion_bp_playercontroller.jpg" alt="IMC" width="500" height="154">   
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

<div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw5/img_for_readme/animation_abp_animgraph.jpg" alt="AnimGraph" width="500" height="229">   
</div>
<div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw5/img_for_readme/animation_abp_eventgraph.jpg" alt="EventGraph" width="500" height="259">   
</div>
<div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw5/img_for_readme/animation_chooser_table.jpg" alt="ChooserTable" width="500" height="104">   
</div>

<b> Дополнительно</b>
- [x]  В TickComponent теперь вызывается [UpdateDirection](https://github.com/etrehappy/ue_prj_a/blob/hw5/Project_A/Source/Project_A/Public/Character/CustomLocomotionComponent.h). На основе данных о направлении работают анимация и ограничение Sprint (доступен только при движении вперёд в I и II четвертях). 
<div style="text-align: center;">
            <img src="./img_for_readme/animation_forwarddot.jpg" alt="forwarddot" width="500" height="397">   
</div>
<div style="text-align: center;">
            <img src="./img_for_readme/animation_rightdot.jpg" alt="rightdot" width="500" height="328">   
</div>


**Как сделано**:
1. Для анимации персонажа используются: 
    - персонаж с root-motion — ассет из [fab](https://www.fab.com/listings/94fd60a2-5659-4fc4-af1d-a8cdd2681c2e);
    - анимации из [Game Animation Sample](https://www.fab.com/listings/880e319a-a59e-4ed2-b268-b32dac7fa016)    
2. В [компоненте движения](https://github.com/etrehappy/ue_prj_a/blob/hw5/Project_A/Source/Project_A/Public/Character/CustomLocomotionComponent.h) есть структура [FCharacterMovementStruct](https://github.com/etrehappy/ue_prj_a/blob/hw5/Project_A/Source/Project_A/Public/Character/CharacterStateEnums.h), которая хранит разные состояния движения (направление, на земле\в воздухе\sprint\crouch и т.п.). В [ABP_SimpleCharacter](https://github.com/etrehappy/ue_prj_a/blob/hw5/Project_A/Content/Project_A_Root/Character/Player/Animation/ABP_SimpleCharacter.uasset) получаем данные из этой структуры, чтобы использовать в таблице [CHT_PSD_SimpleCharacter_D](https://github.com/etrehappy/ue_prj_a/blob/hw5/Project_A/Content/Project_A_Root/Character/Player/Animation/CHT_PSD_SimpleCharacter_D.uasset)
3. В [PSS_SimpleCharacter](https://github.com/etrehappy/ue_prj_a/blob/hw5/Project_A/Content/Project_A_Root/Character/Player/Animation/CHT_PSD_SimpleCharacter_D.uasset) добавлены Velocity Channels и Position Channel.
4. В [BP_PlayerCharacter](https://github.com/etrehappy/ue_prj_a/blob/hw5/Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacter.uasset) добавлены компоненты MotionWarping и CharacterTrajectory.
5. Для нормализации добавлен [PNS_Locomotion](https://github.com/etrehappy/ue_prj_a/blob/hw5/Project_A/Content/Project_A_Root/Character/Player/Animation/PNS_Locomotion.uasset)
</details>


<details><summary>Задание 6. Компоненты <p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ](https://drive.google.com/file/d/1wP6-dkLnKeS9wPw5FU9G9_KXHQz1ue99/view?usp=drive_link)

- [x] Создан плагин оружия [WeaponSystemPlugin](./Project_A/Plugins/WeaponSystemPlugin/WeaponSystemPlugin.uplugin) и компонент владения оружием [WeaponComponent](./Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponComponent.h). По нажатию на клавишу C появляется оружие (static mesh) в руке персонажа.
- [x] Создан модуль [Enemy](./Project_A/Source/Enemy/Public/EnemyModule.h). По нажатию на тестовую клавишу Q появляется враг.

Дополнительно: 
- Добавлена фабрика для оружия — [WeaponFactory](./Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponFactory.h) (часть плагина WeaponSystemPlugin)
- Добавлен компонент оружия — [WeaponComponent](./Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponComponent.h) (часть плагина WeaponSystemPlugin)
- Добавлен [WeaponDataAsset](./Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponDataAsset.h) и описан один тестовый [DataAsset](./Project_A/Content/Project_A_Root/Weapons/DA_Weapons.uasset).
- Добавлен [компонент сражения](./Project_A/Source/Project_A/Public/Character/BattleComponent.h) (часть основного проекта)


**Как сделано**:
1. Оружие
    - В [CustomPlayerController](./Project_A/Source/Project_A/Public/Character/CustomPlayerController.h)  добавляется MappingContext черезе интерфейс редактора.
    - [Компонент сражения](./Project_A/Source/Project_A/Public/Character/BattleComponent.h) на данном этапе используется для связывания с InputAction.
    -  [Персонаж](./Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacter.uasset) (в BP-редакторе) реализует Event OnEquipWeapon(), который будет вызвать метод [компонента оружия](./Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponComponent.h) UWeaponComponent::EquipWeaponbyTag. Теги добавляются в [WeaponSystemPluginGameplayTags.ini](./Project_A/Plugins/WeaponSystemPlugin/Config/Tags/WeaponSystemPluginGameplayTags.ini)
    - [Компонента оружия](./Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponComponent.h) через метод [фабирики](./Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponFactory.h) UWeaponFactory::SpawnWeaponByTag создаёт оружие и крепет его к сокету, указанному в BP-редакторе.
    - [Фабрика](./Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponFactory.h) использует статический метод для загрузки всех WeaponDataAsset, далее проверяет совпадение по тегу и выполняет SpawnActor< AWeaponBase> и настраивает параметры оружия.
    - [Плагин оружия](./Project_A/Plugins/WeaponSystemPlugin/WeaponSystemPlugin.uplugin) добавляет PrimaryAssetType в [WeaponSystemPlugin.cpp](./Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Private/WeaponSystemPlugin.cpp)

2. Призыв врага
    - [DataTable](.Project_A/Content/Project_A_Root/Character/Npc/Enemy/EnemyDataTable.uasset) заполняется в редакторе по структуре [FEnemyRow](./Project_A/Source/Enemy/Public/EnemyRow.h).    
    - В [BP_PlayerCharacterController](./Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacterController.uasset) через Debug-клавишу Q вызывается Server_SpawnEnemyInFront (это DevelopmentOnly-метод).
    - Server_SpawnEnemyInFront создаёт временную [EnemyFactory](./Project_A/Source/Enemy/Public/EnemyFactory.h) и вызывает её метод SpawnEnemyByTag. Теги добавляются в  [EnemyTags.ini](./Project_A/Config/Tags/EnemyTags.ini)
</details>
