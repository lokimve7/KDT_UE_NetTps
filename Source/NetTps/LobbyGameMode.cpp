// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "LobbyWidget.h"

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	// lobby widget 만들어서 viewport 에 붙히자
	lobbyWidget = CreateWidget<ULobbyWidget>(GetWorld(), lobbyWidgetFactory);
	lobbyWidget->AddToViewport();

	// 마우스 나오게 하자
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	pc->SetShowMouseCursor(true);
}
