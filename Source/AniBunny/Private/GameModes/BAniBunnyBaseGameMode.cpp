// Fill out your copyright notice in the Description page of Project Settings.


#include "BAniBunnyBaseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BCharacter.h"

void ABAniBunnyBaseGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	ABCharacter* PlayerPawn = Cast<ABCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (PlayerPawn)
	{
		PlayerPawn->SetPlatformerModeActive(false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BAniBunnyBaseGameMode could not find PlayerPawn! PlayerPawn = null."));
	}
}
