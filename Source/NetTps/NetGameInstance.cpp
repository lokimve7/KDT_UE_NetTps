// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"
#include <OnlineSubsystem.h>
#include <OnlineSessionSettings.h>
#include <Interfaces/OnlineSessionInterface.h>
#include <Online/OnlineSessionNames.h>

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
		sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnDestroySessionComplete);
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetGameInstance::OnFindSessionComplete);
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

	sessionInterface->CreateSession(*netID, FName(mySessionName), sessionSettings);
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

void UNetGameInstance::DestroyMySession()
{
	sessionInterface->DestroySession(FName(mySessionName));
}

void UNetGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnDestroySessionComplete Success -- %s"), *SessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnDestroySessionComplete Fail"));
	}
}

void UNetGameInstance::FindOtherSession()
{
	sessionSearch = MakeShared<FOnlineSessionSearch>();

	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	sessionSearch->MaxSearchResults = 10;

	// 세션 검색 요청
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void UNetGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		auto results = sessionSearch->SearchResults;
		UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete Success - count : %d"), results.Num());

		for (int32 i = 0; i < results.Num(); i++)
		{
			FOnlineSessionSearchResult si = results[i];
			FString roomName;
			si.Session.SessionSettings.Get(FName(TEXT("ROOM_NAME")), roomName);
			UE_LOG(LogTemp, Warning, TEXT("%d name : %s"), i, *roomName);
		}

		/*for (auto si : results)
		{
			FString roomName;
			si.Session.SessionSettings.Get(FName(TEXT("ROOM_NAME")), roomName);
		}*/
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete Fail"));
	}
}
