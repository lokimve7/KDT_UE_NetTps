// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include <Components/Button.h>
#include <Components/WidgetSwitcher.h>
#include <Components/Slider.h>
#include <Components/TextBlock.h>
#include <Components/EditableTextBox.h>

#include "NetGameInstance.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// game instance 가져오자
	gi = Cast<UNetGameInstance>(GetGameInstance());

	

	// 메인 화면 기능들
	btn_MoveCreateSession->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickMoveCreateSession);
	btn_MoveSearchSession->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickMoveSearchSession);


	// 세션 생성 화면 기능들

	// text_PlayerCount 를 초기값이 2로 설정
	text_PlayerCount->SetText(FText::AsNumber(slider_PlayerCount->GetValue()));
	// Slider 의 값이 변경될 때 함수 등록
	slider_PlayerCount->OnValueChanged.AddDynamic(this, &ULobbyWidget::OnValueChanged);

	btn_CreateSession->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickCreateSession);
}

void ULobbyWidget::OnClickMoveCreateSession()
{
	// widget switcher 를 이용해서 1번째 Widget 이 활성화 되라!
	widgetSwitcher->SetActiveWidgetIndex(1);
}

void ULobbyWidget::OnClickMoveSearchSession()
{
	// widget switcher 를 이용해서 2번째 Widget 이 활성화 되라!
	widgetSwitcher->SetActiveWidgetIndex(2);
}

void ULobbyWidget::OnValueChanged(float Value)
{
	// player count 텍스트 갱신
	text_PlayerCount->SetText(FText::AsNumber(Value));
}

void ULobbyWidget::OnClickCreateSession()
{
	gi->CreateMySession(
			edit_SessionName->GetText().ToString(), 
			slider_PlayerCount->GetValue());
}

void ULobbyWidget::OnClickFindSession()
{
	gi->FindOtherSession();
}
