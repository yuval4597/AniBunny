// Fill out your copyright notice in the Description page of Project Settings.


#include "BPlantable.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "AniBunny.h"

// Sets default values
ABPlantable::ABPlantable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Start by having GrownMeshComp ignoring collisions, later we will change that when it isn't hidden in game anymore
	GrownMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrownMeshComp"));
	GrownMeshComp->SetCollisionObjectType(COLLISIONOBJECT_PLANT);
	GrownMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GrownMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	GrownMeshComp->SetCollisionResponseToChannel(COLLISIONOBJECT_PLANT, ECR_Overlap);	// Overlaps with plants even when not spawned yet
	GrownMeshComp->SetHiddenInGame(true);
	RootComponent = GrownMeshComp;

	PlantedMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlantedMeshComp"));
	GrownMeshComp->SetCollisionObjectType(COLLISIONOBJECT_PLANT);
	PlantedMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PlantedMeshComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	PlantedMeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	PlantedMeshComp->SetupAttachment(RootComponent);

	// Plant settings
	bSpawnGrown = false;
	bShouldGrow = true;
	TimeUntilGrown = 10.0f;
}

// Called when the game starts or when spawned
void ABPlantable::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnGrown)
	{
		SetGrownTrue();
		bShouldGrow = false;
	}

	if (bShouldGrow)
	{
		FTimerDelegate TimeUntilGrown_Delegate = FTimerDelegate::CreateUObject(this, &ABPlantable::SetGrownTrue, true);
		GetWorldTimerManager().SetTimer(TimerHandle_TimeUntilGrown, TimeUntilGrown_Delegate, TimeUntilGrown, false);
	}
}

void ABPlantable::SetShouldGrowFalse()
{
	bShouldGrow = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeUntilGrown);
}

void ABPlantable::SetGrowTimeRemaining(float NewTime)
{
	if (NewTime < 0.01f)
	{
		SetShouldGrowFalse();
		SetGrownTrue();
		return;
	}

	FTimerDelegate TimeUntilGrown_Delegate = FTimerDelegate::CreateUObject(this, &ABPlantable::SetGrownTrue, true);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeUntilGrown, TimeUntilGrown_Delegate, NewTime, false);
}

float ABPlantable::GetGrowTimeRemaining()
{
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_TimeUntilGrown);
}

// Called every frame
void ABPlantable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// PrimaryActorTick.bCanEverTick = false right now, change before writing code here
}

void ABPlantable::SetMaterialState(EPlantMaterialState DesiredMaterialState)
{
	switch (DesiredMaterialState)
	{
	case EPlantMaterialState::Invalid:
		GrownMeshComp->SetMaterial(0, InvalidLocationMaterial);
		break;
	case EPlantMaterialState::Valid:
		GrownMeshComp->SetMaterial(0, ValidLocationMaterial);
		break;
	case EPlantMaterialState::OutOfRange:
		GrownMeshComp->SetMaterial(0, OutOfRangeMaterial);
		break;
	case EPlantMaterialState::Planted:
	default:
		GrownMeshComp->SetMaterial(0, PlantedMaterial);
	}
}

UStaticMeshComponent* ABPlantable::GetPlantedMeshComponent()
{
	return PlantedMeshComp;
}

UStaticMeshComponent* ABPlantable::GetGrownMeshComponent()
{
	return GrownMeshComp;
}

void ABPlantable::SetGrownTrue(bool Condition /*= true*/)
{
	PlantedMeshComp->SetHiddenInGame(true);
	PlantedMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	GrownMeshComp->SetHiddenInGame(false);
	GrownMeshComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	GrownMeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

