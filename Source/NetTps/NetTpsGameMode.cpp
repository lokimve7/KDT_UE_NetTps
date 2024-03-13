// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTpsGameMode.h"
#include "NetTpsCharacter.h"
#include "UObject/ConstructorHelpers.h"


ANetTpsGameMode::ANetTpsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

