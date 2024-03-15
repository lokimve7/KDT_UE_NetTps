// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "NetTpsGameMode.h"
#include <GameFramework/SpectatorPawn.h>
#include <Kismet/KismetSystemLibrary.h>

void ANetPlayerController::RespawnPlayer()
{
	ServerRPC_RespawnPlayer();
}

void ANetPlayerController::ServerRPC_RespawnPlayer_Implementation()
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

void ANetPlayerController::ChangeToSpectator()
{
	ServerRPC_ChangeToSpectator();
}

void ANetPlayerController::ServerRPC_ChangeToSpectator_Implementation()
{
	APawn* player = GetPawn();
	if (player)
	{
		ANetTpsGameMode* gm = Cast<ANetTpsGameMode>(GetWorld()->GetAuthGameMode());

		// 관전자 Pawn 만들자
		ASpectatorPawn* spectator = GetWorld()->SpawnActor<ASpectatorPawn>(gm->SpectatorClass, player->GetActorTransform());

		// 현재 플레이어 Possess 해제
		UnPossess();

		// 관전자 Pawn 을 Possess
		Possess(spectator);

		// 이전 플레이어 삭제
		player->Destroy();

		// 5초 뒤에 다시 살아나자
		UKismetSystemLibrary::K2_SetTimer(this, TEXT("RespawnPlayer"), 5, false);
	}
}
