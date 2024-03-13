// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NetTpsCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ANetTpsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* TakePistolAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

public:
	ANetTpsCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void TakePistol();
	UFUNCTION(Server, Reliable)
	void ServerRPC_TakePistol();

	void AttachPistol(AActor* pistol);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_AttachPistol(AActor* pistol);

	void DetachPistol();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_DetachPistol();

public:
	void Fire();
	UFUNCTION(Server, Reliable)
	void ServerRPC_Fire();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_Fire(bool isHit, FVector impactPoint);

	void Reload();
	UFUNCTION(Server, Reliable)
	void ServerRPC_Reload();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_Reload();
	void ReloadComplete();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void PrintNetLog();
	
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UPROPERTY()
	class UNetPlayerAnimInstance* anim;

	UPROPERTY(EditAnywhere)
	class USceneComponent* compGun;

	UPROPERTY(VisibleInstanceOnly)
	TArray<class AActor*> allPistol;

	UPROPERTY(EditAnywhere)
	float takeGunDist = 200;

	UPROPERTY()
	AActor* closestPistol;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* pistolEffect;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* pistolMontage;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMainWidget> mainWidgetFactory;
	UPROPERTY()
	class UMainWidget* mainWidget;

	int32 maxBulletCnt = 10;
	int32 currBulletCnt = 0;
	bool isReloading = false;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* compHP;
	void BillboardHP();

	float maxHP = 100;

	UPROPERTY(ReplicatedUsing = OnRep_CurrHP)
	float currHP = maxHP;
	UFUNCTION()
	void OnRep_CurrHP();

	void DamageProcess();



	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> cubeFactory;
	UFUNCTION(Server, Reliable)
	void ServerRPC_MakeCube();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_MakeCube();
};


