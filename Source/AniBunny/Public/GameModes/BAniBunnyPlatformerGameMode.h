// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BAniBunnyPlatformerGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ANIBUNNY_API ABAniBunnyPlatformerGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

};
