// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EWeapons.h"
#include "ERarity.h"
#include "Weapon.generated.h"

class UDamageTypeMelee : public UDamageType
{
};

UCLASS()
class CPPTHIRDPERSON_API AWeapon : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TWeakObjectPtr<class AActor> WielderPtr;

	bool DealsDamage;

	bool IsHeavyAttack;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* OnHitFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* OnHitAudio;

	/**
	 * Particle system used to create a trail when the weapon is swung.
	 * 
	 * <remarks>
	 * Implemented following tutorial seen here https://www.youtube.com/watch?v=SukG8k9OMu8
	 * </remarks>
	 **/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* TrailSystemFX;

	UPROPERTY()
	class UNiagaraComponent* TrailInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AWeapon();

	/** Mesh used to store the mesh that represents the weapon */
	UPROPERTY(Category = Weapon, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponMesh;

	/** Enum value that determines the type of weapon it is. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	EWeapons WeaponType = EWeapons::DA_Unarmed;

	/** Enum value that determines the rarity of the weapon. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	ERarity Rarity = ERarity::DA_Common;

	UPROPERTY(Category = Weapon, EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float Damage = 10;

	UPROPERTY(Category = Weapon, EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float DamageHeavyAttack = 25;

	UFUNCTION(Category = Character)
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Category = Character)
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Category = Character)
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Category = Weapon)
	void SetWielder(class AActor* Actor);

	UFUNCTION(Category = Weapon)
	void SetDealsDamage(bool bDealsDamage);

	UFUNCTION(Category = Weapon)
	void SetIsHeavyAttack(bool bIsHeavyAttack);
};
