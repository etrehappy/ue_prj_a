# Сборка
1. git clone -b 'hw19' --single-branch https://github.com/etrehappy/ue_prj_a.git
2. Скачать [архив (1,2 Гб)]() и распаковать с заменой в .\ue_prj_a\ 
3. Выполнить Generate Visual Studio project files для  ".\ue_prj_a\Project_A\Project_A.uproject"
4. Открыть ".\ue_prj_a\Project_A\Project_A.sln"
5. Build Project_A

Важно: на github загружен только минимум, в Project_A.Build.cs можно закомментировать лишние пути, а в Project_A.uproject — отлкючить лишние плагины.

# Запуск проекта
Проект разделён на клиент и сервер, поэтому есть bat-файлы для запуска.
1. В файлах Editor_StartGame и Editor_StartTwoServers исправить пути до UnrealEditor.exe
2. Запустить серверы через Editor_StartTwoServers.bat
3. Запустить клиент через Editor_StartGame.bat
4. Подключиться к лобби через Play via Lobby





# Что и как сделано
<details><summary id="task1">Задание 1. Создание проекта<p></p></summary>

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


<details><summary id="task2">Задание 2. Инструменты тестирования<p></p></summary>

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

<details><summary id="task4">Задание 4. Создание персонажа<p></p></summary>

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


<details><summary id="task5">Задание 5. Анимации персонажа<p></p></summary>

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
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw5/img_for_readme/animation_forwarddot.jpg" alt="forwarddot" width="500" height="397">   
</div>
<div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw5/img_for_readme/animation_rightdot.jpg" alt="rightdot" width="500" height="328">   
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


<details><summary id="task6">Задание 6. Создание модуля и плагина <p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ](https://drive.google.com/file/d/1wP6-dkLnKeS9wPw5FU9G9_KXHQz1ue99/view?usp=drive_link)

- [x] Создан плагин оружия [WeaponSystemPlugin](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Plugins/WeaponSystemPlugin/WeaponSystemPlugin.uplugin) и компонент владения оружием [WeaponComponent](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponComponent.h). По нажатию на клавишу C появляется оружие (static mesh) в руке персонажа.
- [x] Создан модуль [Enemy](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Source/Enemy/Public/EnemyModule.h). По нажатию на тестовую клавишу Q появляется враг.

Дополнительно: 
- Добавлена фабрика для оружия — [WeaponFactory](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponFactory.h) (часть плагина WeaponSystemPlugin)
- Добавлен компонент оружия — [WeaponComponent](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponComponent.h) (часть плагина WeaponSystemPlugin)
- Добавлен [WeaponDataAsset](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponDataAsset.h) и описан один тестовый [DataAsset](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Content/Project_A_Root/Weapons/DA_Weapons.uasset).
- Добавлен [компонент сражения](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Source/Project_A/Public/Character/BattleComponent.h) (часть основного проекта)


**Как сделано**:
1. Оружие
    - В [CustomPlayerController](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Source/Project_A/Public/Character/CustomPlayerController.h)  добавляется MappingContext черезе интерфейс редактора.
    - [Компонент сражения](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Source/Project_A/Public/Character/BattleComponent.h) на данном этапе используется для связывания с InputAction.
    -  [Персонаж](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacter.uasset) (в BP-редакторе) реализует Event OnEquipWeapon(), который будет вызвать метод [компонента оружия](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponComponent.h) UWeaponComponent::EquipWeaponbyTag. Теги добавляются в [WeaponSystemPluginGameplayTags.ini](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Plugins/WeaponSystemPlugin/Config/Tags/WeaponSystemPluginGameplayTags.ini)
    - [Компонента оружия](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponComponent.h) через метод [фабирики](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponFactory.h) UWeaponFactory::SpawnWeaponByTag создаёт оружие и крепет его к сокету, указанному в BP-редакторе.
    - [Фабрика](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponFactory.h) использует статический метод для загрузки всех WeaponDataAsset, далее проверяет совпадение по тегу и выполняет SpawnActor< AWeaponBase> и настраивает параметры оружия.
    - [Плагин оружия](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Plugins/WeaponSystemPlugin/WeaponSystemPlugin.uplugin) добавляет PrimaryAssetType в [WeaponSystemPlugin.cpp](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Private/WeaponSystemPlugin.cpp)

2. Призыв врага
    - [DataTable](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Content/Project_A_Root/Character/Npc/Enemy/EnemyDataTable.uasset) заполняется в редакторе по структуре [FEnemyRow](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Source/Enemy/Public/EnemyRow.h).    
    - В [BP_PlayerCharacterController](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacterController.uasset) через Debug-клавишу Q вызывается Server_SpawnEnemyInFront (это DevelopmentOnly-метод).
    - Server_SpawnEnemyInFront создаёт временную [EnemyFactory](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Source/Enemy/Public/EnemyFactory.h) и вызывает её метод SpawnEnemyByTag. Теги добавляются в  [EnemyTags.ini](https://github.com/etrehappy/ue_prj_a/blob/hw6/Project_A/Config/Tags/EnemyTags.ini)
</details>

<details><summary id="task7">Задание 7. Оружие и стрельба <p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ](https://drive.google.com/file/d/1ElaLLC0NnkKew8Z7iY4K1MWFTVgSKBe_/view?usp=sharing)

- [x] Созданы три вида оружия
    - ближнее (меч)
    - дальнее (магическое/fireball/гримуар)
    - метательное (бомба) 
- [x] Слот для оружия в персонаже
- [x] Оружие можно изменить через настройки персонажа

Дополнительно: 
- [CombatComponent](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Public/Character/CombatComponent.h) — обрабатывает логику связанную с боем
- Расширен [WeaponComponent](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponComponent.h)
    - [AbilityComponent](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/AbilityComponent.h) — управляет BaseAbility
    - [BaseAbility](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/BaseAbility.h) — на текущем этапе проверяет доступность действия
    - [WeaponMagic](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponMagic.h), [WeaponMelee](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponMelee.h), [WeaponThrowable](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponThrowable.h) — расшыряют [WeaponBase](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponBase.h)  из-за разных боевых механик
    - [ProjectileBase](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/ProjectileBase.h)  — для магического оружия или метательных предметов
    - [TargetIndicator](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/TargetIndicator.h)  — для визуализации траектории броска
    - [AreaExplosion](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/AreaExplosion.h)  — для метательных предметов
- [AnimAttackNotifies](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/AnimAttackNotifies.h) — добавлены собственные уведомления для работы с анимацией атаки
- [CombatMontageTable](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/CombatMontageTable.h) — в C++ используется montage для слияния атаки с Locomotion
- [CharacterAnimInterface](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/CharacterAnimInterface.h) — для передачи информации из C++ в ABP.

**Личная цель задания**:
Рассмотреть разные варианты работы с оружием в RPG играх. 
<br> Цель выполнена, но получившийся код подходит только для учебного проекта, так как требует доработки. Некоторые элементы имеют сильную\избыточную связанность. <b>Допускаю, что какие-то механики можно было реализовать более простым способом</b>. Но либо это было неинтересно, либо отсутсвует соответсвующий опыт. Так как время ограничено, то сделан минимум, который понадобится дальше в проекте.

**Как сделано**:
1. Работа с оружием и способностями реализовывалась через [теги](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Config/Tags/WeaponSystemPluginGameplayTags.ini) и [фабрику оружия](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WeaponFactory.h). 
2. Математические расчеты (прицеливание, направление projectile, удар) описывались при помощи генеративного ИИ. Получившиеся расчеты рабочие, но требуют больше времени на отладку.
3. Слот для оружия в персонаже — временное поле в BP персонажа WeaponTag или ThrowableItemTag
    <div style="text-align: center;">
                <img src="https://github.com/etrehappy/ue_prj_a/blob/hw7/img_for_readme/character_tags.jpg" alt="tags" width="500" height="141">   
    </div>
4. Экипировка оружия (частично реализовано в Задании 6)
    - В [InputComponent](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Public/Character/CustomInputComponent.h) добавлена реакция на Input экипировки (клавиша C для базового оружия и клавиша T для метательного).
    - В C++ [персонаж](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Public/Character/NetPlayerCharacter.h) вызывает BlueprintImplementableEvent для экипировки.
    - В [Blueprint персонажа](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacter.uasset) указываются базовые настройки для генерации оружия. Меш персонажа должен иметь подходящий сокет. В учебном проекте универсальность сокетов не доработана, поэтому текущие сокеты подходят для однотипного оружия, а настройки подобраны для конкретного экземпляра. 
        <div style="text-align: center;">
                <img src="https://github.com/etrehappy/ue_prj_a/blob/hw7/img_for_readme/equip_weapon.jpg" alt="tags" width="216" height="300
                ">   
        </div>
    - Далее через [UWeaponComponent::Server_EquipWeaponByTag](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Private/WeaponComponent.cpp) и [UWeaponFactory::SpawnWeaponByTag](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Private/WeaponFactory.cpp) создаётся оружие (меч, гримуар) или метательный предмет (бомба).
    - Настройки оружия для генерации фабрикой задаются через [Data Asset](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Content/Project_A_Root/Weapons). 
5. Снятие\замена оружия
    - Вызывается по нажатию той же клавиши, что и для экипировки. 
    - В C++ [UCombatComponent::TryUnequipWeapon](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Private/Character/CombatComponent.cpp) реализует логику.
6. Простая атака (меч/fireball)
    - В [InputComponent](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Public/Character/CustomInputComponent.h) добавлена реакция на Input ЛКМ
    - [AbilityComponent](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/AbilityComponent.h) проверяет, имеет ли персонаж подходящую способность для этого Input. Список способностей задаётся через Blueprint. Сама спасобность также настраивается через Blueprint, знает про доступные ей теги оружия и соответствующий Input.
        <div style="text-align: center;">
                <img src="https://github.com/etrehappy/ue_prj_a/blob/hw7/img_for_readme/ability_component.jpg" alt="ability_component" width="427" height="317
                ">   
        </div>
        <br>
        <div style="text-align: center;">
                <img src="https://github.com/etrehappy/ue_prj_a/blob/hw7/img_for_readme/ability.jpg" alt="ability" width="425" height="388
                ">   
        </div>
    - Если персонаж имеет подходящее оружие, то способность транслирует, что она может быть активирована. В текущей реализации способность знает про ETriggerEvent, но пока используется только Completed-событие.
    - [Персонаж](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Public/Character/NetPlayerCharacter.h) подписан на уведомление от способности.
    - В [CombatComponent->UpdateAttackState](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Private/Character/CombatComponent.cpp) происходит обновление тегов. Текущее состояние хранится в контейнере CurrentAttackTags (тег оружия + тег способности). Изменение CurrentAttackTags приводит к запуску анимации на клиенте. Параллельно оружие переходит в состояние атаки.
    - Так как на сервере отключены анимации, то клиент передаёт на сервер данные для расчета: 
        - Меч <p>Если запущена атака, каждый тик передает позицию основания и позицию конца оружия через [Server_SendSwingPositions (Unreliable)](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Private/WeaponMelee.cpp). Сервер расчитывает столкновение с объектами и регистрирует удар. Если анимация завершается и удар не зарегистрирован, то оружие переходит в "базовое\неатакующее" состояние.</p>
        - Магия (fireball) <p>Анимация сообщает о моменте, когда нужно вызвать projectile. Клиент передаёт данные на сервер, там projectile создаётся, транслируется и регистрируется удар.</p>    
7. Прицеливание и бросок (бомба)
    - Прицеливание и бросок предмета связаны по смыслу, но это разные способности в Blueprint. 
    - Прицеливание активируется автоматически при экипировке метательного предмета: 
        - В [InputComponent](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Public/Character/CustomInputComponent.h) есть отдельный метод для экипировки метатльного предмета. Он запускает логику оружия и логику способности. Аналогично с простой атакой выше, если способность (прицеливание) активирована, это приведёт к обновлению состояния [UCombatComponent::UpdateAttackState](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Private/Character/CombatComponent.cpp) и анимации на стороне клиента. 
        - При экипировке создается [TargetIndicator](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/TargetIndicator.h), который отвечает за траекторию броска и зону поражения.
        - Траектория реализована с помощью простого Plane из стартового контента. Зона поражения — декаль. 
    - Бросок активируется подобно простой атаке, но по нажатию ПКМ. 
        - В [InputComponent](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Public/Character/CustomInputComponent.h) обрабатывается как обычная способность, что приведёт к обновлению состояния [UCombatComponent::UpdateAttackState](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Private/Character/CombatComponent.cpp) и анимации на стороне клиента.
        - По аналогии с простой магической атакой (fireball) вызывается projectile, который летит по той же траектории, что и прогнозировало прицеливание. 
        - Если projectile сталкивается с объектом на пути — регистрируется удар на сервере. 
        - После броска автоматически экипируется последнее оружие.
8. Меч    
    - В static mesh оружия добавлены сокеты (TipSocket и BaseSocket) для расчета удара.
    - Появляется в правой руке.
    - Реализована только простая атака.
9. Магия
    - В левой руке книга, в правой — появляется projectile. 
    - Реализована только простая атака.    
10. Анимация атаки
    - В [ABP](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Content/Project_A_Root/Character/Player/Animation/ABP_SimpleCharacter.uasset) есть state machine. В одной ветке ChoserTable "Locmotion", а в другой добавляется AnimMontage (как наиболее простой вариант для смешивания анимаций). 
    - Переключение на другую ветку происходит по обновлению состояния атаки. ABP реализует интерфейс [ICharacterAnimInterface](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/CharacterAnimInterface.h), благодаря чему [UCombatComponent::StartAnimAttack()](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Private/Character/CombatComponent.cpp) выполняет [ICharacterAnimInterface::Execute_SetCanAttack(AnimInstance, bIsFighting)](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Private/CharacterAnimInterface.cpp).
    - После того, как способность активирована, это приведёт к запуску анимации на клиенте. 
    - Анимация атаки имеет [уведомление об окончании](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/AnimAttackNotifies.h), чтобы изменить состояние атаки.
    - Для прицеливания метательным предметом используется зацикленный AnimMontage. Выход из цикла происходит в C++ при броске или при смене оружия.
    - Каждой способности соответсвует своя анимация (через теги) в таблице [DT_CombatAnimation](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Content/Project_A_Root/Character/Player/Animation/Attack/DT_CombatAnimation.uasset)
</details>

<details><summary id="task8">Задание 8. Компонент здоровья<p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ](https://drive.google.com/file/d/1X_ZL3i0zRIkcdsAuEyjo1VF1Tolof7mG/view?usp=sharing)

- [x] Добавлена система жизни — [HealthComponent](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Source/Common/Public/HealthComponent.h) (показатель здоровья, смерть)
- [x] Добавлена система получение урона.
    - [WspDamageType](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/WspDamageType.h) — для отслеживания собственных Damage-классов
- [x] Доступны мишени с возможностью получения урона и тестирования оружия.
- [x] Отображение урона на мишенях

Дополнительно: 
- Добавлены анимации смерти для персонажа и врагов
- Добавленые тестовые зоны для урона и исцеления

**Как сделано**:
1. HealthComponent — общий класс для персонажа и NPC\врагов
    - реализован минимум, компонент имеет только CurrentHealth и MaxHealth
2. Получение урона
    <br> Разные виды оружия работают по простой схеме со встроенными инструментами.
    - Меч <p>Использует UGameplayStatics::ApplyDamage. [Тип урона (физический)](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Content/Project_A_Root/Weapons/DamageType/WspDamageType_PhysicalDamage.uasset) задается через DataAsset оружия</p>
    - Магия (fireball) <p>Так как оружие создает [Projectile](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Public/ProjectileBase.h), то урон также наносится этим Projectile с помощью ApplyDamage. [Тип урона (магический)](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Content/Project_A_Root/Weapons/DamageType/WspDamageType_MagicIce.uasset) задается через [Projectile's blueprint](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Content/Project_A_Root/Weapons/Magic/BP_Fireball.uasset)</p> 
    - Метательное (бомба) <p>По аналогии с магическим использует Projectile, но наносит урон по площади. [Тип урона (физический)](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Content/Project_A_Root/Weapons/DamageType/WspDamageType_PhysicalDamage.uasset) также задаётся через [Projectile's blueprint](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Content/Project_A_Root/Weapons/Throwable/BP_BombProjectile.uasset), но имеет флаг урона по площади. Урон по области реализуется в методе [AAreaExplosion::Explode](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Plugins/WeaponSystemPlugin/Source/WeaponSystemPlugin/Private/AreaExplosion.cpp) с помощью Collision Sphere, bHit и стандартного ApplyDamage.</p>
3. Исцеление
    - Работает только в соответсвующей зоне, с помощью отрицательного урона и ApplyDamage.
    - На данном этапе в enum EInjuryType имеет дополнительный вид урона "Heal". HealthComponent по типу урона выполняет либо IncreaseHealth, либо DecreaseHealth
4. Отображение урона
    - Реализовано в виде временного [Actor](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Content/Project_A_Root/Blueprints/DamageTest/UI/BP_FloatingDamage.uasset) со своим [WidgetComponent](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Content/Project_A_Root/Blueprints/DamageTest/UI/WC_FloatingDamage.uasset), который использует простой текстовый [Widget](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Content/Project_A_Root/Blueprints/DamageTest/UI/WBP_TempHealth.uasset).
    - HealthComponent транслирует на сервере о полученном уроне. Персонаж\враг реализует Cosmetic-методы для вызова текстового [Actor](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Content/Project_A_Root/Blueprints/DamageTest/UI/BP_FloatingDamage.uasset).
        <div style="text-align: center;">
                    <img src="https://github.com/etrehappy/ue_prj_a/blob/hw8/img_for_readme/damage.jpg" alt="damage" width="500" height="208
                    ">   
        </div>
        <br>
        
    
5. Отображение исцеления
    - То же, что для урона. Но при создании текстового [Actor](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Content/Project_A_Root/Blueprints/DamageTest/UI/BP_FloatingDamage.uasset) передаются другой цвет, Abs-значение, другая позиция.
6. Анимация смерти
    - По аналогии с уроном\исцелением HealthComponent транслирует на сервере о смерти. Далее персонаж\враг реализуют Cosmetic-методы. 
        <div style="text-align: center;">
                <img src="https://github.com/etrehappy/ue_prj_a/blob/hw8/img_for_readme/death.jpg" alt="death" width="500" height="168
                ">   
        </div>
    - После смерти враг просто исчезает по таймеру, а персонаж удалется и создается новый на стартовой точке.
7. Мишень
    - есть постоянный Pawn на сцене, который наследуется от EnemyBase, но не реализует анимацию смерти
    - враг — был реализован ранее, вызов по клавише Q

</details>


<details><summary id="task9">Задание 9. Расширение pickup системы<p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ](https://drive.google.com/file/d/1CuxZw_uTpaNWLs4wt6N9_XUH93Rg0Tku/view?usp=sharing)

- [x] Добавлены специализированные типы объектов.
- [x] Реализовано их поднятие.
- [x] Реализован учёт в персонаже.
- [x] В лог выводится информацию о поднятых объектах.

Дополнительно: 
- Добавлен простой текстовый popup
- [InventoryComponent](https://github.com/etrehappy/ue_prj_a/blob/hw9/Project_A/Source/Project_A/Public/InventoryComponent.h)
- [Inventory](https://github.com/etrehappy/ue_prj_a/blob/hw9/Project_A/Source/Project_A/Public/Inventory.h)

**Как сделано**:
1. Предметы для поднятия: 
     - по аналогии с примером на занятии добавлен [AItemPickup](https://github.com/etrehappy/ue_prj_a/blob/hw9/Project_A/Source/Project_A/Public/ItemPickup.h), от которого наследуются игровые BP-предметы ([кристалл](https://github.com/etrehappy/ue_prj_a/blob/hw9/Project_A/Content/Project_A_Root/Levels/FirstMap/Items/BP_CrystalItemPickup.uasset) и [нож](https://github.com/etrehappy/ue_prj_a/blob/hw9/Project_A/Content/Project_A_Root/Levels/FirstMap/Items/BP_KnifePickup.uasset) )
     - в планах: реализовать поднятие предметов с учетом уже настроенной работы с тегами
2. Поднятие и учет предметов (временное решение, так как нет полноценной системы взаимодействия в C++)
    - в [InputComponent](https://github.com/etrehappy/ue_prj_a/blob/hw9/Project_A/Source/Project_A/Public/Character/CustomInputComponent.h) добавлен соответсвующий Input (клавиша E)     
    - когда персонаж пересекает SphereComponent [поднимаемого объекта](https://github.com/etrehappy/ue_prj_a/blob/hw9/Project_A/Source/Project_A/Public/ItemPickup.h) обновляется ссылка на текущий объект взаимодействия у [персонажа](https://github.com/etrehappy/ue_prj_a/blob/hw9/Project_A/Source/Project_A/Public/Character/NetPlayerCharacter.h) (временное решение)      
    - Input вызывает метод Interact() у персонажа -> PickUp и Destroy у поднимаемого объекта -> AddToInventory у персонажа (временное решение, пока не продуман полноценный инвентарь)
    - AddItem просто добавляет элементы в контейнер и выводит в лог их количество (временное решение) 
3. Popup
    - реализовано с помощью [WBP_Hud](https://github.com/etrehappy/ue_prj_a/blob/hw9/Project_A/Content/Project_A_Root/Character/Player/UI/WBP_Hud.uasset) (аналогично примеру на занятии) и простого текстового [WBP_PickupTooltip](https://github.com/etrehappy/ue_prj_a/blob/hw9/Project_A/Content/Project_A_Root/Character/Player/UI/WBP_PickupTooltip.uasset)
    - Hud создается и сохраняется через BP персонажа (временное решение)
    - персонаж реализует [PickUpInterface](https://github.com/etrehappy/ue_prj_a/blob/hw9/Project_A/Source/Project_A/Public/PickUpInterface.h) (временное решение)

</details>


<details><summary id="task10">Задание 10. Инвентарь<p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ](https://drive.google.com/file/d/1wCzFuY_E5RFIR53HeuGxp2e5Ci9jhrnc/view?usp=sharing)

- [x] Добавлена возможность подбирать объекты со сцены
- [x] Добавлена возможность хранения оружия с помощью системы инвентаря;
- [] ~~Добавлено возможность хранения боеприпасов для оружия~~ (не требуется в проекте)

Дополнительно: 
- В [UInventoryComponent](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Public/InventoryComponent.h) есть общий инвентарь и инвентарь экипированных предметов
- Добавлена система взаимодействия с окружающими объектами
- Добавлен UI для инвентаря (минимальный)
- Доступно:
    - экипировка оружия в зависимости от предмета в ячейке для оружия; 
    - перетаскивание предметов из одного инвентаря в другой
    - перетаскивание предметов внутри инвентаря

Что <u>не</u> сделано (из ожидаемого):
- выбрасывание предметов из инвентаря;
- отображение кол-ва предметов в ячейке (stack реализован, но отключен);
- валидация предметов в ячейках для экипировки; 
- связь между развооружением и инвентарем;
- меню для предмета инвентаря (ПКМ).

**Как сделано**:
1. Взаиодействие с объектами
<br> Временная система из Задания 9 (выше) удалена. Реализован другой подход.
    - Добавлен [UInteractionComponent](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Interaction/Public/InteractionComponent.h)
    - Объекты, с которыми можно взаимодействовать, реализуют интерфейс [UInteractable](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Interaction/Public/Interactable.h). Такие объекты имеют Sphere collision для Overlay c пресонажем. 
    - На тике InteractionComponent [составляет](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Interaction/Private/InteractionComponent.cpp) список кандидатов, которые находятся рядом и доступны для взаимодействия.
    - После нажатия на клавишу "E" InteractionComponent выбирает наиболее подходящий объект (в зависимости от поворота и рассотяния до объекта).
    - Interactable-объект реализует метод для взаимодействия с ним.

2. Инвентарь
<br> Доработана система, созданная в Задании 9 (выше).
    - Система инвентаря использует следующие элементы:
        - [UInventoryComponent](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Public/InventoryComponent.h) — имеет 2 инвентаря (обычный и для снаряжения), отвечает за репликацию инвентаря;
        - [UInventory](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Public/Inventory.h) — реплицируемый UObject, который имеет массив локальных InventorySlots и массив "облегченных" ReplicatedSlots;
        - [FReplicatedSlotArray](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Public/InventorySlot.h) — массив для передачи содержимого инвентаря по сети, данные восстанавливаются на клиенте, использует функционал FFastArraySerializer;
        - [FInventorySlot](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Public/InventorySlot.h) — ячейка обычного (локального) инвентаря; 
        - [FInventoryReplicatedSlot](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Public/InventorySlot.h) — ячейка реплицируемого (облегченного) инвентаря;
        - [UInventoryItem](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Public/InventoryItem.h) — объект, который можно хранить в UInventory;
        - [UInventoryItemDefinition](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Public/InventoryItem.h) — описание предмета, который можно хранить в инвентаре;
            <div style="text-align: center;">
                <img src="https://github.com/etrehappy/ue_prj_a/blob/hw10/img_for_readme/inventory_item.jpg" alt="death" width="460" height="251">   
            </div>
        - [UInventoryItemAdapter_Weapon](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Public/InventoryItem.h) — наследник UInventoryItem, который связывает представление предмета в инвентаре с оружейным плагином из Задания 6 (выше); 
    - Добавление предмета в инвентарь:
        1. На серевере [UInventory::AddItem](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Private/Inventory.cpp) обновляет массив локальных InventorySlots и реплицируемых [ReplicatedSlots](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Public/Inventory.h).
        2. На клиенте [FReplicatedSlotArray::PostReplicatedAdd](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Private/Inventory.cpp) вызывает [UInventory::HandleReplicatedAdd](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Private/Inventory.cpp), который восстанавливает данные через [UInventory::UpdateLocalSlotFromReplicated](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Private/Inventory.cpp) и транслирует, что инвентарь обновлён.
        3. Виджет получает уведомление OnInventoryChanged и [обновляется](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/UI/Private/InventoryWidget.cpp).
    - Перемещение предметов внутри инвентаря или в другой инвентарь:
        1. Виджет проверяет в какую ячейку и в какой инвентарь был сделан Drop (в методе [UInventorySlotWidget::NativeOnDrop](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/UI/Private/InventoryWidget.cpp)), а затем вызывает соответствующий метод инвентаря (перенос внутри инвентаря или в другой).
        2. Так как инвентарь — это UObject, он транслирует событие (например, FOnInventoryMoveRequested) для UInventoryComponent, чтобы запустить логику именно на сервере.
        3. UInventoryComponent вызывает на сервере подходящий метод (например, [UInventoryComponent::MoveItemToOtherInventory](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/Inventory/Private/InventoryComponent.cpp)), чтобы обновить локальный инвентарь и "облегченный" инвентарь для репликации.
        4. На клиенте данные обновляются по аналогии с "добавление предмета" выше.
3. UI
<br> Минимум для визуализации.        
    - Добавленны следующие элементы:
        - [AGeneralHud](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/UI/Public/GeneralHud.h) — наследник AHUD, который устанавливается в World;
        - [UBaseWidget](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/UI/Public/BaseWidget.h) — общий класс для всех виджетов; 
        - [UGeneralWidget](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/UI/Public/GeneralWidget.h) — основной виджет, который содержит UInventoryWidget и UEquippedItemWidget;
        - [UInventoryWidget](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/UI/Public/InventoryWidget.h) — для обычного инвентаря; 
        - [UInventorySlotWidget](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/UI/Public/InventorySlotWidget.h) — для слота обычного инвентаря;
        - [UEquippedItemWidget](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/UI/Public/EquippedItemWidget.h) — для инвентаря экипировки; 
        - [UEquippedItemSlotWidget](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/UI/Public/EquippedItemSlotWidget.h) — для слота инвентаря экипировки;
    - Для фиксации 'drag & drop' [UInventorySlotWidget](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/UI/Public/InventorySlotWidget.h) использует Native-события.
    - Для визуализации перетаскивания [UInventorySlotWidget](https://github.com/etrehappy/ue_prj_a/blob/hw10/Project_A/Source/UI/Private/InventorySlotWidget.cpp) использует встроенный UDragDropOperation.

</details>


<details><summary id="task11">Задание 11. Widgets<p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ](https://drive.google.com/file/d/1ony2NzsrwTex4TGzvH4nrJPiBllehNxO/view?usp=sharing)

- [x] визуальное отображение жизни на экране;
- [x] визуальное отображение инвентаря на экране.

Дополнительно
- Widget после смерти персонажа.

Что не сделано (но ожидается):
- система персистентности.

**Как сделано**:
1. Widget инвентаря
<br> Было реализовано в задании 10 ([см. выше](#task10)).        

2. Widget здоровья
    - Добавлен [HealthWidget](https://github.com/etrehappy/ue_prj_a/blob/hw11/Project_A/Source/UI/Public/HealthWidget.h).
    - HealthWidget подписывается на делегат OnHealthChanged [компонента здоровья](https://github.com/etrehappy/ue_prj_a/blob/hw11/Project_A/Source/Common/Public/HealthComponent.h) у персонажа.
    - Метод [HandleHealthChanged](https://github.com/etrehappy/ue_prj_a/blob/hw11/Project_A/Source/UI/Private/HealthWidget.cpp) обновляет HpProgressBar и числовое значение здоровья.  
        <div style="text-align: center;">
                <img src="https://github.com/etrehappy/ue_prj_a/blob/hw11/img_for_readme/hp_bar.jpg" alt="death" width="309" height="98">   
        </div>

3. Widget после смерти персонажа
    - Персонаж подписан на изменение здоровья. 
    - Метод ANetPlayerCharacter::[HandleHealthChanged](https://github.com/etrehappy/ue_prj_a/blob/hw11/Project_A/Source/Project_A/Private/Character/NetPlayerCharacter.cpp) локально показывает окно смерти через метод ACustomPlayerController::[ShowDeathMenu()](https://github.com/etrehappy/ue_prj_a/blob/hw11/Project_A/Source/Project_A/Private/Character/CustomPlayerController.cpp).
    - ACustomPlayerController подписан на кнопки, которые пользователь может нажать в [UDeathMenuWidget](https://github.com/etrehappy/ue_prj_a/blob/hw11/Project_A/Source/UI/Public/DeathMenuWidget.h). 
    - Методы ACustomPlayerController::[HandleDeathMenuRespawnRequested](https://github.com/etrehappy/ue_prj_a/blob/hw11/Project_A/Source/Project_A/Private/Character/CustomPlayerController.cpp) и ACustomPlayerController::HandleDeathMenuExitRequested обрабатывают нажатие кнопок.
        <div style="text-align: center;">
                <img src="https://github.com/etrehappy/ue_prj_a/blob/hw11/img_for_readme/death_menu.jpg" alt="death" width="500" height="257">   
        </div> 

</details>

<details><summary id="task12">Задание 12. Бафы и дебафы<p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ](https://drive.google.com/file/d/16sgt-qhc-7Fp2WlKxttJWKvDxvBs3XRt/view?usp=sharing)

- [x] три разных вида бафов для персонажа (восполнение здоровья через еду, увеличение скорости и защиты через зелья);
- [x] два вида дебафов для персонажа (потеря здоровья каждую секунду, снижение скорости).

Дополнительно:
- созданы предметы с эффектами (еда, зелья);
- добавлено минимальное контекстное меню в инвентарь.

Добавлено:
- [FStatusEffectDef](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Common/Public/StatusEffect/StatusEffectTypes.h) — описание эффекта.
- [FEffectAction](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Common/Public/StatusEffect/StatusEffectTypes.h) — действия, которые накладывает эффект на персонажа.
- [FActiveStatusEffect](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Common/Public/StatusEffect/StatusEffectTypes.h) — активный эффект на персонаже.
- [UStatusEffectData](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Common/Public/StatusEffect/StatusEffectData.h) — Primary Data Asset, в котором перечислены все бафы и дебафы
- [UStatusEffectSubsystem](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Common/Public/StatusEffect/StatusEffectSubsystem.h) — подсистема для загрузки эффектов и поиска.
- [IStatusEffectStatHandler](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Common/Public/StatusEffect/StatusEffectStatHandler.h) — интерфейс для различных компонентов, которые отвечают за характеристики персонажа (здоровье, движение и т.п.).
- [UStatusEffectComponent](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Common/Public/StatusEffect/StatusEffectsComponent.h) — компонент для персонажа

**Как сделано**:
1. Система эффектов (бафов и дебафов)
    - Объект в мире
        - Объект, который может наложить эффект, отслеживает пересечение. 
        - Если Actor имеет компонент [UStatusEffectComponent](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Common/Public/StatusEffect/StatusEffectsComponent.h), то вызывается метод UStatusEffectComponent::[ApplyEffectByTag](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Common/Private/StatusEffect/StatusEffectsComponent.cpp).
        - Если это [i]периодический[/i] эффект (например, кровотечение) или [i]длительный[/i] (например, увеличение защиты на 30 сек.), он помещается в список активных. 
        - Если это [i]разовый[/i] эффект, он применяется сразу (например, +10 ед. лечение). 
        - На [тике](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Common/Private/StatusEffect/StatusEffectsComponent.cpp) проверяются активные эффекты. Тик срабатывает 5 раз в секунду. Если активных эффектов нет — тик отключается. 
        - В методе UStatusEffectComponent::[ApplyAction](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Common/Private/StatusEffect/StatusEffectsComponent.cpp) отыскивается компонент, отвечающий за характеристику, на которую влияет эффект (например, ищем какой компонент отвечает за здоровье). 
        - Компонент, отвечающий за характеристику, должен реализовать интерфейс [IStatusEffectStatHandler](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Common/Public/StatusEffect/StatusEffectStatHandler.h). 
        - Компонент меняет характеристику (например, для компонента здоровья — [UHealthComponent](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Common/Private/HealthComponent.cpp)::ApplyStatusEffectAction, а для скорости — [UCustomLocomotionComponent](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Project_A/Private/Character/CustomLocomotionComponent.cpp)::ApplyStatusEffectAction).

        Сейчас используется примитивная логика в методах ApplyStatusEffectAction.

    - Предмет из инвентаря
        - В [InventoryItem](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Inventory/Public/InventoryItem.h) добавлено поле bConsumable. 
        - Если в ячейке ([UInventorySlotWidget](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/UI/Public/InventorySlotWidget.h)) есть предмет, то по ПКМ срабатывает OnContextMenuRequested, который в BP создаёт виджет контектного меню. Сейчас добавлена только одна кнопка — UseItem (без доп. проверок).
        - Вызывается метод UInventoryComponent::[UseItem](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Inventory/Private/InventoryComponent.cpp) далее UInventory::[UseItemFromSlot](https://github.com/etrehappy/ue_prj_a/blob/hw12/Project_A/Source/Inventory/Private/Inventory.cpp) (для очистки) и  UStatusEffectComponent::ApplyEffectByTag. 
        - Дальнейшая схема аналогична.

</details>

<details><summary id="task13">Задание 13. Смена оружия<p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ](https://drive.google.com/file/d/1VBIP8zMkNRNui3LQjrYf67wTmtAIY0zo/view?usp=sharing)

- [x] реализовано не менее двух разных способностей персонажа;
    - ближняя атака (меч);
    - дальняя атака (fireball);
    - метательное оружие (бомба).
- [x] настроена анимация с выбранным предметом инвентаря;
- [x] возможность смены оружия через инвентарь;
- [ ] ~~возможность зарядки оружия боеприпасами из инвентаря~~.
    - настройка анимации стрельбы из лука займёт много времени, поэтому такое оружие пока [i]не[/i] добавлено;
    - в качестве альтернативы — ячейка с бомбами в инвентаре.

Дополнительно:
- настроено отображение кол-ва предметов в инвентаре;
- реализован стак; 
- если оружие экипировано (активно) и снимается из ячейки инвентаря, то оно снимается также с персонажа (персонаж остается без оружия).

Что не сделано:
- валидация предметов в инвентаре экипировки. 

**Как сделано**:
1. Способности персонажа и анимация
    - реализовано в задании 7([см. выше](#task7)).;

2. Cмена оружия через инвентарь
    - начинается с события [UInventorySlotWidget::NativeOnDragDetected](https://github.com/etrehappy/ue_prj_a/blob/hw13/Project_A/Source/UI/Private/InventoryWidget.cpp) (перемещение предметов внутри инвентаря реализовано в задинии 10, [см. выше](#task10) ); 
    - после того, как оружие поместили в инвентарь экипировки, его можно надеть по клавише C (реализовано в задании 7, [см. выше](#task7)).

3. Стак: 
    - в UInventoryItemDefinition добавлены поля bStackable и MaxStackCount; 
    - при перемещении предметов (реализовано в задинии 10, [см. выше](#task10)) на ячейку с предметом выполняется  UInventory::TryStackItemIntoSlot.    

3. Ячейка с бомбами:
    - активное оружие меняется на метательное по клавише T, бросок по клавише ПКМ (реализовано в задании 7, [см. выше](#task7));
    - в виджет экипированных предметов [UEquippedItemWidget](https://github.com/etrehappy/ue_prj_a/blob/hw13/Project_A/Source/UI/Public/EquippedItemWidget.h) добавлено поле ThrowableSlot для метательных предметов; 
    - ячейка поддерживает стак в отличие от ячейки для обычного оружия; 
    - после успешного броска UCombatComponent::[OnSpawnBombProjectile()](https://github.com/etrehappy/ue_prj_a/blob/hw13/Project_A/Source/Project_A/Private/Character/CombatComponent.cpp) кол-во бомб в инвентаре уменьшается за счёт UInventory::[UseItemFromSlot()](https://github.com/etrehappy/ue_prj_a/blob/hw13/Project_A/Source/Inventory/Private/Inventory.cpp).

4. Сброс активного оружия после перемещения в инвентарь
    - [UCombatComponent](https://github.com/etrehappy/ue_prj_a/blob/hw13/Project_A/Source/Project_A/Private/Character/CombatComponent.cpp) подписан на изменение инвентаря: если оружие в руке персонажа и его удаляют из ячейки, то [HandleEquipmentChanged()](https://github.com/etrehappy/ue_prj_a/blob/hw13/Project_A/Source/Project_A/Private/Character/CombatComponent.cpp) сбросит состояние к обычному.  
</details>

<details><summary id="task14">Задание 14. Система способностей NPC<p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ](https://drive.google.com/file/d/1qScu_0AlYseQMPofJ40ZOmB4O7Ldax8x/view?usp=sharing)

- [x] добавлены три NPC с разным поведением:
    - наблюдатель — смотрит на персонажа после обнаружения, враждебен только при атаке;
    - преследователь — атакует персонажа сразу после обнаружения; 
    - патрульный — перемщается между точками, выдаёт квестовое задание.
- [ ] ~~NPC обладают базовыми способностями персонажа игрока~~
    <br>Пересмотрена работа с NPC. Для них не подходит оружейный компонент, созданный в задании 7 ([см. выше](#task7)) из-за другой структуры скелета и оружия. 
    - добавлен оружейный компонент для NPC [UNpcBattleComponent](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Npc/Public/NpcBattleComponent.h);
    - реализована логика атаки ближним оружием у бота-гоблина;
    - NPC может переиспользовать компоненты, созданные ранее для игрока (например, инвентарь и [компонент здоровья](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Npc/Public/NpcBase.h));
    - NPC может использовать StatusEffectComponent для получения бафов и дебафов, как и персонаж.
- [x] реализована возможность задания поведения NPC через блюпринт:
    - используется StateTree для управления поведением NPC.

Дополнительно:
- подготовлена загатовка (заглушка) для квестовой системы.

Что не сделано:
- не продумана хорошо репликация полей (просто используется Replicated там, где это нужно для игровой логики).

**Как сделано**:
1. NPC:
    - Модуль с врагами был реализован в задании 6 ([см. выше](#task6)). Но для <b>упрощения</b> дальнейшей работы существующий класс пересмотрен: теперь [NpcBase](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Npc/Public/NpcBase.h) наследуется от Character из-за Movement и готовой репликации.
    - [Ассет](https://www.fab.com/listings/0c7313ce-2bf8-4987-848b-6b38dbc38ee5) основного врага использует отдельные скелет и анимацию для оружия. Поэтому в классе *BP_LeadGoblin* (/Game/Project_A_Root/Character/Npc/Enemy/Goblin) пересмотрена логика атаки и работы с оружием — [NpcBattleComponent](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Npc/Public/NpcBattleComponent.h). Пока реализована атака только для ближнего боя. Стиль боя можно изменить, используя наследника от [NpcAttackLogicBase](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Npc/Public/NpcAttackLogicBase.h)(например, [NpcAttackLogicWeaponSweep](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Npc/Public/NpcAttackLogicWeaponSweep.h)).
    - Атака запускается через StateTree (например, [ST_Enemy_PatrolCombat](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Content/Project_A_Root/Character/Npc/EnemyST_Enemy_PatrolCombat.uasset)). Состояние атаки использует собственную задачу [ST_GoblinStartAttackTask](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Content/Project_A_Root/Character/Npc/ST_Enemy_ObserverCombat.uasset).
    - В [логике ближнего боя](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Npc/Public/NpcAttackLogicWeaponSweep.h) у врага используется Tick. Он включается только в момент атаки UNpcBattleComponent::[BeginDamageWindow](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Npc/Private/NpcBattleComponent.cpp) -> UNpcAttackLogicWeaponSweep::[OnAttackStarted](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Npc/Private/NpcAttackLogicWeaponSweep.cpp). Hit расчитывается по пересечению линии между рукояткой и остриём ближнего оружия. 
    - Тип урона задаётся через настройки NpcBattleComponent (соответственно, для атаки по персонажу требуется тип урона реализованный в задании 6, [см. выше](#task6)).
    - Система здоровья у NPC такая же, как у игрока (реализовано в задании 8, [см. выше](#task8)).
    - В [NpcAIController](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Npc/Public/NpcAIController.h) обрабатывается зрение бота. Если игрок замечен, он назначается целью. 
2. Разное поведение NPC:
    - [NpcAIController](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Npc/Public/NpcAIController.h) использует поле *StateTreeAIComponent*. Созданы несколько деревьев для разных ботов: 
    <br> Для облегчения работы в контексте StateTree указаны конкретные классы actor и controller.
        - ST_Enemy_ObserverCombat (/Game/Project_A_Root/Character/Npc/Enemy)
        - ST_Enemy_PatrolCombat (/Game/Project_A_Root/Character/Npc/Enemy)
        - ST_Npc_VillageHeader (/Game/Project_A_Root/Character/Npc/VillageHeader)
3. Бафы и дебафы NPC:
    - NPC использует компонент из задания 12 ([см. выше](#task12));
    - в текущей логике показан только дебаф на здоровье ("кровотечение"), если бот наступит на шипы.
4. Квестовая система: 
    - подготовлен минимальный код, который будет пересматриваться в дальнейшем: реализован только один конкретный сценарий;      
    - класс [AQuestNpcBase](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Quest/Public/QuestNpcBase.h) расширяет функционал NPC для взаимодействия с ним и получения квеста;
    - класс [AQuestObelisk](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Quest/Public/FirstQuest/QuestObelisk.h) отвечает за объект, с которыми можно будет взаимодействовать, чтобы выполнить квест;
    - после получения квеста игрок должен взять в инвентарь предмет (кристалл), затем провзаимодействовать с объектом (положить в него кристалл); 
    - класс [UQuestLogComponent](https://github.com/etrehappy/ue_prj_a/blob/hw14/Project_A/Source/Quest/Public/QuestLogComponent.h) отвечает за квестовую логику у игрока; 
    - на экране отображается примитивный виджет со статусом квеста. 

</details>

<details><summary id="task15">Задание 15. Ландшафт<p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ](https://drive.google.com/file/d/1czJtRQXtfYDhhUGp-icmPXmorQNLVU78/view?usp=sharing)

- [x] добавлен ландшафт;
- [x] на ландшафте выставлены объекты с помощью foliage;
- [x] добавлены динамические материалы;
- [x] имеется сцена с ландшафтом и выстроенным окружением для игры.


Дополнительно:
- Добавлен Level Sequence (/Game/Project_A_Root/Quests/Sequence/LS_FirstObelisk), который срабатывает после взаимодействия с кристаллом в поселении. Но сам квест и система квестов ещё не доработаны, есть примитивная реализация. Можно проверить на видео выше.

**Как сделано**:
1. Основная карта находится по пути /Game/Project_A_Root/Levels/Map
2. Работа с ландшафтом:
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw15/img_for_readme/landscapes.jpg" alt="forwarddot" width="500" height="185">   
        </div>
    - Используются 3 ландшафта: 
        - MainLandscape — главная карта, за основу взято [это окружение](https://www.fab.com/listings/e4434155-b27f-44bd-bc41-2fc09d05c578), переделан рельеф. 
        - Landscape2 — создан вручную, скрывает пустоту за MainLandscape, использует тот же материал, что и MainLandscape.
        - Landscape3 — для дальних объектов за MainLandscape. Сгенерирован на основе <i>карты высот</i> (взята [здесь](https://www.motionforgepictures.com/height-maps/)). Поверхность — просто текстура, растянутая до размеров ландшафта через настройки материала (/Game/Project_A_Root/Levels/Landscape/M_SimpleLandscape)
            <div style="text-align: center;">
                <img src="https://github.com/etrehappy/ue_prj_a/blob/hw15/img_for_readme/landscape3_material.jpg" alt="landscape3_material" width="500" height="131">   
            </div>
            
3. Окружение для игры: 
    - схематичная карта окружения с ключевыми эелементами: 
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw1/img_for_readme/map_1.jpg" alt="map_1" width="500" height="348">   
        </div>
    - расставлены элементы декора и NPC в поселении (мирная зона, защитный пост); 
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw15/img_for_readme/guard_post.jpg" alt="guard_post" width="500" height="314">   
        </div>
    - Обозначены места, в которых будут находиться враги и объекты квеста.
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw15/img_for_readme/fight.jpg" alt="fight" width="500" height="188">   
        </div>

4. Работа с foliage
    - На стандартный ландшафт добавлено озеро. Удалена лишняя трава с прибержной части и дна.
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw15/img_for_readme/lake.jpg" alt="lake" width="500" height="307">   
        </div>
    - Поселение (застава) украшено цветами, чтобы заполнить пустое пространство.
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw15/img_for_readme/foliage.jpg" alt="foliage" width="500" height="265">   
        </div>
    

5. Работа с динамическими материалами
    - Когда игрок взаимодействует с объектом (ставит кристалл на пъедестал), у объекта включается подсветка.
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw15/img_for_readme/crystall.jpg" alt="crystall" width="234" height="165">   
        </div>
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw15/img_for_readme/mid.jpg" alt="mid" width="500" height="152">   
        </div>
    - Активация кристалла включает подсветку также у большого (центрального) сооружия на карте. 
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw15/img_for_readme/obelisk_one_side.jpg" alt="obelisk_one_side" width="100" height="216">   
        </div>
        <br> Реализовано примитивным способом: <i>маленьник объект</i> получает ссылку на <i>центральный камень</i>, а потом напрямую вызывает функцию для включения подсветки. Всего на карте 4 <i>маленьких объекта</i> (пъедесталов для кристаллов) и один <i>центральный камень</i> с 4 сторонами. Каждый <i>пъедестал</i> отвечает за подсветку у одной стороны <i>центрального камня</i>. Предполагается, что будет 4 разных цвета.

</details>

<details><summary id="task18">Задание 18. Меню для мультиплеера<p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ](https://drive.google.com/file/d/1lmuwfW5ZIQt8AL0fEX6iDVZAnosOh1og/view?usp=sharing)

- [x] Добавьте меню для запуска игры и подключения по сети;
- [ ] Создайте лобби
    - В моёй работе не планируются матч-сессии, но игроки могут выбрать сервер и подключиться к нему (по классической схеме MMORPG).
    - В Задании 19 планирую сделать так, чтобы игроки могли объединяться в одну группу и отображать это на экране (но также без модуля авторизации). 


Дополнительно:
- окно выбора персонажа;
- само подключение по сети было реализовано ранее.

Не сделано: 
- Не реализована система авторизации и сохранения данных пользователя, поэтому выбор персонажа — примитивная демонстрация, без осмыленного решения.

**Как сделано**:
1. Подключение по сети:
    - изложено ниже по каждому этапу подключения отдельно. 

2. Меню для запуска игры
    - При запуске игры открывается первая (локальная) карта — /Game/Project_A_Root/Levels/StartMap/ClientDefaultMap — с собственными HUD (/Game/Project_A_Root/Levels/StartMap/ClienFirstMapHUD), [GameMode](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Project_A/Public/Core/ClientStartGameMode.h) и виджетом (/Game/Project_A_Root/Levels/StartMap/Widgets/WBP_MainClientWindow)
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw18/img_for_readme/start_map.jpg" alt="start_map" width="500" height="272">   
        </div>
 
3. Меню для подключения по сети
    - Далее по кнопке Play запускается загрузочный экран WBP_ClientLoadingScreen (/Game/Project_A_Root/Levels/StartMap/Widgets). Если возникнет ошибка подключения — появится подсказка.
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw18/img_for_readme/loading_widget.jpg" alt="loading_widget" width="343" height="179">   
        </div>
    
    <br> и выполняется подключение к промежуточному серверу через AClientStartGameMode::[ConnectToHubServer()](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Project_A/Private/Core/ClientStartGameMode.cpp)
    - Далее клиент попадает на карту Hub-сервера (/Game/Project_A_Root/Levels/StartMap/ServerHubMap), который также имеет свои HUD (/Game/Project_A_Root/Levels/StartMap/HubHUD) и [GameMode](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Project_A/Public/Core/HubGameMode.h).
    - HUD промежуточного сервера показывает локально виджет WBP_ServerSelectionStartMenu со списком серверов. 
        - Для этого через контроллер — ACustomPlayerController::[Server_RequestWorldServerList()](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Project_A/Private/Character/CustomPlayerController.cpp) — отправляется запрос на сервер. На севрере AHubGameMode::[BuildWorldServersSnapshot()](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Project_A/Private/Core/HubGameMode.cpp) даёт информацию о том, какие игровые серверы доступны.
        - Игровой сервер периодически посылает информацию о себе на Hub-сервер через AWorldGameMode::[SendHeartbeat()](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Project_A/Private/Core/WorldGameMode.cpp). Для этого используется структура [FWorldServerView](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Common/Public/WorldServerView.h).
        - Далее в BP HubHUD (/Game/Project_A_Root/Levels/StartMap) обрабатывается Event UpdateServerSelectionTable.
        - Сам список серверов реализован на клиенте через ListView.    
        - Для ListView используется виджет WBP_RowServer (/Game/Project_A_Root/Levels/StartMap/Widgets). Он требует интерфейс <i>ObjectListEntry</i> и реализует <i>Event OnListItemObectSet</i>, для которого необходим Object-reference, из-за чего используется обёртка [UWorldServerEntryObject](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Common/Public/WorldServerView.h) для структуры [FWorldServerView](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Common/Public/WorldServerView.h)
        - Кнопка Update на WBP_ServerSelectionStartMenu аналогично вызывает ACustomPlayerController::[Server_RequestWorldServerList()](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Project_A/Private/Character/CustomPlayerController.cpp).
            <div style="text-align: center;">
                <img src="https://github.com/etrehappy/ue_prj_a/blob/hw18/img_for_readme/server_widget.jpg" alt="server_widget" width="500" height="270">   
            </div>
        - После выбора сервера отправляется запрос на список персонажей через контроллер ACustomPlayerController::[Server_RequestCharacterList()](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Project_A/Private/Character/CustomPlayerController.cpp).
        <br> Если информация получена — следующий экран, если нет — пока только вывод в консоль.
        

4. Меню выбора персонажа
    - НЕ реализованы системы авторизации и сохранения прогресса пользователя, поэтому далее с персонажами используются простые схемы (заглушки), чтобы отобразить общую идею. <br><br> 
    - После получении списка персонажей BP HUD (/Game/Project_A_Root/Levels/StartMap/HubHUD) показывает локально виджет — WBP_CharacterSelectionScreen (/Game/Project_A_Root/Levels/StartMap/Widgets) — и сцену с персонажами игрока.
    - Список персонажей реализован по той же схеме, что и список серверов в пункте 3 выше: 
        - ListView, 
        - WBP_RowCharacter (/Game/Project_A_Root/Levels/StartMap/Widgets),
        - AHubGameMode::[BuildMockCharactersForServer()](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Project_A/Private/Core/HubGameMode.cpp) — здесь просто заглушка из текстовых данных.
    - Переключение персонажей — переключение уже размещенного актора на сцене, чтобы не удлинять по времени работу над текущим заданием по UI.
        <div style="text-align: center;">
            <img src="https://github.com/etrehappy/ue_prj_a/blob/hw18/img_for_readme/character_widget.jpg" alt="character_widget" width="500" height="312">   
        </div>
    - Но выбор персонажа всё-таки влияет на то, какой Character будет загружен в игровом мире. Это реализовано также простым способом через временное TMap AWorldGameMode::[CharacterDefinitionById](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Project_A/Public/Core/WorldGameMode.h). 
    - По кнопке «StartGame» доходим до AHubGameMode::[EnterToWorldWithCharacter](https://github.com/etrehappy/ue_prj_a/blob/hw18/Project_A/Source/Project_A/Private/Core/HubGameMode.cpp), передавая информация о кокнретном персонаже и сервере. 
    - Если подключение успешно, персонаж появляется в мире. Если нет — пока информаци только в консоли.

</details>

<details><summary id="task19">Задание 19. Сетевая игра<p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ]()

- [x] Добавлена возможность подключения игроков к лобби.
- [x] Из лобби игроки способны подключиться к игровой сцене.
    - [x] есть ожидание других игроков;
    - [x] есть статус готовности;
    - [x] есть принудительный старт матча.
- [x] Работает подключение через лобби для 2 или более игроков.
- [x] Добавлена возможность взаимодействия игроков друг с другом
    - наносить урон; 
    - объединение в группу. 

Исправления по комментариями к прошлому ДЗ: 
- <i>Нет конфигурируемости через .ini для подключения к серверу</i> — добавлено поле LobbyTargetWorldAddress в [LobbyGameMode]() и соответсвующее поле [DefaultNetSetCustom.ini]()
- <i>Не выполнено "Создайте лобби"</i> — теперь открыть меню лобби можно через соответсвующую кнопку.

Не реализовано: 
- автоматическое объединение в команду при подключении к миру (не требуется для моего проекта); 
- разделение по правам (нет пользователя, который создал комнату и имеет расширенные права). 

**Как сделано**:
1. Лобби
    - Добавлены: [LobbyGameMode](./Project_A/Source/Project_A/Public/Core/LobbyGameMode.h), [LobbyGameState](./Project_A/Source/Project_A/Public/Core/LobbyGameState.h), [LobbyPlayerState](./Project_A/Source/Project_A/Public/Core/LobbyPlayerState.h), [LobbyPlayerController](./Project_A/Source/Project_A/Public/Character/LobbyPlayerController.h), [LobbyHud](./Project_A/Source/UI/Public/LobbyHud.h), 
        - [LobbyWidget](./Project_A/Source/UI/Public/LobbyMainWidget.h) — главный экран при подключении к Lobby-серверу, 
        - [LobbyRoomWidget](./Project_A/Source/UI/Public/LobbyRoomWidget.h) — отдельная комната, в которой можно выбрать персонажа и начать игру.
    - После подключения к лобби-серверу пользователь видит главный экран WBP_LobbyBrowser (/Game/Project_A_Root/Levels/Lobby/Widgets).     
    - Пользователь может создать команту или присоединиться к существующей группе.
    - Список комнат и список подключенных игроков реализованы через ListView.
    - После подключения к комнате скрывается WBP_LobbyBrowser и открывается WBP_LobbyRoom.
    - Добавлены 2 кнопки (как заглушки) для выбора персонажа, так как для входа в игровой мир требуется ID персонажа. Настроено корректное поведение при переключении между этими кнопками и статусом готовности. 
    - Пользователь видит: 
        - список игроков в комнате,
        - статус готовности других игроков; 
        - текущее состояние (ожидание, обртаный отсчёт);
        - мир (просто image). 
    - Пользователь может сообщить о готовности, принудительно начать игру (персонаж будет выбран автоматически) или покинуть комнату.
2. Подключение к игровой сцене
    - выполняется в ALobbyGameMode::[StartMatchTravelForRoom](./Project_A/Source/Project_A/Public/Core/LobbyGameMode.cpp). Вызывается, если обратный отсчет завершился в ALobbyGameMode::HandleRoomCountdownTick или вызван ALobbyGameMode::RequestForceStart.

3. Взаимодействие
    - атака была реализована в [задании 7](#task7) и урон по здоровью в [задании 8](#task8);
    - объединение в группу реализовано с помощью 
        - [IInteractable](./Project_A/Source/Interaction/Public/Interactable.h) — добавлены методы BuildInteractionActions и ExecuteInteractionAction, игрок теперь тоже реализует IInteractable (объект, с которым можно взаимодействовать).
        - [InteractionActionType](./Project_A/Source/Interaction/Public/InteractionActionType.h) — определние того, какие действия можно совершать с объектом.
        - [NetPlayerState](./Project_A/Source/Interaction/Public/NetPlayerState.h) — пока хранит только идентификатор группы. 
        - через AGeneralHud::[ShowInteractionMenu](./Project_A/Source/UI/Public/GeneralHud.h) отображается список действий.
        - С помощью ANetPlayerCharacter::[ExecuteInteractionAction](./Project_A/Source/Project_A/Private/Character/NetPlayerCharacter.cpp) выполнятеся доступное действие. 

</details>
