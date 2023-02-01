// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyServerPJGameMode.generated.h"

UCLASS(minimalapi)
class AMyServerPJGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyServerPJGameMode();

protected:
    /** Called when the game starts. */
    virtual void BeginPlay() override;

};



