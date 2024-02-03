// Fill out your copyright notice in the Description page of Project Settings.


#include "Shield.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
AShield::AShield()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield Mesh"));
	ShieldMesh->SetCollisionProfileName(FName("OverlapAll"));

	Wielder = nullptr;

	DamageAbsorption = 10.0f;

	SetCanBeDamaged(false);
}

// Called when the game starts or when spawned
void AShield::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShield::SetWielder(AActor* wielder)
{
	Wielder = wielder;
}
