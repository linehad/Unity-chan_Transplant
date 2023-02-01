// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyServerPJGameMode.h"
#include "AimHUD.h"
#include "MyServerPJCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMyServerPJGameMode::AMyServerPJGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/UnityChan"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	HUDClass = AAimHUD::StaticClass();
	
}

void AMyServerPJGameMode::BeginPlay()
{
    Super::BeginPlay();
}