// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"

#include <Components/Image.h>
#include <Components/HorizontalBox.h>

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
