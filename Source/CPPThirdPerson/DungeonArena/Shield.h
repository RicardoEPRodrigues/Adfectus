// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ERarity.h"
#include "GameFramework/Actor.h"
#include "Shield.generated.h"

UCLASS()
class CPPTHIRDPERSON_API AShield : public AActor
{
	GENERATED_BODY()

	class AActor* Wielder;

public:
	// Sets default values for this actor's properties
	AShield();

	/** Mesh used to store the mesh that represents the Shield */
	UPROPERTY(Category = Shield, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShieldMesh;

	/** Enum value that determines the rarity of the shield. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shield)
	ERarity Rarity = ERarity::DA_Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shield, meta = (ClampMin = "0", UIMin = "0"))
	float DamageAbsorption;

	UFUNCTION(Category = Character)
	void SetWielder(class AActor* wielder);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
