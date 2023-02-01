// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerGameModeBase.h"
#include "ServerPlayerController.h"

void AServerGameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void AServerGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AServerPlayerController* Controller = Cast<AServerPlayerController>(NewPlayer);

	if (Controller)
	{
		Controller->ChangeWidget(StartingWidgetClass);
	}
}