// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYSERVERPJ_API UServerUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
		class UButton* server_Button = nullptr;
	UPROPERTY(meta = (BindWidget))
		class UButton* client_Button = nullptr;
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* ip_EditableTextBox = nullptr;

	UFUNCTION(BlueprintCallable)
		void ServerButtonCallback();
	UFUNCTION(BlueprintCallable)
		void ClientButtonCallback();
	UFUNCTION(BlueprintCallable)
		void IP_EditableTextBoxCallback(const FText& Text);
};
