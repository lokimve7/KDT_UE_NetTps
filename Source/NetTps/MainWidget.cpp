// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"

#include <Components/Image.h>
#include <Components/HorizontalBox.h>
#include <Components/Button.h>
#include <Blueprint/WidgetBlueprintLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/PlayerState.h>
#include <Components/EditableTextBox.h>
#include <Components/ScrollBox.h>

#include "NetPlayerController.h"
#include "ChatItemWidget.h"
#include "NetTpsCharacter.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 다시하기 버튼 누르면 실행되는 함수 등록
	btn_Retry->OnClicked.AddDynamic(this, &UMainWidget::OnRetry);
	// 채팅 보내기 함수 등록
	btn_Send->OnClicked.AddDynamic(this, &UMainWidget::OnClickSend);
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

void UMainWidget::OnClickSend()
{
	// 내가 채팅 입력한 내용
	FString chatContent = edit_Chat->GetText().ToString();
	// 채팅 내용이 없으면 함수 나가자
	if(chatContent.IsEmpty()) return;

	auto playerState = UGameplayStatics::GetPlayerState(GetWorld(), 0);
	FString chat = FString::Printf(
		TEXT("[%s] : %s"), 
		*playerState->GetPlayerName(), *chatContent);

	// edit_Chat 의 내용을 지우자
	edit_Chat->SetText(FText());	

	// 서버한테 chat 내용을 전달하자.
	// 나의 Pawn을 찾자
	ANetTpsCharacter* myPlayer = Cast<ANetTpsCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	myPlayer->ServerRPC_SendChat(chat);
}

void UMainWidget::AddChat(FString chat)
{
	// ChatItem 만들어서 내용 추가하자.
	UChatItemWidget* chatItem = CreateWidget<UChatItemWidget>(GetWorld(), chatItemFactory);
	scroll_ChatList->AddChild(chatItem);
	chatItem->SetChat(chat);
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
