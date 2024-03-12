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
	//bReplicates = true;
	SetReplicates(true);
}

void ANetTestActor::BeginPlay()
{
	Super::BeginPlay();

	mat = compMesh->CreateDynamicMaterialInstance(0);
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
	
	TestScale();	

	TestColor();
}

void ANetTestActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetTestActor, rotYaw);
	DOREPLIFETIME(ANetTestActor, scaleValue);
	DOREPLIFETIME(ANetTestActor, matColor);
}


void ANetTestActor::OnRep_RotYaw()
{
	FRotator rot = GetActorRotation();
	rot.Yaw = rotYaw;
	SetActorRotation(rot);
}

void ANetTestActor::TestScale()
{
	// 만약에 서버라면
	if (HasAuthority())
	{
		// 1. 만약에 bScaleIncrease true 라면
		if (bScale == true)
		{
			// 1 - 1. FVector 자료형 현재 나의 크기를 변수 담자
			FVector s = GetActorScale3D();
			// 1 - 2. 받은 변수의를 50 * DeltaTime 만큼 증가시키자
			s += FVector(1 * GetWorld()->DeltaTimeSeconds);
			// 1 - 3. s 를 나의 스케일 값으로 하자.
			SetActorScale3D(s);
			// 1 - 4. 만약에 1.5보다 커지면 bScaleIncrease 를 false;
			if (s.X > 1.5f)
			{
				bScale = false;
			}
			scaleValue = s;
		}
		// 2. 그렇지 않으면
		else
		{
			// 1 - 1. FVector 자료형 현재 나의 크기를 변수 담자
			FVector s = GetActorScale3D();
			// 1 - 2. 받은 변수의를 50 * DeltaTime 만큼 감소시키자
			s -= FVector(1 * GetWorld()->DeltaTimeSeconds);
			// 1 - 3. s 를 나의 스케일 값으로 하자.
			SetActorScale3D(s);
			// 1 - 4. 만약에 0.5보다 작아지면 bScaleIncrease 를 true;
			if (s.X < 0.5f)
			{
				bScale = true;
			}
			scaleValue = s;
		}
	}
	else
	{
		SetActorScale3D(scaleValue);
	}
}

void ANetTestActor::TestColor()
{
	// 서버라면 로직처리(2초마다 랜덤한 색 변경)
	if (HasAuthority())
	{
		currTime += GetWorld()->GetDeltaSeconds();
		if (currTime > 2)
		{
			currTime = 0;
			matColor = FLinearColor::MakeRandomColor();
			mat->SetVectorParameterValue(TEXT("FloorColor"), matColor);
		}
	}
	else
	{
		mat->SetVectorParameterValue(TEXT("FloorColor"), matColor);
	}
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

