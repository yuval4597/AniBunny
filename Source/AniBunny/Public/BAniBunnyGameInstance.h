// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BInventoryComponent.h"
#include "BAniBunnyGameInstance.generated.h"


/**
 * Custom Game Instance to store inventory items and possibly other variables
 */

UCLASS()
class ANIBUNNY_API UBAniBunnyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UBAniBunnyGameInstance();

	TArray<FInventoryStack> PlayerInventory;
	int32 PlayerEquippedItemIndex;
};
