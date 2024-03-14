// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTpsCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include <Components/WidgetComponent.h>
#include <Net/UnrealNetwork.h>
#include "Kismet/GameplayStatics.h"
#include "NetPlayerAnimInstance.h"
#include "MainWidget.h"
#include "HealthBar.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>
#include "SimpleCube.h"
#include "NetTpsGameMode.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ANetTpsCharacter

ANetTpsCharacter::ANetTpsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// 총이 붙을 컴포넌트 만들자
	compGun = CreateDefaultSubobject<USceneComponent>(TEXT("GUN"));
	compGun->SetupAttachment(GetMesh(), FName(TEXT("GunPosition")));
	compGun->SetRelativeLocation(FVector(-7.144f, 3.68f, 4.136f));
	compGun->SetRelativeRotation(FRotator(3.4f, 75.699f, 6.6424f));

	// HP 위젯 (내 머리위에 있는)
	compHP = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP"));
	compHP->SetupAttachment(RootComponent);


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ANetTpsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// AnimInstance 가져오자
	anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	// 1. 바닥에 깔려있는 Pistol 을 찾자.
	TArray<AActor*> allActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), allActor);
	for (int32 i = 0; i < allActor.Num(); i++)
	{
		if (allActor[i]->GetActorLabel().Contains(TEXT("Pistol")))
		{
			allPistol.Add(allActor[i]);
		}
	}
	// 만약에 나의 Local Player 라면
	if (IsLocallyControlled())
	{
		// Main Widget 생성
		mainWidget = Cast<UMainWidget>(CreateWidget(GetWorld(), mainWidgetFactory));
		mainWidget->AddToViewport();
		mainWidget->ShowPistolUI(false);

		compHP->SetVisibility(false);
	}

	ReloadComplete();

	//// 총알 초기 설정
	//currBulletCnt = maxBulletCnt;

	//// 총알 UI 초기 설정
	//for (int32 i = 0; i < maxBulletCnt; i++)
	//{
	//	mainWidget->AddBullet();
	//}
	
}

void ANetTpsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//PrintNetLog();

	BillboardHP();
}

void ANetTpsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetTpsCharacter, currHP);

}

void ANetTpsCharacter::PrintNetLog()
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

void ANetTpsCharacter::BillboardHP()
{
	AActor* cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	// -카메라의 앞방향
	FVector foward = -cam->GetActorForwardVector();
	// 카메라의 윗방향
	FVector up = cam->GetActorUpVector();
 
	FRotator rot = UKismetMathLibrary::MakeRotFromXZ(foward, up);

	compHP->SetWorldRotation(rot);
}

void ANetTpsCharacter::DieProcess()
{
	// 내것이 아니라면 함수 나가자
	if(!IsLocallyControlled()) return;

	// 화면 회색 처리
	FollowCamera->PostProcessSettings.bOverride_ColorSaturation = true;
	FollowCamera->PostProcessSettings.ColorSaturation = FVector4(0, 0, 0, 1);
}

void ANetTpsCharacter::OnRep_CurrHP()
{
	// 나의 캐릭터라면
	if (IsLocallyControlled())
	{
		// mainwidget 에 있는 healtBar 갱신
		mainWidget->myHealthBar->UpdateHealthBar(currHP, maxHP);
		// 데미지 UI 보이게 하자
		mainWidget->ShowDamageUI();
	}
	else
	{
		// 머리위에 있는 healtBar 갱신
		// compHP 에 셋팅되어 있는 HealthBar 를 가져오자
		UHealthBar* healthbar = Cast<UHealthBar>(compHP->GetWidget());
		// 가져온 HealtBar 의 함수 UpdateHealthBar 호출
		healthbar->UpdateHealthBar(currHP, maxHP);
	}
	

	// 만약에 현재 HP 가 0이면
	if (currHP <= 0)
	{
		// 죽음처리
		anim->isDeath = true;
		// 충돌 안되게
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Movement 컴포넌트 비활성
		GetCharacterMovement()->DisableMovement();
		// 만약에 총들고 있다면 떨구자
		if (IsLocallyControlled() && closestPistol)
		{
			TakePistol();
		}
	}
}

void ANetTpsCharacter::DamageProcess()
{
	// 현재 HP 줄이자
	currHP -= 10;
	
	OnRep_CurrHP();
}

void ANetTpsCharacter::MakeCube()
{
	ServerRPC_MakeCube(compGun->GetComponentLocation(), compGun->GetComponentRotation());
}


void ANetTpsCharacter::ServerRPC_MakeCube_Implementation(FVector pos, FRotator rot)
{
	// 나한테 접한 애들이 큐브 만들어
	//MultiRPC_MakeCube();
	GetWorld()->SpawnActor<ASimpleCube>(cubeFactory, pos, rot);
}

void ANetTpsCharacter::MultiRPC_MakeCube_Implementation()
{
	ASimpleCube* cube = GetWorld()->SpawnActor<ASimpleCube>(cubeFactory, compGun->GetComponentLocation(), compGun->GetComponentRotation());
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANetTpsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetTpsCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetTpsCharacter::Look);

		// 총 잡기
		EnhancedInputComponent->BindAction(TakePistolAction, ETriggerEvent::Started, this, &ANetTpsCharacter::TakePistol);

		// 총 쏘기
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ANetTpsCharacter::Fire);

		// 재장전
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ANetTpsCharacter::Reload);

		EnhancedInputComponent->BindAction(MakeCubeAction, ETriggerEvent::Started, this, &ANetTpsCharacter::MakeCube);


	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANetTpsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANetTpsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ANetTpsCharacter::TakePistol()
{
	ServerRPC_TakePistol();
}

void ANetTpsCharacter::ServerRPC_TakePistol_Implementation()
{
	// 만약에 재장전 중이면 함수를 나가자
	if (isReloading) return;

	// 만약에 총을 들고 있다면
	if (closestPistol)
	{
		closestPistol->SetOwner(nullptr);
		MultiRPC_DetachPistol();
		return;
	}


	// 총을 들고 있지 않다면
	float closestDist = std::numeric_limits<float>::max();
	closestPistol = nullptr;

	for (int32 i = 0; i < allPistol.Num(); i++)
	{
		// 만약에 총이 소유자가 있으면 continue;
		if(allPistol[i]->GetOwner() != nullptr) continue;

		// 1. 모든 Pistol 에서 나와의 거리를 구하자.
		float dist = FVector::Distance(GetActorLocation(), allPistol[i]->GetActorLocation());

		// 내가 집을 수 있는 범위에 있니?
		if (dist > takeGunDist) continue;

		// closestDist 보다 dist 작니?
		if (closestDist > dist)
		{
			// closestDist 를 dist 로 갱신
			closestDist = dist;
			// closestPistol 를 allPistol[i] 로 갱신
			closestPistol = allPistol[i];
			// 총의 owner 설정
			closestPistol->SetOwner(this);
		}
	}

	MultiRPC_AttachPistol(closestPistol);
	//AttachPistol();
}

void ANetTpsCharacter::AttachPistol(AActor* pistol)
{
	// 가까운 총이 없으면 함수를 나가자
	if (pistol == nullptr) return;
	closestPistol = pistol;

	// 물리적인 현상 Off 시켜주자
	auto compMesh = closestPistol->GetComponentByClass<UStaticMeshComponent>();
	compMesh->SetSimulatePhysics(false);

	// 가장 가까운 총을  Mesh -> GunPosition 소켓에 붙이자.
	closestPistol->AttachToComponent(compGun, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	// animInstance 에 있는 hasPistol 을 true
	anim->hasPistol = true;

	// bOrientRotaionToMovement 꺼주자
	GetCharacterMovement()->bOrientRotationToMovement = false;
	// RotationYaw 켜주자
	bUseControllerRotationYaw = true;
	// SprintArm 위치 바꿔주자
	CameraBoom->TargetArmLength = 100;
	CameraBoom->SetRelativeLocation(FVector(-4.33f, 33.8f, 70));

	if (mainWidget)
	{
		//Pistol UI 보이게 하자
		mainWidget->ShowPistolUI(true);
	}
}

void ANetTpsCharacter::MultiRPC_AttachPistol_Implementation(AActor* pistol)
{
	AttachPistol(pistol);
}

void ANetTpsCharacter::DetachPistol()
{
	// 물리적인 현상 On 시켜주자
	auto compMesh = closestPistol->GetComponentByClass<UStaticMeshComponent>();
	compMesh->SetSimulatePhysics(true);
	// closestPistol 을 compGun 떨어져 나가자
	closestPistol->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	// animInstance 에 있는 hasPistol 을 false
	anim->hasPistol = false;

	// bOrientRotaionToMovement 켜주자
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// RotationYaw 꺼주자
	bUseControllerRotationYaw = false;
	// SprintArm 위치 바꿔주자
	CameraBoom->TargetArmLength = 400;
	CameraBoom->SetRelativeLocation(FVector::ZeroVector);

	if (mainWidget)
	{
		//Pistol UI 보이지 않게 하자
		mainWidget->ShowPistolUI(false);
	}
}

void ANetTpsCharacter::MultiRPC_DetachPistol_Implementation()
{
	DetachPistol();
	closestPistol = nullptr;
}

void ANetTpsCharacter::Fire()
{
	ServerRPC_Fire();
}

void ANetTpsCharacter::ServerRPC_Fire_Implementation()
{
	// 총을 들고 있지 않으면 함수를 나가자
	// 총알이 0개면 함수를 나가자
	// 재장전 중에는 함수를 나가자
	if (closestPistol == nullptr || currBulletCnt <= 0 || isReloading) return;

	FHitResult hitInfo;
	FVector startPos = FollowCamera->GetComponentLocation();
	FVector endPos = startPos + FollowCamera->GetForwardVector() * 100000;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	bool isHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECollisionChannel::ECC_Visibility, params);
	
	if (isHit)
	{
		// 만약에 맞은 애가 다른 Player 라면
		ANetTpsCharacter* otherPlayer = Cast<ANetTpsCharacter>(hitInfo.GetActor());
		if (otherPlayer)
		{
			// 데미지 주자
			otherPlayer->DamageProcess();
		}
		// cube 라면
		else if (hitInfo.GetActor()->GetName().Contains(TEXT("BP_Cube")))
		{
		}
	}


	MultiRPC_Fire(isHit, hitInfo.ImpactPoint);
}

void ANetTpsCharacter::MultiRPC_Fire_Implementation(bool isHit, FVector impactPoint)
{
	if (isHit)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pistolEffect, impactPoint, FRotator::ZeroRotator, true);

		
	}

	// 총 쏘는 애니메이션 실행
	PlayAnimMontage(pistolMontage, 2.0f, FName(TEXT("Fire")));

	// 총알 하나 사용!
	currBulletCnt--;
	// 총알 UI 하나 제거
	if (mainWidget)
	{
		mainWidget->RemoveBullet();
	}
}

void ANetTpsCharacter::Reload()
{
	ServerRPC_Reload();
}

void ANetTpsCharacter::ServerRPC_Reload_Implementation()
{
	// 총을 들고 있지 않으면 함수를 나가자
	// 재장전 중이면 함수를 나가자
	if (closestPistol == nullptr || isReloading) return;

	MultiRPC_Reload();
}

void ANetTpsCharacter::MultiRPC_Reload_Implementation()
{
	// 재장전 애니메이션 실행
	PlayAnimMontage(pistolMontage, 1.0f, FName(TEXT("Reload")));

	// 재장전 중
	isReloading = true;
}

void ANetTpsCharacter::ReloadComplete()
{	
	// UI 에 추가해야 하는 총알 갯수
	int32 addBulletCnt = maxBulletCnt - currBulletCnt;

	if (mainWidget)
	{
		for (int32 i = 0; i < addBulletCnt; i++)
			mainWidget->AddBullet();
	}

	currBulletCnt = maxBulletCnt;

	// 재장전 완료
	isReloading = false;
}
