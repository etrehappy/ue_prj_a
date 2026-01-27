# Сборка
1. git clone -b 'hw9' --single-branch https://github.com/etrehappy/ue_prj_a.git
2. Скачать [архив ( MB)]() и распаковать с заменой в .\ue_prj_a\ 
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


<details><summary>Задание 6. Создание модуля и плагина <p></p></summary>

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

<details><summary>Задание 7. Оружие и стрельба <p></p></summary>

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
                <img src="./img_for_readme/character_tags.jpg" alt="tags" width="500" height="141">   
    </div>
4. Экипировка оружия (частично реализовано в Задании 6)
    - В [InputComponent](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Public/Character/CustomInputComponent.h) добавлена реакция на Input экипировки (клавиша C для базового оружия и клавиша T для метательного).
    - В C++ [персонаж](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Source/Project_A/Public/Character/NetPlayerCharacter.h) вызывает BlueprintImplementableEvent для экипировки.
    - В [Blueprint персонажа](https://github.com/etrehappy/ue_prj_a/blob/hw7/Project_A/Content/Project_A_Root/Character/Player/BP_PlayerCharacter.uasset) указываются базовые настройки для генерации оружия. Меш персонажа должен иметь подходящий сокет. В учебном проекте универсальность сокетов не доработана, поэтому текущие сокеты подходят для однотипного оружия, а настройки подобраны для конкретного экземпляра. 
        <div style="text-align: center;">
                <img src="./img_for_readme/equip_weapon.jpg" alt="tags" width="216" height="300
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
                <img src="./img_for_readme/ability_component.jpg" alt="ability_component" width="427" height="317
                ">   
        </div>
        <br>
        <div style="text-align: center;">
                <img src="./img_for_readme/ability.jpg" alt="ability" width="425" height="388
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

<details><summary>Задание 8. Компонент здоровья<p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> 800x600](https://drive.google.com/file/d/1X_ZL3i0zRIkcdsAuEyjo1VF1Tolof7mG/view?usp=sharing)

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
                    <img src="./img_for_readme/damage.jpg" alt="damage" width="500" height="208
                    ">   
        </div>
        <br>
        
    
5. Отображение исцеления
    - То же, что для урона. Но при создании текстового [Actor](https://github.com/etrehappy/ue_prj_a/blob/hw8/Project_A/Content/Project_A_Root/Blueprints/DamageTest/UI/BP_FloatingDamage.uasset) передаются другой цвет, Abs-значение, другая позиция.
6. Анимация смерти
    - По аналогии с уроном\исцелением HealthComponent транслирует на сервере о смерти. Далее персонаж\враг реализуют Cosmetic-методы. 
        <div style="text-align: center;">
                <img src="./img_for_readme/death.jpg" alt="death" width="500" height="168
                ">   
        </div>
    - После смерти враг просто исчезает по таймеру, а персонаж удалется и создается новый на стартовой точке.
7. Мишень
    - есть постоянный Pawn на сцене, который наследуется от EnemyBase, но не реализует анимацию смерти
    - враг — был реализован ранее, вызов по клавише Q

</details>


<details><summary>Задание 9. Расширение pickup системы<p></p></summary>

**Что сделано**:

<b>Основное</b>
<br>
<br> [ <--- ВИДЕО ---> ](https://drive.google.com/file/d/1CuxZw_uTpaNWLs4wt6N9_XUH93Rg0Tku/view?usp=sharing)

- [x] Добавлены специализированные типы объектов.
- [x] Реализовано их поднятие
- [x] Реализован учёт в персонаже.
- [x] В лог выводится информацию о поднятых объектах.

Дополнительно: 
- Добавлен простой текстовый popup
- [InventoryComponent](./Project_A/Source/Project_A/Public/InventoryComponent.h)
- [Inventory](./Project_A/Source/Project_A/Public/Inventory.h)

**Как сделано**:
1. Предметы для поднятия: 
     - по аналогии с примером на занятии добавлен [AItemPickup](./Project_A/Source/Project_A/Public/ItemPickup.h), от которого наследуются игровые BP-предметы ([кристалл](./Project_A/Content/Project_A_Root/Levels/FirstMap/Items/BP_CrystalItemPickup.uasset) и [нож](./Project_A/Content/Project_A_Root/Levels/FirstMap/Items/BP_KnifePickup.uasset) )
     - в планах: реализовать поднятие предметов с учетом уже настроенной работы с тегами
2. Поднятие и учет предметов (временное решение, так как нет полноценной системы взаимодействия в C++)
    - в [InputComponent](./Project_A/Source/Project_A/Public/Character/CustomInputComponent.h) добавлен соответсвующий Input (клавиша E)     
    - когда персонаж пересекает SphereComponent [поднимаемого объекта](./Project_A/Source/Project_A/Public/ItemPickup.h) обновляется ссылка на текущий объект взаимодействия у [персонажа](./Project_A/Source/Project_A/Public/Character/NetPlayerCharacter.h) (временное решение)      
    - Input вызывает метод Interact() у персонажа -> PickUp и Destroy у поднимаемого объекта -> AddToInventory у персонажа (временное решение, пока не продуман полноценный инвентарь)
    - AddItem просто добавляет элементы в контейнер и выводит в лог их количество (временное решение) 
3. Popup
    - реализовано с помощью WBP_Hud (аналогично примеру на занятии) и простого текстового WBP_PickupTooltip
    - Hud создается и сохраняется через BP персонажа (временное решение)
    - персонаж реализует [PickUpInterface](./Project_A/Source/Project_A/Public/PickUpInterface.h) (временное решение)

</details>

