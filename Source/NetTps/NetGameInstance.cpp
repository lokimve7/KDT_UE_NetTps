// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"
#include <OnlineSubsystem.h>
#include <OnlineSessionSettings.h>
#include <Interfaces/OnlineSessionInterface.h>

// OnlineSessionInterface 통해서 한다.
// 세션을 만든다. 
// 세션을 검색
// 세션을 참여

void UNetGameInstance::Init()
{
	Super::Init();

	// 온라인 서브 시스템 가져오자
	IOnlineSubsystem* subsys = IOnlineSubsystem::Get();
	if (subsys)
	{
		// 세션 인터페이스 가져오자
		sessionInterface = subsys->GetSessionInterface();
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnCreateSessionComplete);
	}
}

void UNetGameInstance::CreateMySession()
{
	FOnlineSessionSettings sessionSettings;

	// true 세션이 검색 된다.
	sessionSettings.bShouldAdvertise = true;

	// steam 사용하면 해당 옵션이 true 세션을 만들 수 있다.
	sessionSettings.bUseLobbiesIfAvailable = true;

	// 인원 수 
	sessionSettings.NumPublicConnections = 10;

	// 커스텀 옵션
	sessionSettings.Set(FName(TEXT("ROOM_NAME")), FString(TEXT("KHJ_ROOM")), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// 세션 생성 요청
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	sessionInterface->CreateSession(*netID, FName(TEXT("lokimve7")), sessionSettings);
}

void UNetGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete Success -- %s"), *SessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete Fail"));
	}
}
