// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"

#include <Components/Image.h>
#include <Components/HorizontalBox.h>
#include <Components/Button.h>
#include <Blueprint/WidgetBlueprintLibrary.h>
#include "NetPlayerController.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 다시하기 버튼 누르면 실행되는 함수 등록
	btn_Retry->OnClicked.AddDynamic(this, &UMainWidget::OnRetry);
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (isShowDamageUI)
	{
		// 데미지 UI 를 서서히 보이지 않게 하자.	
		// 1. currOpacity 를 줄어들게 하자
		currOpacity -= InDeltaTime;
		// 2. 그 값을 데미지 UI 의 opacity 값으로 설정
		damageUI->SetRenderOpacity(currOpacity);
		// 3. currOpacity 가 0보다 같거나 작아지면
		if (currOpacity <= 0)
		{
			// 4. 데미지 UI 를 안보이게 하자
			isShowDamageUI = false;
		}
	}	
}

void UMainWidget::ShowPistolUI(bool isShow)
{
	if (isShow)
	{
		crosshair->SetVisibility(ESlateVisibility::Visible);
		bulletPanel->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		crosshair->SetVisibility(ESlateVisibility::Hidden);
		bulletPanel->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainWidget::AddBullet()
{
	
	// 총알 위젯 생성
	UUserWidget* bullet = CreateWidget(GetWorld(), bulletFactory);
	// 생성된 총알 위젯을 bulletPanel 추가하자
	bulletPanel->AddChildToHorizontalBox(bullet);
}

void UMainWidget::RemoveBullet()
{
	// bulletPanel 의 자식의 갯수 - 1
	int32 idx = bulletPanel->GetChildrenCount() - 1;
	// idx 번째 자식을 지우자
	bulletPanel->RemoveChildAt(idx);
}

void UMainWidget::ShowDamageUI()
{
	//isShowDamageUI = true;
	//currOpacity = 1;

	PlayAnimation(damageAnim);
}

void UMainWidget::ShowGameOverUI(bool isShow)
{
	// PlayerController 가져오자
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	pc->SetShowMouseCursor(isShow);

	if (isShow)
	{
		btn_Retry->SetVisibility(ESlateVisibility::Visible);
		btn_Exit->SetVisibility(ESlateVisibility::Visible);		

		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(pc);
	}
	else
	{
		btn_Retry->SetVisibility(ESlateVisibility::Hidden);
		btn_Exit->SetVisibility(ESlateVisibility::Hidden);

		UWidgetBlueprintLibrary::SetInputMode_GameOnly(pc);
	}
}

void UMainWidget::OnRetry()
{
	ShowGameOverUI(false);

	ANetPlayerController* pc = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
	//pc->RespawnPlayer();
	pc->ChangeToSpectator();

	// 나를 지우자
	RemoveFromParent();
}
