// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <Interfaces/OnlineSessionInterface.h>
#include "NetGameInstance.generated.h"



/**
 * 
 */

 DECLARE_DELEGATE_TwoParams(FSearchComplete, int32, FString);

UCLASS()
class NETTPS_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

	// 세션 만드는 함수
	UFUNCTION(BlueprintCallable)
	void CreateMySession(FString roomName, int32 maxPlayer);
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	// 세션 파괴하는 함수
	UFUNCTION(BlueprintCallable)
	void DestroyMySession();
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	// 세션을 검색 함수
	UFUNCTION(BlueprintCallable)
	void FindOtherSession();
	void OnFindSessionComplete(bool bWasSuccessful);

	// 세션 참여 함수
	UFUNCTION(BlueprintCallable)
	void JoinOtherSession(int32 idx);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type result);


	FString StringBase64Encode(FString str);
	FString StringBase64Decode(FString str);


public:
	// 이 변수통해 (세션 만들고, 세션 검색, 세션 참여)
	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> sessionInterface;

	// 세션 검색에 쓰이는 클래스
	TSharedPtr<class FOnlineSessionSearch> sessionSearch;

	// 세션 이름
	FString mySessionName = TEXT("lokimve7");

	// 세션 검색이 완료되면 호출해야 하는 Delegate
	FSearchComplete onSearchComplete;
};
