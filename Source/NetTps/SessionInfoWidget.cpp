// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionInfoWidget.h"

#include <Components/Button.h>

void USessionInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn_JoinSession->OnClicked.AddDynamic(this, &USessionInfoWidget::OnClickJoinSession);
}

void USessionInfoWidget::OnClickJoinSession()
{
	
}
