// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingPlayerController.h"

void AShootingPlayerController::BeginPlay()
{
	Super::BeginPlay();
	bShowMouseCursor = false;
	// SetInputMode에 FInputModeGameAndUI를 넣어두면 게임과 UI모두 입력이 가능한 상태가 된다.
	SetInputMode(FInputModeGameOnly());
}
