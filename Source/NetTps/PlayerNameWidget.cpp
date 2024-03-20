// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerNameWidget.h"

#include <Components/TextBlock.h>

void UPlayerNameWidget::SetNickName(FString nickName)
{
	text_NickName->SetText(FText::FromString(nickName));
}
