// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include <Components/Button.h>
#include <Components/WidgetSwitcher.h>

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 메인 화면 기능들
	btn_MoveCreateSession->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickMoveCreateSession);
	btn_MoveSearchSession->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickMoveSearchSession);
}

void ULobbyWidget::OnClickMoveCreateSession()
{
	// widget switcher 를 이용해서 1번째 Widget 이 활성화 되라!
	widgetSwitcher->SetActiveWidgetIndex(1);
}

void ULobbyWidget::OnClickMoveSearchSession()
{
}
