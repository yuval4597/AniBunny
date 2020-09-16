// Fill out your copyright notice in the Description page of Project Settings.


#include "BAniBunnyPlatformerGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BCharacter.h"

void ABAniBunnyPlatformerGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	ABCharacter* PlayerPawn = Cast<ABCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (PlayerPawn)
	{
		PlayerPawn->SetPlatformerModeActive(true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BAniBunnyBaseGameMode could not find PlayerPawn! PlayerPawn = null."));
	}
}
