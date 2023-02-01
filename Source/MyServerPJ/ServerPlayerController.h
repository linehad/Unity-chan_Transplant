// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ServerPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYSERVERPJ_API AServerPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG_Game")
		TSubclassOf<UUserWidget> StartWidget;

	UPROPERTY()
		UUserWidget* CurrentWidget;
	UFUNCTION(Client, Reliable)
		void ChangeWidget(TSubclassOf<UUserWidget> NewWidgetClass);
	void ChangeWidget_Implementation(TSubclassOf<UUserWidget> NewWidgetClass);
};
