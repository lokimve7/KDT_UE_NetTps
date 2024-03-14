// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleCube.h"

// Sets default values
ASimpleCube::ASimpleCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 통신이 가능하게!
	bReplicates = true;
}

// Called when the game starts or when spawned
void ASimpleCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASimpleCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

