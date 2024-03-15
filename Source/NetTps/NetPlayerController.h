// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API ANetPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// 다시 태어나기
	UFUNCTION()
	void RespawnPlayer();
	UFUNCTION(Server, Reliable)
	void ServerRPC_RespawnPlayer();

	// 관전자 모드 되기
	void ChangeToSpectator();
	UFUNCTION(Server, Reliable)
	void ServerRPC_ChangeToSpectator();

};
