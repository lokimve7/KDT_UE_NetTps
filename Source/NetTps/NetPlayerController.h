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
	void RespawnPlayer();
};
