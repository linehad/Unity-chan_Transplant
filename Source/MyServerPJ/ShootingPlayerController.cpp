// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingPlayerController.h"

void AShootingPlayerController::BeginPlay()
{
	Super::BeginPlay();
	bShowMouseCursor = false;
	// SetInputMode�� FInputModeGameAndUI�� �־�θ� ���Ӱ� UI��� �Է��� ������ ���°� �ȴ�.
	SetInputMode(FInputModeGameOnly());
}
