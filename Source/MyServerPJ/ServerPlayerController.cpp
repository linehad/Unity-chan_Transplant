// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerPlayerController.h"
#include <Blueprint/UserWidget.h>

void AServerPlayerController::BeginPlay()
{
	Super::BeginPlay();
	bShowMouseCursor = true;
	// SetInputMode에 FInputModeGameAndUI를 넣어두면 게임과 UI모두 입력이 가능한 상태가 된다.
	SetInputMode(FInputModeGameAndUI());
}

void AServerPlayerController::ChangeWidget_Implementation(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}