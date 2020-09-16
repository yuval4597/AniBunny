// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ABPlantable;
class UBInventoryComponent;

// OnSettingsChangedEvent
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettingsChangedSignature, bool, bRandomizeRotation);

UCLASS()
class ANIBUNNY_API ABCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	// Sets default values for this character's properties
	ABCharacter();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBInventoryComponent* InventoryComp;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void HandleAction();

	void HandleInteract();

	// Variable used to play anim montage in blueprint (automatically set back to false in BP)
	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	bool bPlayActionAnim;

	// Currently equipped plant, should be nullptr by default
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gardening Mode")
	TSubclassOf<ABPlantable> EquippedPlant;

	// Displayed instead of EquippedPlant when EquippedPlant is nullptr
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gardening Mode")
	TSubclassOf<ABPlantable> NoneEquippedPlant;

	// Used to aim where to plant
	UPROPERTY()
	ABPlantable* PreviewPlant;

	// Plant currently targeting
	UPROPERTY()
	ABPlantable* TargetPlant;

	// Draws plant when aiming to plant
	void SpawnNewPreviewPlant();

	// Allows player to plant flowers
	UPROPERTY(BlueprintReadOnly, Category = "Gardening Mode")
	bool bGardeningModeActive;

	UFUNCTION(BlueprintImplementableEvent)
	void GardeningModeHasChanged();

	// Function which handles toggling the Gardening Mode functionality
	void GardeningModeToggle();

	UPROPERTY(EditDefaultsOnly, Category = "Gardening Mode")
	float GardeningModeCameraHeightChange;

	// Timer responsible for looping trace checking to see where player can plant
	FTimerHandle TimerHandle_PlantTrace;

	void GardeningModeTrace();

	FCollisionQueryParams GardeningModeQueryParams;

	// Trace for when action button is pressed (what the player is targeting)
	FCollisionQueryParams ActionTraceParams;

	FActorSpawnParameters GardeningModeSpawnParams;

	// Hit result for plant trace (uses GardeningMode channel)
	FHitResult PlantTraceHitResult;

	// Hit result for anything else (uses Visibility channel)
	FHitResult ActionTrace;

	void PlantingSpotTrace();

	void HarvestablePlantTrace();

	bool IsValidPlantingSpot();

	// Automatically set to false when Gardening Mode is inactive
	bool bValidPlantingSpot;

	bool IsValidHarvestablePlant();

	bool bValidHarvestablePlant;

	UPROPERTY(EditDefaultsOnly, Category = "Gardening Mode")
	float MinPlantingDistance;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gardening Mode")
	float MaxPlantingDistance;

	float TempYawValue;

	// Updates temporary plant's yaw value
	void RotatePlant(float Value);

	UPROPERTY(EditDefaultsOnly, Category = "Gardening Mode")
	float PlantRotateRate;

	// Cycle forward or back in inventory
	void CycleEquippedForward();

	void CycleEquippedBack();

	// Whether or not to randomize plant local yaw when planting
	void RandomizePlantRotationToggle();

	UPROPERTY(BlueprintReadOnly, Category = "Gardening Mode")
	bool bRandomizePlantRotation;

	float RandomRotationValue;

	void UpdateEquippedPlantAndSpawnPreview();

	// Wrapper function for the inventory component's AddItemToInventory function, calls UpdateEquippedPlantAndSpawnPreview()
	// Can call from Console in-game
	UFUNCTION(Exec, BlueprintCallable, Category = "Player Inventory")
	void AddItemToInventory(TSubclassOf<AActor> AddedItemClass, int32 Amount = 1);

	// Should be called before dying, copies values to the game instance
	UFUNCTION(BlueprintCallable, Category = "Player")
	void PrepareToRespawn();
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	void Respawn();

	// Kills player and prepares for respawn
	UFUNCTION(BlueprintCallable, Category = "Player")
	void Kill();

	// If true, enables platformer style movement
	bool bPlatformerModeActive;

	void DeterminePlatformerModeActive();

	// Sets correct values for movement based on whether or not bPlatformerModeActive
	void InitializeAppropriateMovement();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSettingsChangedSignature OnSettingsChanged;

	// Called by the GameMode class in a new level
	void SetPlatformerModeActive(bool Condition = true);

};
