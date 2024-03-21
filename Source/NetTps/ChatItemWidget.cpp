// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatItemWidget.h"
#include <Components/TextBlock.h>

void UChatItemWidget::SetChat(FString chat)
{
	text_Chat->SetText(FText::FromString(chat));
}
