// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BPlantable.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;

UENUM()
enum class EPlantMaterialState
{
	Valid,
	Invalid,
	OutOfRange,
	Planted,
};

UCLASS()
class ANIBUNNY_API ABPlantable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABPlantable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* GrownMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PlantedMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Plant Materials")
	UMaterialInterface* InvalidLocationMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Plant Materials")
	UMaterialInterface* ValidLocationMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Plant Materials")
	UMaterialInterface* OutOfRangeMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Plant Materials")
	UMaterialInterface* PlantedMaterial;

	// If true, plant will grow
	UPROPERTY(EditAnywhere, Category = "Plant Settings", meta = (EditCondition = "!bSpawnGrown"))
	bool bShouldGrow;

	// Time until the plant fully grows
	UPROPERTY(EditDefaultsOnly, Category = "Plant Settings")
	float TimeUntilGrown;

	FTimerHandle TimerHandle_TimeUntilGrown;

	// If true, will spawn as grown rather than planted
	UPROPERTY(EditAnywhere, Category = "Plant Settings", meta = (EditCondition = "!bShouldGrow"))
	bool bSpawnGrown;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetMaterialState(EPlantMaterialState DesiredMaterialState);

	UStaticMeshComponent* GetPlantedMeshComponent();

	UFUNCTION(BlueprintCallable, Category = "Static Mesh")
	UStaticMeshComponent* GetGrownMeshComponent();

	// Have grown mesh showing instead of planted
	void SetGrownTrue(bool Condition = true);

	// Stop plant from growing
	void SetShouldGrowFalse();

	UFUNCTION(BlueprintCallable, Category = "Plant Functions")
	void SetGrowTimeRemaining(float NewTime);

	UFUNCTION(BlueprintCallable, Category = "Plant Functions")
	float GetGrowTimeRemaining();
};
