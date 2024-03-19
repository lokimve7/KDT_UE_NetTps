// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* widgetSwitcher;

	// 메인화면
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_MoveCreateSession;
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_MoveSearchSession;

	UFUNCTION()
	void OnClickMoveCreateSession();
	UFUNCTION()
	void OnClickMoveSearchSession();


	// 세션 생성 화면
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* edit_SessionName;
	UPROPERTY(meta = (BindWidget))
	class USlider* slider_PlayerCount;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* text_PlayerCount;
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_CreateSession;
	
	// 세션 참여 화면

	
};
