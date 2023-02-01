// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerUserWidget.h"
#include "ServerPlayerController.h"
#include <Components/Button.h>
#include <Components/EditableTextBox.h>
#include "Kismet/GameplayStatics.h"

void UServerUserWidget::NativeConstruct()
{
	server_Button->OnClicked.AddDynamic(this, &UServerUserWidget::ServerButtonCallback);
	client_Button->OnClicked.AddDynamic(this, &UServerUserWidget::ClientButtonCallback);
	ip_EditableTextBox->OnTextChanged.AddDynamic(this, &UServerUserWidget::IP_EditableTextBoxCallback);
}

void UServerUserWidget::ServerButtonCallback()
{
	UGameplayStatics::OpenLevel(this, FName(TEXT("Maze")), true, FString(TEXT("listen")));
}

void UServerUserWidget::ClientButtonCallback()
{
	FName ip = FName(*ip_EditableTextBox->GetText().ToString());
	UGameplayStatics::OpenLevel(this, ip, true);
}

void UServerUserWidget::IP_EditableTextBoxCallback(const FText& Text)
{
	if (ip_EditableTextBox->GetText().ToString().Len() <= 0)
	{
		client_Button->SetIsEnabled(false);
	}
	else {
		client_Button->SetIsEnabled(true);
	}
}