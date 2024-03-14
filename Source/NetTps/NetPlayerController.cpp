// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "NetTpsGameMode.h"

void ANetPlayerController::RespawnPlayer()
{
	// 해당 Controller 가 컨트롤 하고 있는 Pawn
	APawn* player = GetPawn();

	// 빙의 나가자
	UnPossess();

	// 이전 Player 를 삭제
	player->Destroy();

	// 새로운 Player 를 만들어서 다시 시작
	ANetTpsGameMode* gm = Cast<ANetTpsGameMode>(GetWorld()->GetAuthGameMode());
	gm->RestartPlayer(this);
}
