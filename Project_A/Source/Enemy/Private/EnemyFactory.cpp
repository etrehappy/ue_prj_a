


#include "EnemyFactory.h"
#include "EnemyBase.h"
#include "EnemyRow.h"
#include "ProjectALog.h"

AEnemyBase* UEnemyFactory::SpawnEnemyByTag(UWorld* World, UDataTable* EnemyDataTable, FGameplayTag EnemyTag, const FVector& Location, const FRotator& Rotation)
{
    if (!World || !EnemyDataTable)
    {        
        UE_LOGFMT(LogProjectA, Warning, "{0} — Invalid World or DataTable", FString(__FUNCTION__));
        return nullptr;
    }

    if(World->GetNetMode() == NM_Client)
    {
        UE_LOGFMT(LogProjectA, Warning, "{0} — SpawnEnemyByTag called on client; spawning must be done on server", FString(__FUNCTION__));
        return nullptr;
    }

    FActorSpawnParameters Params; 
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    TArray<FName> RowNames = EnemyDataTable->GetRowNames();

    for (const auto& RowName : RowNames)
    {
        FEnemyRow* Row = EnemyDataTable->FindRow<FEnemyRow>(RowName, "");

        if (Row && ( Row->EnemyTag == EnemyTag) && Row->EnemyClass)
        {
            AEnemyBase* Enemy = World->SpawnActor<AEnemyBase>(Row->EnemyClass, Location, Rotation, Params);

            if (Enemy)
            {
                Enemy->InitializeFromTableData(*Row);
            }

            return Enemy;
        }
    }

    UE_LOGFMT(LogProjectA, Warning, "{0} — Enemy not found for tag {1}", FString(__FUNCTION__), *EnemyTag.ToString() );
    return nullptr;
}
