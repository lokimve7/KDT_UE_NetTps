// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API USessionInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	int32 sessionIdx;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* text_SessionInfo;

	UPROPERTY(meta = (BindWidget))
	class UButton* btn_JoinSession;

	UFUNCTION()
	void OnClickJoinSession();

	void SetInfo(int32 idx, FString info);
};
