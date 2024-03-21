// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UChatItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* text_Chat;

public:
	void SetChat(FString chat);
};
