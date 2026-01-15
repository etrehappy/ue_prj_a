#include "TargetIndicator.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#include "WeaponPluginLog.h"



ATargetIndicator::ATargetIndicator()
{  
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ArcSpline = CreateDefaultSubobject<USplineComponent>(TEXT("ArcSpline"));
	ArcSpline->SetupAttachment(RootComponent);

	ExplosionDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("ExplosionDecal"));
	ExplosionDecal->SetupAttachment(RootComponent);
	ExplosionDecal->SetVisibility(false);
}

void ATargetIndicator::Tick(float DeltaTime)
{    
	Super::Tick(DeltaTime);

}

void ATargetIndicator::BeginPlay()
{
	Super::BeginPlay();
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));    
}

void ATargetIndicator::Show()
{
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
}

void ATargetIndicator::Hide()
{
	UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

void ATargetIndicator::UpdateIndicator(const FVector& StartLocation, const FVector& InitialVelocity,
	float GravityZ, float ExplosionRadius, int32 MaxSteps, float TimeStep)
{
	//UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
	ComputeArc(StartLocation, InitialVelocity, GravityZ, MaxSteps, TimeStep);

	UpdateSplineMeshes();

	UpdateExplosionDecal(ExplosionRadius);
}

void ATargetIndicator::ComputeArc(const FVector& StartLocation, const FVector& InitialVelocity,
	float GravityZ, int32 MaxSteps, float TimeStep)
{
	//UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
	ArcPoints.Empty();

	if (MaxSteps <= 0 || TimeStep <= 0.f)
	{
		FinalHitLocation = StartLocation;
		FinalHitNormal = FVector::UpVector;
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		FinalHitLocation = StartLocation;
		FinalHitNormal = FVector::UpVector;
		return;
	}

	// Prepare collision query params once
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ComputeArc), true);
	if (AActor* OwnerActor = GetOwner())
	{
		QueryParams.AddIgnoredActor(OwnerActor);
	}
	QueryParams.AddIgnoredActor(this);

	ArcPoints.Reserve(FMath::Max(4, MaxSteps)); // small optimization to reduce reallocations

	FVector Pos = StartLocation;
	FVector Vel = InitialVelocity;

	for (int32 Step = 0; Step < MaxSteps; ++Step)
	{
		ArcPoints.Add(Pos);

		// Semi-implicit integration: apply gravity to velocity, then compute next position
		FVector NextVel = Vel;
		NextVel.Z += GravityZ * TimeStep;
		FVector NextPos = Pos + NextVel * TimeStep;

		// Trace between current and next position
		FHitResult Hit;
		if (World->LineTraceSingleByChannel(Hit, Pos, NextPos, ECC_Visibility, QueryParams))
		{
			ArcPoints.Add(Hit.ImpactPoint);
			FinalHitLocation = Hit.ImpactPoint;
			FinalHitNormal = Hit.ImpactNormal;
			break;
		}

#if WITH_EDITOR
		DrawDebugSphere(World, NextPos, 6.f, 6, FColor::Green, false, 0.1f);
#endif

		// advance state
		Pos = NextPos;
		Vel = NextVel;

		// If this was the final step and nothing hit, record final point
		if (Step == MaxSteps - 1)
		{
			FinalHitLocation = Pos;
			FinalHitNormal = FVector::UpVector;
		}
	}

	// Update spline from computed points
	ArcSpline->ClearSplinePoints(false);
	for (const FVector& P : ArcPoints)
	{
		ArcSpline->AddSplinePoint(P, ESplineCoordinateSpace::World, false);
	}
	ArcSpline->UpdateSpline();
}


void ATargetIndicator::UpdateSplineMeshes()
{
	//UE_LOGFMT(LogWeaponPlugin, Log, "{0} - called", FString(__FUNCTION__));
	
	// Destroy old meshes
	for (auto* Mesh : ArcMeshes)
	{
		if (Mesh)
			Mesh->DestroyComponent();
	}
	ArcMeshes.Empty();

	if (!SplineMesh || ArcPoints.Num() < 2) { return; } // need at least two points to create a spline mesh
		

	const int32 NumSegments = ArcPoints.Num() - 1;
	const float TangentScale = 0.5f;

	ArcMeshes.Reserve(NumSegments);

	for (int32 SegmentIndex = 0; SegmentIndex < NumSegments; ++SegmentIndex)
	{
		const FVector& StartLocation = ArcPoints[SegmentIndex];
		const FVector& EndLocation = ArcPoints[SegmentIndex + 1];

		const FVector Delta = EndLocation - StartLocation;
		const FVector StartTangent = Delta * TangentScale;
		const FVector EndTangent = Delta * TangentScale;
				
		USplineMeshComponent* Mesh = NewObject<USplineMeshComponent>(this);
		Mesh->SetMobility(EComponentMobility::Movable);
		Mesh->SetStaticMesh(SplineMesh);
		if (SplineMaterial)
		{
			Mesh->SetMaterial(0, SplineMaterial);
		}

		Mesh->RegisterComponent();
		Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

		Mesh->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

#if WITH_EDITOR
		Mesh->SetCastShadow(true);
#else
		Mesh->SetCastShadow(false);
#endif

		ArcMeshes.Add(Mesh);
	}
}

void ATargetIndicator::UpdateExplosionDecal(float ExplosionRadius)
{
	if (!ExplosionDecal)
	{
		return;
	}

	ExplosionDecal->SetVisibility(true);
	ExplosionDecal->SetWorldLocation(FinalHitLocation);

	if (!FinalHitNormal.IsZero())
	{
		const FRotator DecalRot = FRotationMatrix::MakeFromX(FinalHitNormal).Rotator();
		ExplosionDecal->SetWorldRotation(DecalRot);

#if WITH_EDITOR || UE_BUILD_DEBUG
		DrawDebugDirectionalArrow(
			GetWorld(),
			FinalHitLocation,
			FinalHitLocation + FinalHitNormal * 50.f,
			10.f,
			FColor::Yellow,
			false,
			0.1f,
			0,
			2.f
		);
#endif
	}
	else
	{
		ExplosionDecal->SetWorldRotation(FRotator::ZeroRotator);
	}

	const float DecalDepth = FMath::Max(32.f, ExplosionRadius * 0.25f); 
	const float DecalXY = ExplosionRadius;
	ExplosionDecal->DecalSize = FVector(DecalDepth, DecalXY, DecalXY);
}






//const int32 NumPoints = ArcPoints.Num();
//
//for (int32 i = 0; i < NumPoints - 1; ++i)
//{
//    FVector StartLocation = ArcPoints[i];
//    FVector EndLocation = ArcPoints[i + 1];
//
//    FVector StartTangent = FVector::ZeroVector;
//    FVector EndTangent = FVector::ZeroVector;
//    const float TangentScale = 0.5f;
//    StartTangent = (EndLocation - StartLocation) * TangentScale;
//    EndTangent = (EndLocation - StartLocation) * TangentScale;
//
//
//    USplineMeshComponent* Mesh = NewObject<USplineMeshComponent>(this);
//    Mesh->SetMobility(EComponentMobility::Movable);
//    Mesh->SetStaticMesh(SplineMesh);
//    Mesh->SetMaterial(0, SplineMaterial);
//    Mesh->RegisterComponent();
//    Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
//
//
//    Mesh->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);
//
//    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//
//#if WITH_EDITOR
//    Mesh->SetCastShadow(true);
//#elif
//    Mesh->SetCastShadow(false);
//#endif
//
//    ArcMeshes.Add(Mesh);
//}