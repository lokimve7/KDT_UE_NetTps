// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include <Components/Button.h>
#include <Components/WidgetSwitcher.h>
#include <Components/Slider.h>
#include <Components/TextBlock.h>
#include <Components/EditableTextBox.h>
#include <Components/ScrollBox.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/PlayerState.h>

#include "NetGameInstance.h"
#include "SessionInfoWidget.h"

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

	// 세션 검색 화면 기능들
	btn_FindSession->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickFindSession);

	gi->onSearchComplete.BindUObject(this, &ULobbyWidget::OnSearchComplete);
}

void ULobbyWidget::OnClickMoveCreateSession()
{
	// widget switcher 를 이용해서 1번째 Widget 이 활성화 되라!
	widgetSwitcher->SetActiveWidgetIndex(1);
	
	APlayerState* ps = UGameplayStatics::GetPlayerState(GetWorld(), 0);
	UE_LOG(LogTemp, Warning, TEXT("Player Name : %s"), *ps->GetPlayerName());
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

void ULobbyWidget::OnSearchComplete(int32 idx, FString info)
{
	// SessionInfoWidget 생성
	auto widget = CreateWidget<USessionInfoWidget>(GetWorld(), sessionInfoWidgetFactory);
	// Scroll_RoomList 에 추가
	scroll_RoomList->AddChild(widget);
	// 만들어진 sessionInfo 에 데이터를 셋팅
	widget->SetInfo(idx, info);
}
