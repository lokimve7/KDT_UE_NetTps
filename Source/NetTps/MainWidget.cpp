// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"

#include <Components/Image.h>
#include <Components/HorizontalBox.h>
#include <Components/Button.h>
#include <Blueprint/WidgetBlueprintLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/PlayerState.h>
#include <Components/EditableTextBox.h>

#include "NetPlayerController.h"
#include "ChatItemWidget.h"
#include <../../../../../../../Source/Runtime/UMG/Public/Components/ScrollBox.h>

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// �ٽ��ϱ� ��ư ������ ����Ǵ� �Լ� ���
	btn_Retry->OnClicked.AddDynamic(this, &UMainWidget::OnRetry);
	// ä�� ������ �Լ� ���
	btn_Send->OnClicked.AddDynamic(this, &UMainWidget::OnClickSend);
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (isShowDamageUI)
	{
		// ������ UI �� ������ ������ �ʰ� ����.	
		// 1. currOpacity �� �پ��� ����
		currOpacity -= InDeltaTime;
		// 2. �� ���� ������ UI �� opacity ������ ����
		damageUI->SetRenderOpacity(currOpacity);
		// 3. currOpacity �� 0���� ���ų� �۾�����
		if (currOpacity <= 0)
		{
			// 4. ������ UI �� �Ⱥ��̰� ����
			isShowDamageUI = false;
		}
	}	
}

void UMainWidget::OnClickSend()
{
	// ���� ä�� �Է��� ����
	FString chatContent = edit_Chat->GetText().ToString();
	// ä�� ������ ������ �Լ� ������
	if(chatContent.IsEmpty()) return;

	auto playerState = UGameplayStatics::GetPlayerState(GetWorld(), 0);
	FString chat = FString::Printf(
		TEXT("[%s] : %s"), 
		*playerState->GetPlayerName(), *chatContent);

	// edit_Chat �� ������ ������
	edit_Chat->SetText(FText());

	// ChatItem ���� ���� �߰�����.
	UChatItemWidget* chatItem = CreateWidget<UChatItemWidget>(GetWorld(), chatItemFactory);
	scroll_ChatList->AddChild(chatItem);
	chatItem->SetChat(chatContent);
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
	
	// �Ѿ� ���� ����
	UUserWidget* bullet = CreateWidget(GetWorld(), bulletFactory);
	// ������ �Ѿ� ������ bulletPanel �߰�����
	bulletPanel->AddChildToHorizontalBox(bullet);
}

void UMainWidget::RemoveBullet()
{
	// bulletPanel �� �ڽ��� ���� - 1
	int32 idx = bulletPanel->GetChildrenCount() - 1;
	// idx ��° �ڽ��� ������
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
	// PlayerController ��������
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

	// ���� ������
	RemoveFromParent();
}
