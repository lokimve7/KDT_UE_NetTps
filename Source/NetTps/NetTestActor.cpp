// Fill out your copyright notice in the Description page of Project Settings.


#include "NetTestActor.h"

// Sets default values
ANetTestActor::ANetTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TEXT"));
	SetRootComponent(compMesh);
}

// Called when the game starts or when spawned
void ANetTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANetTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

