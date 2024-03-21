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
	btn_Exit_CreateSession->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickExit);

	// 세션 검색 화면 기능들
	btn_FindSession->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickFindSession);
	gi->onSearchComplete.BindUObject(this, &ULobbyWidget::OnSearchComplete);
	btn_Exit_RoomList->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickExit);
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
	// 바로 검색 시작
	OnClickFindSession();
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
	//scroll_RoomList 의 자식들을 지우자
	scroll_RoomList->ClearChildren();

	/*for(int32 i = 0; i < scroll_RoomList->GetChildrenCount(); i++)
		scroll_RoomList->RemoveChildAt(0);*/
	
	gi->FindOtherSession();

	text_FindSession->SetText(FText::FromString(TEXT("검색중...")));
	btn_FindSession->SetIsEnabled(false);
}

void ULobbyWidget::OnSearchComplete(int32 idx, FString info)
{
	if (idx < 0)
	{
		text_FindSession->SetText(FText::FromString(TEXT("세션 검색")));
		btn_FindSession->SetIsEnabled(true);
	}
	else
	{
		// SessionInfoWidget 생성
		auto widget = CreateWidget<USessionInfoWidget>(GetWorld(), sessionInfoWidgetFactory);
		// Scroll_RoomList 에 추가
		scroll_RoomList->AddChild(widget);
		// 만들어진 sessionInfo 에 데이터를 셋팅
		widget->SetInfo(idx, info);
	}
}

void ULobbyWidget::OnClickExit()
{
	widgetSwitcher->SetActiveWidgetIndex(0);
}
