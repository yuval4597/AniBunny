// Fill out your copyright notice in the Description page of Project Settings.


#include "BCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "BPlantable.h"
#include "BInventoryComponent.h"
#include "BAniBunnyGameInstance.h"
#include "BAniBunnyBaseGameMode.h"
#include "BAniBunnyPlatformerGameMode.h"
#include "AniBunny.h"

#include "DrawDebugHelpers.h"

// Show or hide debug tools for Gardening Mode (disabled by default)
static int32 DebugGardeningMode = 0;
FAutoConsoleVariableRef CVARGardeningModeDebug (TEXT("BUNNY.DebugGardeningMode"), DebugGardeningMode, TEXT("Draw debug shapes for Gardening Mode"), ECVF_Cheat);

// Sets default values
ABCharacter::ABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	bPlatformerModeActive = false;

	// Movement settings that are the same for both base and platformer levels
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->BrakingDecelerationWalking = 0.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Inventory
	InventoryComp = CreateDefaultSubobject<UBInventoryComponent>(TEXT("InventoryComp"));

	// Gardening Mode Initialization
	bGardeningModeActive = false;
	GardeningModeCameraHeightChange = 200.0f;

	GardeningModeQueryParams.AddIgnoredActor(this);
	GardeningModeQueryParams.bReturnPhysicalMaterial = true;

	ActionTraceParams.AddIgnoredActor(this);

	GardeningModeSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	GardeningModeSpawnParams.Owner = this;

	bValidPlantingSpot = false;
	bValidHarvestablePlant = false;
	MinPlantingDistance = 0.0f;		// Used to be minimum planting distance, decided against it. Still functional if want to change.
	MaxPlantingDistance = 250.0f;

	bRandomizePlantRotation = true;
	TempYawValue = 0.0f;
	PlantRotateRate = 10.0f;

	// Animation
	bPlayActionAnim = false;
}

// Called when the game starts or when spawned
void ABCharacter::BeginPlay()
{
	Super::BeginPlay();

	Respawn();

	DeterminePlatformerModeActive();

	// Handle movement
	InitializeAppropriateMovement();
}

// Called every frame
void ABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// PrimaryActorTick.bCanEverTick = false right now, change before writing code here

}

// Called to bind functionality to input
void ABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ABCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ABCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAxis("RotatePlant", this, &ABCharacter::RotatePlant);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ABCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ABCharacter::StopJumping);

	PlayerInputComponent->BindAction("GardeningMode", EInputEvent::IE_Pressed, this, &ABCharacter::GardeningModeToggle);
	PlayerInputComponent->BindAction("Action", EInputEvent::IE_Pressed, this, &ABCharacter::HandleAction);
	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &ABCharacter::HandleInteract);

	PlayerInputComponent->BindAction("CycleEquippedForward", EInputEvent::IE_Pressed, this, &ABCharacter::CycleEquippedForward);
	PlayerInputComponent->BindAction("CycleEquippedBack", EInputEvent::IE_Pressed, this, &ABCharacter::CycleEquippedBack);
	PlayerInputComponent->BindAction("RandomizePlantRotation", EInputEvent::IE_Pressed, this, &ABCharacter::RandomizePlantRotationToggle);
}

void ABCharacter::SetPlatformerModeActive(bool Condition /*= true*/)
{
	bPlatformerModeActive = Condition;
	InitializeAppropriateMovement();
}

void ABCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ABCharacter::HandleAction()
{
	// bValidHarvestableSpot is only true when Gardening Mode is active, no need to check both
	if (bValidHarvestablePlant)
	{
		bPlayActionAnim = true;

		InventoryComp->AddItemToInventory(TargetPlant);

		// Debugging
		FInventoryStack TempStack = InventoryComp->GetItemStack(TargetPlant);
		UE_LOG(InventoryLog, Log, TEXT("Added %s, Current count: %d"), *TempStack.Item->GetName(), TempStack.Count);

		TargetPlant->Destroy();

		UpdateEquippedPlantAndSpawnPreview();
	}
}

void ABCharacter::HandleInteract()
{
	if (EquippedPlant && 
		PreviewPlant &&
		bValidPlantingSpot)		// bValidPlantingSpot is only true when Gardening Mode is active, no need to check both
	{
		bPlayActionAnim = true;

		FVector SpawnLocation = PreviewPlant->GetActorLocation();
		FRotator SpawnRotation = PreviewPlant->GetActorRotation();

		GetWorld()->SpawnActor<ABPlantable>(EquippedPlant, SpawnLocation, SpawnRotation, GardeningModeSpawnParams);
		
		InventoryComp->RemoveItem(PreviewPlant);

		UpdateEquippedPlantAndSpawnPreview();
	}
}

void ABCharacter::SpawnNewPreviewPlant()
{
	FVector SpawnDirection;
	FRotator SpawnRotation;


	if (EquippedPlant)
	{
		// If plant is equipped
		SpawnDirection = PlantTraceHitResult.ImpactNormal;
		SpawnRotation = SpawnDirection.Rotation() + FRotator(-90.0f, 0.0f, 0.0f);

		PreviewPlant = GetWorld()->SpawnActor<ABPlantable>(EquippedPlant, PlantTraceHitResult.ImpactPoint, SpawnRotation, GardeningModeSpawnParams);
		
		// Make sure the preview plant doesn't accidentally grow
		PreviewPlant->SetShouldGrowFalse();

		// Run this function in order to change to correct material before showing the object
		IsValidPlantingSpot();

		// Set RandomRotationValue whenever a new preview plant is spawned
		RandomRotationValue = FMath::FRandRange(0.0f, 359.9f);
	}
	else
	{
		// No plant equipped
		SpawnDirection = ActionTrace.ImpactNormal;
		SpawnRotation = SpawnDirection.Rotation() + FRotator(-90.0f, 0.0f, 0.0f);

		PreviewPlant = GetWorld()->SpawnActor<ABPlantable>(NoneEquippedPlant, ActionTrace.ImpactPoint, SpawnRotation, GardeningModeSpawnParams);

		// Make sure the preview plant doesn't accidentally grow
		PreviewPlant->SetShouldGrowFalse();

		// Run this function in order to change to correct material before showing the object
		IsValidHarvestablePlant();
	}

	// Show grown mesh rather than planted mesh in preview
	PreviewPlant->SetGrownTrue();

	// This plant is never placed on the ground, it should be treated as if it was invisible (e.g. so we won't be able to accidentally harvest it)
	PreviewPlant->GetGrownMeshComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
}

void ABCharacter::GardeningModeToggle()
{
	if (!bGardeningModeActive)
	{
		UE_LOG(GardeningModeLog, Log, TEXT("Gardening Mode Active..."));

		bGardeningModeActive = true;

		CameraBoom->AddRelativeLocation(FVector(0.0f, 0.0f, GardeningModeCameraHeightChange));

		GetWorldTimerManager().SetTimer(TimerHandle_PlantTrace, this, &ABCharacter::GardeningModeTrace, 0.1f, true, 0.0f);

		UpdateEquippedPlantAndSpawnPreview();
	}
	else
	{
		UE_LOG(GardeningModeLog, Log, TEXT("Gardening Mode NOT Active..."));

		bGardeningModeActive = false;
		bValidPlantingSpot = false;

		CameraBoom->AddRelativeLocation(FVector(0.0f, 0.0f, -GardeningModeCameraHeightChange));

		GetWorldTimerManager().ClearTimer(TimerHandle_PlantTrace);

		PreviewPlant->Destroy();
	}

	// Implemented in BP (used for UI)
	GardeningModeHasChanged();
	
}

void ABCharacter::GardeningModeTrace()
{
	PlantingSpotTrace();
	HarvestablePlantTrace();

	bValidPlantingSpot = IsValidPlantingSpot();
	bValidHarvestablePlant = IsValidHarvestablePlant();
}

void ABCharacter::PlantingSpotTrace()
{
	FVector EyesLocation;
	FRotator EyesRotation;
	GetActorEyesViewPoint(EyesLocation, EyesRotation);

	FVector TraceEnd = EyesLocation + 10e3 * EyesRotation.Vector();

	GetWorld()->LineTraceSingleByChannel(PlantTraceHitResult, EyesLocation, TraceEnd, COLLISION_GARDENINGMODE, GardeningModeQueryParams);

	FVector SpawnDirection = PlantTraceHitResult.ImpactNormal;
	FRotator SpawnRotation = SpawnDirection.Rotation() + FRotator(-90.0f, 0.0f, 0.0f);	// a vector of (0,0,1) is converted to a pitch of 90 degrees, but we want 90 degrees to be 0 in world space

	// If no plant is equipped we handle the preview in HarvestablePlantTrace
	if (EquippedPlant)
	{
		PreviewPlant->SetActorLocationAndRotation(PlantTraceHitResult.ImpactPoint, SpawnRotation);

		TempYawValue = FMath::ClampAngle(TempYawValue, -179.9f, 180.0f);

		// If bRandomizePlantRotation is false, we change the value to 0 in order to avoid affecting the rotation
		if (!bRandomizePlantRotation)
		{
			RandomRotationValue = 0.0f;
		}

		// Add rotation set by mouse wheel axis input
		PreviewPlant->AddActorLocalRotation(FRotator(0.0f, RandomRotationValue + TempYawValue, 0.0f));
	}

	// Debugging
	if (DebugGardeningMode > 0)
	{
		DrawDebugLine(GetWorld(), EyesLocation, PlantTraceHitResult.ImpactPoint, FColor::Purple, false, 0.1f, 2, 5.0f);
	}
}

void ABCharacter::HarvestablePlantTrace()
{
	FVector EyesLocation;
	FRotator EyesRotation;
	GetActorEyesViewPoint(EyesLocation, EyesRotation);

	FVector TraceEnd = EyesLocation + 10e3 * EyesRotation.Vector();

	GetWorld()->LineTraceSingleByChannel(ActionTrace, EyesLocation, TraceEnd, ECC_Visibility, ActionTraceParams);

	FVector SpawnDirection = ActionTrace.ImpactNormal;
	FRotator SpawnRotation = SpawnDirection.Rotation() + FRotator(-90.0f, 0.0f, 0.0f);	// a vector of (0,0,1) is converted to a pitch of 90 degrees, but we want 90 degrees to be 0 in world space

	if (!EquippedPlant)
	{
		PreviewPlant->SetActorLocationAndRotation(ActionTrace.ImpactPoint, SpawnRotation);
	}
}

bool ABCharacter::IsValidPlantingSpot()
{
	bool Result = false;

	FColor DebugValidPlantColor = FColor::Red;

	FVector DistanceFromPlayer = PlantTraceHitResult.ImpactPoint - GetActorLocation();

	TArray<UPrimitiveComponent*> OverlappingComps;
	PreviewPlant->GetOverlappingComponents(OverlappingComps);

	if (OverlappingComps.Num() < 1 &&
		DistanceFromPlayer.Size() > MinPlantingDistance &&
		DistanceFromPlayer.Size() < MaxPlantingDistance &&
		UPhysicalMaterial::DetermineSurfaceType(PlantTraceHitResult.PhysMaterial.Get()) == SURFACE_GROUND)
	{
		Result = true;

		PreviewPlant->SetMaterialState(EPlantMaterialState::Valid);

		DebugValidPlantColor = FColor::Blue;
	}
	else
	{
		if (OverlappingComps.Num() >= 1 ||
			DistanceFromPlayer.Size() <= MinPlantingDistance ||
			UPhysicalMaterial::DetermineSurfaceType(PlantTraceHitResult.PhysMaterial.Get()) != SURFACE_GROUND)
		{
			PreviewPlant->SetMaterialState(EPlantMaterialState::Invalid);
		}
		else if(DistanceFromPlayer.Size() >= MaxPlantingDistance)
		{
			PreviewPlant->SetMaterialState(EPlantMaterialState::OutOfRange);
		}
	}
	
	// Debugging
	if (DebugGardeningMode > 0)
	{
		DrawDebugSphere(GetWorld(), PlantTraceHitResult.ImpactPoint, 20.0f, 32, DebugValidPlantColor, false, 0.1f, 3, 2.0f);
		DrawDebugDirectionalArrow(GetWorld(), PlantTraceHitResult.ImpactPoint, PlantTraceHitResult.ImpactPoint + PlantTraceHitResult.ImpactNormal * 100.0f, 5.0f, FColor::Orange, false, 0.1f, 3, 2.0f);
	}

	return Result;
}

bool ABCharacter::IsValidHarvestablePlant()
{
	// In this function we only update the preview plant if there is no EquippedPlant
	bool Result;

	FVector DistanceFromPlayer = ActionTrace.ImpactPoint - GetActorLocation();

	TargetPlant = Cast<ABPlantable>(ActionTrace.GetActor());

	// The hit target is indeed a plant (as opposed to any other object)
	if (TargetPlant &&
		DistanceFromPlayer.Size() < MaxPlantingDistance)
	{
		Result = true;

		if (!EquippedPlant)
		{
			PreviewPlant->SetMaterialState(EPlantMaterialState::Valid);
		}
	}
	else
	{
		Result = false;

		if (DistanceFromPlayer.Size() < MaxPlantingDistance)
		{
			// Within range, just not a valid target
			if (!EquippedPlant)
			{
				PreviewPlant->SetMaterialState(EPlantMaterialState::Invalid);
			}
		}
		else
		{
			// Out of range
			if (!EquippedPlant)
			{
				PreviewPlant->SetMaterialState(EPlantMaterialState::OutOfRange);
			}
		}
	}

	return Result;
}

void ABCharacter::RotatePlant(float Value)
{
	TempYawValue += Value * PlantRotateRate;
}

void ABCharacter::CycleEquippedForward()
{
	if (bGardeningModeActive)
	{
		// Only update if the equipped item changed
		if (InventoryComp->CycleEquippedItemStack())
		{
			UpdateEquippedPlantAndSpawnPreview();
		}
	}
}

void ABCharacter::CycleEquippedBack()
{
	if (bGardeningModeActive)
	{
		// Only update if the equipped item changed
		if (InventoryComp->CycleEquippedItemStack(-1))
		{
			UpdateEquippedPlantAndSpawnPreview();
		}
	}
}

void ABCharacter::RandomizePlantRotationToggle()
{
	if (bGardeningModeActive)
	{
		if (bRandomizePlantRotation)
		{
			bRandomizePlantRotation = false;
		}
		else // !bRandomizePlantRotation
		{
			bRandomizePlantRotation = true;
		}

		UpdateEquippedPlantAndSpawnPreview();

		OnSettingsChanged.Broadcast(bRandomizePlantRotation);
	}
}

void ABCharacter::UpdateEquippedPlantAndSpawnPreview()
{
	int32 Index = InventoryComp->GetEquippedItemStackIndex();
	if (Index == -1)
	{
		// No item to be equipped
		EquippedPlant = nullptr;
	}
	else
	{
		// Change to the currently equipped item in the inventory
		EquippedPlant = InventoryComp->GetItemStack(Index).Item;
	}

	// Create new preview plant (in case EquippedPlant has changed)
	if (PreviewPlant)
	{
		PreviewPlant->Destroy();
	}
	
	SpawnNewPreviewPlant();
}

void ABCharacter::AddItemToInventory(TSubclassOf<AActor> AddedItemClass, int32 Amount /*= 1*/)
{
	InventoryComp->AddItemToInventory(AddedItemClass, Amount);

	if (bGardeningModeActive)
	{
		UpdateEquippedPlantAndSpawnPreview();
	}
}

void ABCharacter::PrepareToRespawn()
{
	if (bGardeningModeActive) {
		GardeningModeToggle();
	}

	UBAniBunnyGameInstance* GI = Cast<UBAniBunnyGameInstance>(GetGameInstance());
	
	if (GI)
	{
		// Copy inventory data
		GI->PlayerInventory = InventoryComp->GetAllItemsInInventory();
		GI->PlayerEquippedItemIndex = InventoryComp->GetEquippedItemStackIndex();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance is nullptr! Could not cast to UBAniBunnyGameInstance."));
	}
}

void ABCharacter::Respawn()
{
	UBAniBunnyGameInstance* GI = Cast<UBAniBunnyGameInstance>(GetGameInstance());

	if (GI)
	{
		// Retrieve inventory data
		InventoryComp->SetAllItemsInInventory(GI->PlayerInventory);
		if (GI->PlayerEquippedItemIndex >= 0)
		{
			InventoryComp->SetEquippedItemStack(GI->PlayerEquippedItemIndex);
			UpdateEquippedPlantAndSpawnPreview();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance is nullptr! Could not cast to UBAniBunnyGameInstance."));
	}
}

void ABCharacter::Kill()
{
	PrepareToRespawn();

	Destroy();
}

void ABCharacter::DeterminePlatformerModeActive()
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(GetWorld());

	if (Cast<ABAniBunnyBaseGameMode>(GM))
	{
		bPlatformerModeActive = false;
	}
	else if (Cast<ABAniBunnyPlatformerGameMode>(GM))
	{
		bPlatformerModeActive = true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode is neither ABAniBunnyBaseGameMode or ABAniBunnyPlatformerGameMode! Movement could not be properly initialized!"));
	}
}

void ABCharacter::InitializeAppropriateMovement()
{
	if (!bPlatformerModeActive)
	{
		// Base map movement settings
		GetCharacterMovement()->MaxWalkSpeed = 350.0f;
		GetCharacterMovement()->MaxAcceleration = 2048.0f;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 270.0f, 0.0f);
		GetCharacterMovement()->AirControl = 0.2f;
		GetCharacterMovement()->BrakingDecelerationFalling = 0.0f;
		JumpMaxCount = 1;
		JumpMaxHoldTime = 0.0f;

	}
	else // bPlatformerModeActive
	{
		// Platformer map movement settings
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		GetCharacterMovement()->MaxAcceleration = 750.0f;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
		GetCharacterMovement()->AirControl = 1.0f;
		GetCharacterMovement()->BrakingDecelerationFalling = 750.0f;
		JumpMaxCount = 2;
		JumpMaxHoldTime = 0.3f;
	}
}
