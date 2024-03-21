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
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>

#include "NetPlayerAnimInstance.h"
#include "MainWidget.h"
#include "HealthBar.h"
#include "SimpleCube.h"
#include "NetTpsGameMode.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/PlayerState.h>
#include "PlayerNameWidget.h"

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

	// ���� ���� ������Ʈ ������
	compGun = CreateDefaultSubobject<USceneComponent>(TEXT("GUN"));
	compGun->SetupAttachment(GetMesh(), FName(TEXT("GunPosition")));
	compGun->SetRelativeLocation(FVector(-7.144f, 3.68f, 4.136f));
	compGun->SetRelativeRotation(FRotator(3.4f, 75.699f, 6.6424f));

	// HP ���� (�� �Ӹ����� �ִ�)
	compHP = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP"));
	compHP->SetupAttachment(RootComponent);

	// �г��� (�� �Ӹ����� �ִ�)
	compName = CreateDefaultSubobject<UWidgetComponent>(TEXT("NICK_NAME"));
	compName->SetupAttachment(compHP);


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
	
	// AnimInstance ��������
	anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	// 1. �ٴڿ� ����ִ� Pistol �� ã��.
	TArray<AActor*> allActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), allActor);
	for (int32 i = 0; i < allActor.Num(); i++)
	{
		if (allActor[i]->GetActorNameOrLabel().Contains(TEXT("Pistol")))
		{
			allPistol.Add(allActor[i]);
		}
	}

	FString serverClient = HasAuthority() ? TEXT("[Server]") : TEXT("[Client]");
	FString hasController = Controller ? TEXT("Player") : TEXT("No Player");
	UE_LOG(LogTemp, Warning, TEXT("%s - %s"), *serverClient, *hasController);

	ReloadComplete();
	

	

	//// �Ѿ� �ʱ� ����
	//currBulletCnt = maxBulletCnt;

	//// �Ѿ� UI �ʱ� ����
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

	if (nickName.IsEmpty() == false)
	{
		Cast<UPlayerNameWidget>(compName->GetWidget())->SetNickName(nickName);
	}
}

void ANetTpsCharacter::PossessedBy(AController* NewController)
{
	UE_LOG(LogTemp, Warning, TEXT("PossessedBy Start"));
	Super::PossessedBy(NewController);
	UE_LOG(LogTemp, Warning, TEXT("PossessedBy End"));

	ClientRPC_CreateWidget();

	APlayerState* ps = GetPlayerState();
	if (ps)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Name : %s"), *ps->GetPlayerName());
		nickName = ps->GetPlayerName();
		//OnRep_NickName();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerState : null"));
	}
}

void ANetTpsCharacter::PostNetInit()
{
	Super::PostNetInit();

	if (anim == nullptr)
	{
		// AnimInstance ��������
		anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	}

	// ���࿡ ���� ��� �ִٸ�
	if (closestPistol)
	{
		// ���� �ٿ���
		AttachPistol(closestPistol);
	}
}

void ANetTpsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetTpsCharacter, currHP);
	DOREPLIFETIME(ANetTpsCharacter, closestPistol);
	DOREPLIFETIME(ANetTpsCharacter, nickName);

}

void ANetTpsCharacter::PrintNetLog()
{
	// Connection ����
	FString conStr = GetNetConnection() != nullptr ? TEXT("Valid Connect") : TEXT("InValid Connect");
	// ���� ���� Actor
	FString ownerStr = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");
	// Role
	// ROLE_Authority : ��� ������ �� ���� �ִ� (���� ����)
	// ROLE_AutonomousProxy : ���� (Input) �� �����ϴ�.
	// ROLE_SimulatedProxy : ���⸸ (�ùķ��̼Ǹ�) �����Ѵ�.
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
	// -ī�޶��� �չ���
	FVector foward = -cam->GetActorForwardVector();
	// ī�޶��� ������
	FVector up = cam->GetActorUpVector();
 
	FRotator rot = UKismetMathLibrary::MakeRotFromXZ(foward, up);

	compHP->SetWorldRotation(rot);
}

void ANetTpsCharacter::DieProcess()
{
	// ������ �ƴ϶�� �Լ� ������
	if(!IsLocallyControlled()) return;

	// ȭ�� ȸ�� ó��
	FollowCamera->PostProcessSettings.bOverride_ColorSaturation = true;
	FollowCamera->PostProcessSettings.ColorSaturation = FVector4(0, 0, 0, 1);

	// GameOver UI ������
	mainWidget->ShowGameOverUI(true);
}

void ANetTpsCharacter::ClientRPC_CreateWidget_Implementation()
{
	// ������ �ִ� mainWidget ������.

	// Main Widget ����
	mainWidget = Cast<UMainWidget>(CreateWidget(GetWorld(), mainWidgetFactory));
	mainWidget->AddToViewport();
	mainWidget->ShowPistolUI(false);
	mainWidget->ShowGameOverUI(false);

	compHP->SetVisibility(false);
		
	ReloadComplete();
}

void ANetTpsCharacter::OnRep_CurrHP()
{
	// ���� ĳ���Ͷ��
	if (IsLocallyControlled())
	{
		// mainwidget �� �ִ� healtBar ����
		mainWidget->myHealthBar->UpdateHealthBar(currHP, maxHP);
		// ������ UI ���̰� ����
		mainWidget->ShowDamageUI();
	}
	else
	{
		// �Ӹ����� �ִ� healtBar ����
		// compHP �� ���õǾ� �ִ� HealthBar �� ��������
		UHealthBar* healthbar = Cast<UHealthBar>(compHP->GetWidget());
		// ������ HealtBar �� �Լ� UpdateHealthBar ȣ��
		healthbar->UpdateHealthBar(currHP, maxHP);
	}
	

	// ���࿡ ���� HP �� 0�̸�
	if (currHP <= 0)
	{
		// ����ó��
		anim->isDeath = true;
		// �浹 �ȵǰ�
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Movement ������Ʈ ��Ȱ��
		GetCharacterMovement()->DisableMovement();
		// ���࿡ �ѵ�� �ִٸ� ������
		if (IsLocallyControlled() && closestPistol)
		{
			TakePistol();
		}
	}
}

void ANetTpsCharacter::DamageProcess()
{
	// ���� HP ������
	currHP -= 10;
	
	OnRep_CurrHP();
}

void ANetTpsCharacter::ShowCursor()
{
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
}

//void ANetTpsCharacter::OnRep_NickName()
//{
//	UPlayerNameWidget* widget = Cast<UPlayerNameWidget>(compName->GetWidget());
//	if (widget == nullptr)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("compName->GetWidget() is null"));
//	}
//	else
//	{
//		widget->SetNickName(nickName);
//	}
//}

void ANetTpsCharacter::MakeCube()
{
	// ����ó��
	anim->isDeath = true;
	// �浹 �ȵǰ�
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Movement ������Ʈ ��Ȱ��
	GetCharacterMovement()->DisableMovement();
	// ���࿡ �ѵ�� �ִٸ� ������
	if (IsLocallyControlled() && closestPistol)
	{
		TakePistol();
	}
	//ServerRPC_MakeCube(compGun->GetComponentLocation(), compGun->GetComponentRotation());
}


void ANetTpsCharacter::ServerRPC_MakeCube_Implementation(FVector pos, FRotator rot)
{
	// ������ ���� �ֵ��� ť�� �����
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

		// �� ���
		EnhancedInputComponent->BindAction(TakePistolAction, ETriggerEvent::Started, this, &ANetTpsCharacter::TakePistol);

		// �� ���
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ANetTpsCharacter::Fire);

		// ������
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ANetTpsCharacter::Reload);

		EnhancedInputComponent->BindAction(MakeCubeAction, ETriggerEvent::Started, this, &ANetTpsCharacter::MakeCube);

		// Ŀ�� Ȱ��ȭ
		EnhancedInputComponent->BindAction(ShowCursorAction, ETriggerEvent::Started, this, &ANetTpsCharacter::ShowCursor);

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
	// ���࿡ ������ ���̸� �Լ��� ������
	if (isReloading) return;
	// ���࿡ ���� ��� �ִٸ�
	if (closestPistol)
	{
		closestPistol->SetOwner(nullptr);
		MultiRPC_DetachPistol();
		return;
	}


	// ���� ��� ���� �ʴٸ�
	float closestDist = std::numeric_limits<float>::max();
	closestPistol = nullptr;

	for (int32 i = 0; i < allPistol.Num(); i++)
	{
		// ���࿡ ���� �����ڰ� ������ continue;
		if(allPistol[i]->GetOwner() != nullptr) continue;

		// 1. ��� Pistol ���� ������ �Ÿ��� ������.
		float dist = FVector::Distance(GetActorLocation(), allPistol[i]->GetActorLocation());

		// ���� ���� �� �ִ� ������ �ִ�?
		if (dist > takeGunDist) continue;

		// closestDist ���� dist �۴�?
		if (closestDist > dist)
		{
			// closestDist �� dist �� ����
			closestDist = dist;
			// closestPistol �� allPistol[i] �� ����
			closestPistol = allPistol[i];
			// ���� owner ����
			closestPistol->SetOwner(this);
		}
	}

	MultiRPC_AttachPistol(closestPistol);
	//AttachPistol();
}

void ANetTpsCharacter::AttachPistol(AActor* pistol)
{
	// ����� ���� ������ �Լ��� ������
	if (pistol == nullptr) return;
	closestPistol = pistol;

	// �������� ���� Off ��������
	auto compMesh = closestPistol->GetComponentByClass<UStaticMeshComponent>();
	compMesh->SetSimulatePhysics(false);

	// ���� ����� ����  Mesh -> GunPosition ���Ͽ� ������.
	closestPistol->AttachToComponent(compGun, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	// animInstance �� �ִ� hasPistol �� true
	anim->hasPistol = true;

	// bOrientRotaionToMovement ������
	GetCharacterMovement()->bOrientRotationToMovement = false;
	// RotationYaw ������
	bUseControllerRotationYaw = true;
	// SprintArm ��ġ �ٲ�����
	CameraBoom->TargetArmLength = 100;
	CameraBoom->SetRelativeLocation(FVector(-4.33f, 33.8f, 70));

	if (mainWidget)
	{
		//Pistol UI ���̰� ����
		mainWidget->ShowPistolUI(true);
	}
}

void ANetTpsCharacter::MultiRPC_AttachPistol_Implementation(AActor* pistol)
{
	AttachPistol(pistol);
}

void ANetTpsCharacter::DetachPistol()
{
	// �������� ���� On ��������
	auto compMesh = closestPistol->GetComponentByClass<UStaticMeshComponent>();
	compMesh->SetSimulatePhysics(true);
	// closestPistol �� compGun ������ ������
	closestPistol->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	// animInstance �� �ִ� hasPistol �� false
	anim->hasPistol = false;

	// bOrientRotaionToMovement ������
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// RotationYaw ������
	bUseControllerRotationYaw = false;
	// SprintArm ��ġ �ٲ�����
	CameraBoom->TargetArmLength = 400;
	CameraBoom->SetRelativeLocation(FVector::ZeroVector);

	if (mainWidget)
	{
		//Pistol UI ������ �ʰ� ����
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
	// ���� ��� ���� ������ �Լ��� ������
	// �Ѿ��� 0���� �Լ��� ������
	// ������ �߿��� �Լ��� ������
	if (closestPistol == nullptr || currBulletCnt <= 0 || isReloading) return;

	FHitResult hitInfo;
	FVector startPos = FollowCamera->GetComponentLocation();
	FVector endPos = startPos + FollowCamera->GetForwardVector() * 100000;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	bool isHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECollisionChannel::ECC_Visibility, params);
	
	if (isHit)
	{
		// ���࿡ ���� �ְ� �ٸ� Player ���
		ANetTpsCharacter* otherPlayer = Cast<ANetTpsCharacter>(hitInfo.GetActor());
		if (otherPlayer)
		{
			// ������ ����
			otherPlayer->DamageProcess();
		}
		// cube ���
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

	// �� ��� �ִϸ��̼� ����
	PlayAnimMontage(pistolMontage, 2.0f, FName(TEXT("Fire")));

	// �Ѿ� �ϳ� ���!
	currBulletCnt--;
	// �Ѿ� UI �ϳ� ����
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
	// ���� ��� ���� ������ �Լ��� ������
	// ������ ���̸� �Լ��� ������
	if (closestPistol == nullptr || isReloading) return;

	MultiRPC_Reload();
}

void ANetTpsCharacter::MultiRPC_Reload_Implementation()
{
	// ������ �ִϸ��̼� ����
	PlayAnimMontage(pistolMontage, 1.0f, FName(TEXT("Reload")));

	// ������ ��
	isReloading = true;
}

void ANetTpsCharacter::ReloadComplete()
{	
	// UI �� �߰��ؾ� �ϴ� �Ѿ� ����
	//int32 addBulletCnt = maxBulletCnt - currBulletCnt;

	if (mainWidget)
	{	
		for(int32 i = 0; i < currBulletCnt; i++)
			mainWidget->RemoveBullet();
		
		for (int32 i = 0; i < maxBulletCnt; i++)
			mainWidget->AddBullet();
	}

	currBulletCnt = maxBulletCnt;

	// ������ �Ϸ�
	isReloading = false;
}
