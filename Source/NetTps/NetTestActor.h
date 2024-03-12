// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetTestActor.generated.h"

UCLASS()
class NETTPS_API ANetTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* compMesh;

	// Owner 설정
	UPROPERTY()
	TArray<class AActor*> allPlayer;
	float searchDistance = 200;
	void FindOwner();

	// 변수 Replicate
	//UPROPERTY(Replicated)
	UPROPERTY(ReplicatedUsing=OnRep_RotYaw)
	float rotYaw = 0;	
	UFUNCTION()
	void OnRep_RotYaw();

	bool bScale = true;
	UPROPERTY(Replicated)
	FVector scaleValue = FVector::ZeroVector;
	void TestScale();

	UPROPERTY()
	class UMaterialInstanceDynamic* mat;
	UPROPERTY(Replicated)
	FLinearColor matColor;
	float currTime = 0;
	void TestColor();

	// RPC 동기화
	float currTime2 = 0;
	void TestLocation();

	UFUNCTION(Server, Reliable)
	void ServerRPC_ChangeLocation();
	UFUNCTION(Client, Reliable)
	void ClientRPC_ChangeLocation(FVector pos);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_ChangeLocation(FVector pos);

public:
	void PrintNetLog();
};
