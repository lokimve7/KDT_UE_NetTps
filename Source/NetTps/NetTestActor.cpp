// Fill out your copyright notice in the Description page of Project Settings.


#include "NetTestActor.h"
#include <Net/UnrealNetwork.h>

// Sets default values
ANetTestActor::ANetTestActor()
{
	PrimaryActorTick.bCanEverTick = true;

	compMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TEXT"));
	SetRootComponent(compMesh);

	// 통신을 가능하게 하자
	bReplicates = true;
}

void ANetTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANetTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintNetLog();

	// 만약에 서버라면 회전 시켜라
	if (HasAuthority())
	{
		AddActorLocalRotation(FRotator(0, 50 * DeltaTime, 0));
		rotYaw = GetActorRotation().Yaw;
	}
	// 클라이언트 라면
	/*else
	{
		FRotator rot = GetActorRotation();
		rot.Yaw = rotYaw;
		SetActorRotation(rot);
	}*/
	
	//크기를 증가시키자
	//만약에 크기가 1.5 보다 커지면
	//크기가 작아지게하자
	//만약에 크기가 0.5보다 작아지면
}

void ANetTestActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetTestActor, rotYaw);
}


void ANetTestActor::OnRep_RotYaw()
{
	FRotator rot = GetActorRotation();
	rot.Yaw = rotYaw;
	SetActorRotation(rot);
}

void ANetTestActor::PrintNetLog()
{
	// Connection 상태
	FString conStr = GetNetConnection() != nullptr ? TEXT("Valid Connect") : TEXT("InValid Connect");
	// 나의 주인 Actor
	FString ownerStr = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");
	// Role
	// ROLE_Authority : 모든 권한을 다 갖고 있다 (로직 구현)
	// ROLE_AutonomousProxy : 제어 (Input) 만 가능하다.
	// ROLE_SimulatedProxy : 보기만 (시뮬레이션만) 가능한다.
	FString localRoleStr = UEnum::GetValueAsString<ENetRole>(GetLocalRole());
	FString remoteRoleStr = UEnum::GetValueAsString<ENetRole>(GetRemoteRole());

	FString log = FString::Printf(TEXT("Connection : %s\nOwner Name : %s\nLocalRole : %s\nRemoteRole : %s"),
		*conStr, *ownerStr, *localRoleStr, *remoteRoleStr);

	DrawDebugString(
		GetWorld(),
		GetActorLocation() + FVector::UpVector * 100,
		log,
		nullptr,
		FColor::Yellow,
		0,
		true,
		1.0);
}

