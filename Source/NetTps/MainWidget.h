// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
public:
	UPROPERTY(meta = (BindWidget))
	class UImage* crosshair;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* bulletPanel;

	UPROPERTY(meta = (BindWidget))
	class UHealthBar* myHealthBar;

	UPROPERTY(meta = (BindWidget))
	class UImage* damageUI;
	float currOpacity = 0;
	bool isShowDamageUI = false;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* damageAnim;

	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Retry;
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Exit;


	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> bulletFactory;

public:
	void ShowPistolUI(bool isShow);
	void AddBullet();
	void RemoveBullet();
	void ShowDamageUI();
	void ShowGameOverUI(bool isShow);

	UFUNCTION()
	void OnRetry();
};
