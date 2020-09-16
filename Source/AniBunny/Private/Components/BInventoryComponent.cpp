// Fill out your copyright notice in the Description page of Project Settings.


#include "BInventoryComponent.h"
#include "AniBunny.h"

// Sets default values for this component's properties
UBInventoryComponent::UBInventoryComponent()
{
	EquippedItemStackIndex = -1;
}


// Called when the game starts
void UBInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UBInventoryComponent::InitializeNewItem(AActor* AddedItem, int32 Amount /* = 1*/)
{
	if (Amount < 1)
	{
		Amount = 1;
	}

	FInventoryStack TempStack;

	// Spawn new item of the same class as AddedItem and add to inventory (AddedItem can later be deleted without affecting our inventory)
	TempStack.Item = AddedItem->GetClass();
	TempStack.Count = Amount;

	ItemsInInventory.Add(TempStack);
}

void UBInventoryComponent::InitializeNewItem(TSubclassOf<AActor> AddedItemClass, int32 Amount /*= 1*/)
{
	if (Amount < 1)
	{
		Amount = 1;
	}

	FInventoryStack TempStack;

	TempStack.Item = AddedItemClass;
	TempStack.Count = Amount;

	ItemsInInventory.Add(TempStack);
}

bool UBInventoryComponent::IsEmpty()
{
	if (ItemsInInventory.Num() == 0)
	{
		return true;
	}

	return false;
}

void UBInventoryComponent::AddItemToInventory(AActor* AddedItem, int32 Amount /* = 1*/)
{
	if (Amount < 1)
	{
		Amount = 1;
	}

	UE_LOG(InventoryLog, Log, TEXT("AddedItem's class is %s"), *AddedItem->GetClass()->GetName());

	// If no items in inventory
	if (IsEmpty())
	{
		UE_LOG(InventoryLog, Log, TEXT("Inventory is empty, adding new item and setting it to equipped."));
		InitializeNewItem(AddedItem, Amount);
		SetEquippedItemStack(AddedItem);		// OnInventoryChanged is already being broadcasted here, no need to broadcast twice
	}
	else
	{
		UE_LOG(InventoryLog, Log, TEXT("Inventory is not empty, checking."));

		bool bFoundMatch = false;

		for (int32 Index = 0; Index < ItemsInInventory.Num(); Index++)
		{
			// There is already an item of this type in the inventory
			if (ItemsInInventory[Index].Item == AddedItem->GetClass())
			{
				UE_LOG(InventoryLog, Log, TEXT("Found match."));
				ItemsInInventory[Index].Count += Amount;

				bFoundMatch = true;
			}
		}

		if (!bFoundMatch)
		{
			UE_LOG(InventoryLog, Log, TEXT("Inventory is not empty, but no match. Initializing new item."));
			InitializeNewItem(AddedItem, Amount);
		}

		OnInventoryChanged.Broadcast(this);
	}
}

void UBInventoryComponent::AddItemToInventory(TSubclassOf<AActor> AddedItemClass, int32 Amount /*= 1*/)
{
	if (!AddedItemClass)
	{
		return;
	}

	if (Amount < 1)
	{
		Amount = 1;
	}

	UE_LOG(InventoryLog, Log, TEXT("AddedItem's class is %s"), *AddedItemClass->GetName());

	// If no items in inventory
	if(IsEmpty())
	{
		UE_LOG(InventoryLog, Log, TEXT("Inventory is empty, adding new item and setting it to equipped."));
		InitializeNewItem(AddedItemClass, Amount);
		SetEquippedItemStack(AddedItemClass);		// OnInventoryChanged is already being broadcasted here, no need to broadcast twice
	}
	else
	{
		UE_LOG(InventoryLog, Log, TEXT("Inventory is not empty, checking."));

		bool bFoundMatch = false;

		for (int32 Index = 0; Index < ItemsInInventory.Num(); Index++)
		{
			// There is already an item of this type in the inventory
			if (ItemsInInventory[Index].Item == AddedItemClass)
			{
				UE_LOG(InventoryLog, Log, TEXT("Found match."));
				ItemsInInventory[Index].Count += Amount;

				bFoundMatch = true;
			}
		}

		if (!bFoundMatch)
		{
			UE_LOG(InventoryLog, Log, TEXT("Inventory is not empty, but no match. Initializing new item."));
			InitializeNewItem(AddedItemClass, Amount);
		}

		OnInventoryChanged.Broadcast(this);
	}
}

FInventoryStack UBInventoryComponent::GetItemStack(int32 Index)
{
	FInventoryStack TempStack;
	
	if (Index >= 0 && Index < ItemsInInventory.Num())
	{
		TempStack = ItemsInInventory[Index];

		return TempStack;
	}

	TempStack.Item = nullptr;
	TempStack.Count = 0;

	return TempStack;
}

FInventoryStack UBInventoryComponent::GetItemStack(AActor* ActorType)
{
	FInventoryStack TempStack;

	for (int32 Index = 0; Index < ItemsInInventory.Num(); Index++)
	{
		// There is already an item of this type in the inventory
		if (ItemsInInventory[Index].Item == ActorType->GetClass())
		{
			UE_LOG(InventoryLog, Log, TEXT("Found item at index %d"), Index);

			TempStack = ItemsInInventory[Index];

			return TempStack;
		}
	}

	TempStack.Item = nullptr;
	TempStack.Count = 0;

	return TempStack;
}

int32 UBInventoryComponent::GetItemStackIndex(AActor* ActorType)
{
	for (int32 Index = 0; Index < ItemsInInventory.Num(); Index++)
	{
		// There is already an item of this type in the inventory
		if (ItemsInInventory[Index].Item == ActorType->GetClass())
		{
			return Index;
		}
	}

	// None found
	return -1;
}

int32 UBInventoryComponent::GetItemStackIndex(TSubclassOf<AActor> ActorClass)
{
	for (int32 Index = 0; Index < ItemsInInventory.Num(); Index++)
	{
		// There is already an item of this type in the inventory
		if (ItemsInInventory[Index].Item == ActorClass)
		{
			return Index;
		}
	}

	// None found
	return -1;
}

void UBInventoryComponent::SetEquippedItemStack(int32 Index)
{
	if (Index >= 0 && Index < ItemsInInventory.Num())
	{
		EquippedItemStackIndex = Index;
	}

	OnInventoryChanged.Broadcast(this);
}

void UBInventoryComponent::SetEquippedItemStack(AActor* ActorType)
{
	int32 Index = GetItemStackIndex(ActorType);

	if (Index >= 0 && Index < ItemsInInventory.Num())
	{
		EquippedItemStackIndex = Index;
	}

	OnInventoryChanged.Broadcast(this);
}

void UBInventoryComponent::SetEquippedItemStack(TSubclassOf<AActor> ActorClass)
{
	int32 Index = GetItemStackIndex(ActorClass);

	if (Index >= 0 && Index < ItemsInInventory.Num())
	{
		EquippedItemStackIndex = Index;
	}

	OnInventoryChanged.Broadcast(this);
}

FInventoryStack UBInventoryComponent::GetEquippedItemStack()
{
	if (EquippedItemStackIndex >= 0 && EquippedItemStackIndex < ItemsInInventory.Num())
	{
		return GetItemStack(EquippedItemStackIndex);
	}

	FInventoryStack TempStack;
	
	TempStack.Item = nullptr;
	TempStack.Count = 0;

	return TempStack;
}

int32 UBInventoryComponent::GetEquippedItemStackIndex()
{
	return EquippedItemStackIndex;
}

void UBInventoryComponent::RemoveItem(int32 Index, int32 Amount /* = 1*/)
{
	if (Amount < 1)
	{
		Amount = 1;
	}

	if (Index >= 0 && Index < ItemsInInventory.Num())
	{
		ItemsInInventory[Index].Count -= Amount;
		if (ItemsInInventory[Index].Count < 1)
		{
			ItemsInInventory.RemoveAt(Index);
		}
	}

	if (IsEmpty())
	{
		EquippedItemStackIndex = -1;
	}
	else if (EquippedItemStackIndex >= ItemsInInventory.Num())
	{
		EquippedItemStackIndex = 0;
	}

	OnInventoryChanged.Broadcast(this);
}

void UBInventoryComponent::RemoveItem(AActor* ActorType, int32 Amount /* = 1*/)
{
	if (Amount < 1)
	{
		Amount = 1;
	}

	int32 Index = GetItemStackIndex(ActorType);

	// Not -1, therefore there is such an item in the inventory currently
	if (Index >= 0)
	{
		ItemsInInventory[Index].Count -= Amount;
		if (ItemsInInventory[Index].Count < 1)
		{
			ItemsInInventory.RemoveAt(Index);
		}
	}

	if (IsEmpty())
	{
		EquippedItemStackIndex = -1;
	}
	else if (EquippedItemStackIndex >= ItemsInInventory.Num())
	{
		EquippedItemStackIndex = 0;
	}

	OnInventoryChanged.Broadcast(this);
}

bool UBInventoryComponent::CycleEquippedItemStack(int32 CycleBy /* = 1*/)
{
	// Circular array index implementation (can be positive or negative)
	int32 InventorySize = ItemsInInventory.Num();
	if (InventorySize <= 0)
	{
		return false;
	}

	int32 CurrentIndex = EquippedItemStackIndex;
	int32 NewIndex = (CurrentIndex + CycleBy) % InventorySize;

	if (NewIndex < 0)
	{
		NewIndex += InventorySize;
	}

	if (EquippedItemStackIndex != NewIndex)
	{
		EquippedItemStackIndex = NewIndex;
		OnInventoryChanged.Broadcast(this);

		return true;
	}

	return false;

}

TArray<FInventoryStack> UBInventoryComponent::GetAllItemsInInventory()
{
	return ItemsInInventory;
}

void UBInventoryComponent::SetAllItemsInInventory(TArray<FInventoryStack> NewInventory)
{
	ItemsInInventory = NewInventory;
	OnInventoryChanged.Broadcast(this);
}

