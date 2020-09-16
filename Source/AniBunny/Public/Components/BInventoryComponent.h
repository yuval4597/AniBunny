// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BInventoryComponent.generated.h"

// OnInventoryChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryChangedSignature, UBInventoryComponent*, InventoryComp);

// BlueprintType to allow access in BP
USTRUCT(BlueprintType)
struct FInventoryStack
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Stack")
	TSubclassOf<AActor> Item;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory Stack")
	int32 Count;
};

UCLASS( ClassGroup=(BUNNY), meta=(BlueprintSpawnableComponent) )
class ANIBUNNY_API UBInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Code to add new item within the public AddItemToInventory function
	void InitializeNewItem(AActor* AddedItem, int32 Amount = 1);

	void InitializeNewItem(TSubclassOf<AActor> AddedItemClass, int32 Amount = 1);

	// Keeps track of all items in inventory
	TArray<FInventoryStack> ItemsInInventory;

	int32 EquippedItemStackIndex;

	void SetEquippedItemStack(AActor* ActorType);

	void SetEquippedItemStack(TSubclassOf<AActor> ActorClass);

public:	

	void SetEquippedItemStack(int32 Index);

	// Allows us to assign event in BP
	UPROPERTY(BlueprintAssignable, Category = "Inventory Events")
	FOnInventoryChangedSignature OnInventoryChanged;

	// Is the inventory currently empty
	bool IsEmpty();

	void AddItemToInventory(AActor* AddedItem, int32 Amount = 1);

	void AddItemToInventory(TSubclassOf<AActor> AddedItemClass, int32 Amount = 1);

	FInventoryStack GetItemStack(int32 Index);

	FInventoryStack GetItemStack(AActor* ActorType);

	// Get index of ActorType in inventory
	int32 GetItemStackIndex(AActor* ActorType);

	int32 GetItemStackIndex(TSubclassOf<AActor> ActorClass);

	UFUNCTION(BlueprintCallable)
	FInventoryStack GetEquippedItemStack();

	int32 GetEquippedItemStackIndex();

	void RemoveItem(int32 Index, int32 Amount = 1);

	void RemoveItem(AActor* ActorType, int32 Amount = 1);

	// Returns true if EquippedItemStack was changed
	bool CycleEquippedItemStack(int32 CycleBy = 1);

	TArray<FInventoryStack> GetAllItemsInInventory();

	void SetAllItemsInInventory(TArray<FInventoryStack> NewInventory);
};
