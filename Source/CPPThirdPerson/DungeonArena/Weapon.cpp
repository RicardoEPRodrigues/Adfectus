// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "../CPPThirdPersonCharacter.h"
#include "Components/AudioComponent.h"
#include "CPPThirdPerson/Utils/AdfectusUtilsLibrary.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	WeaponMesh->SetCollisionProfileName(FName("Weapon"));
	WeaponMesh->SetNotifyRigidBodyCollision(true);
	SetActorEnableCollision(true);
	RootComponent = WeaponMesh;

	WeaponType = EWeapons::DA_Unarmed;

	Damage = 15.0f;

	SetCanBeDamaged(false);
	DealsDamage = false;

	OnHitFX = LoadObject<UNiagaraSystem>(
		nullptr, TEXT("NiagaraSystem'/Game/Adfectus/VFX/FX_OnHitOthersSystem.FX_OnHitOthersSystem'"), nullptr,
		LOAD_None,
		nullptr);

	TrailSystemFX = LoadObject<UNiagaraSystem>(
		nullptr, TEXT("NiagaraSystem'/Game/Adfectus/VFX/FX_WeaponTrailSystem.FX_WeaponTrailSystem'"), nullptr,
		LOAD_None,
		nullptr);

	// Audio

	OnHitAudio  = CreateDefaultSubobject<UAudioComponent>(TEXT("On Block SFX"));
	OnHitAudio->SetupAttachment(RootComponent);
	OnHitAudio->bAutoActivate = false;
	OnHitAudio->VolumeMultiplier = .5f;
	static ConstructorHelpers::FObjectFinder<USoundCue> OnBlockAudioBPObject(TEXT("SoundCue'/Game/Adfectus/SFX/SC_OnBlock.SC_OnBlock'"));
	if (OnBlockAudioBPObject.Object != nullptr)
	{
		OnHitAudio->SetSound(OnBlockAudioBPObject.Object);
	}
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	FScriptDelegate onHitDelegate;
	onHitDelegate.BindUFunction(this, "OnHit");
	OnActorHit.Add(onHitDelegate);

	FScriptDelegate onBODelegate;
	onBODelegate.BindUFunction(this, "OnBeginOverlap");
	OnActorBeginOverlap.Add(onBODelegate);

	FScriptDelegate onEODelegate;
	onEODelegate.BindUFunction(this, "OnEndOverlap");
	OnActorEndOverlap.Add(onEODelegate);
}

void AWeapon::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	// UE_LOG(LogAdfectus, Warning, TEXT("%s hit %s"), *GetNameSafe(SelfActor), *GetNameSafe(OtherActor));

	if (!DealsDamage)
	{
		return;
	}
	AActor* Wielder = WielderPtr.Get();
	if (!Wielder || OtherActor == Wielder)
	{
		return;
	}
	float FinalDamage = this->IsHeavyAttack ? this->DamageHeavyAttack : this->Damage;
	FPointDamageEvent damageEvent;
	damageEvent.DamageTypeClass = UDamageTypeMelee::StaticClass();
	damageEvent.HitInfo = Hit;
	damageEvent.ShotDirection = Hit.ImpactPoint - this->GetActorLocation();
	damageEvent.Damage = FinalDamage;

	if (OtherActor)
	{
		OtherActor->TakeDamage(FinalDamage, damageEvent, nullptr, Wielder);
	}

	if (OnHitFX)
	{
		const FRotator Rotator = UAdfectusUtilsLibrary::MakeRotatorFromDirections(FVector::UpVector, Hit.ImpactNormal);
		// UNiagaraComponent* NiagaraComp = 
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), OnHitFX, Hit.Location, Rotator);
	}
	if (OnHitAudio && !OnHitAudio->IsPlaying())
	{
		OnHitAudio->Play();
	}
}

void AWeapon::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                             const FHitResult& SweepResult)
{
	// UE_LOG(LogAdfectus, Warning, TEXT("%s began overlap with %s."), *GetNameSafe(this), *GetNameSafe(OtherActor));
	if (!DealsDamage)
	{
		return;
	}
	AActor* Wielder = WielderPtr.Get();
	if (!Wielder || OtherActor == Wielder)
	{
		return;
	}

	float FinalDamage = this->IsHeavyAttack ? this->DamageHeavyAttack : this->Damage;
	FHitResult HitResult;
	if (UAdfectusUtilsLibrary::DetectCollision(this, OtherActor, HitResult, .5f))
	{
		FPointDamageEvent damageEvent;
		damageEvent.DamageTypeClass = UDamageTypeMelee::StaticClass();
		damageEvent.HitInfo = HitResult;
		// Calculation could also be this: HitResult.ImpactPoint - HitResult.TraceStart;
		damageEvent.ShotDirection = HitResult.ImpactPoint - this->GetActorLocation();
		damageEvent.Damage = FinalDamage;

		// DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.ImpactPoint, FColor::Green, false, 5.0f);
		// DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceStart + (HitResult.ImpactNormal * 50),
		//               FColor::Red, false, 5.0f);

		OtherActor->TakeDamage(FinalDamage, damageEvent, nullptr, Wielder);
	}
	else
	{
		FDamageEvent DamageEvent;
		DamageEvent.DamageTypeClass = UDamageTypeMelee::StaticClass();
		OtherActor->TakeDamage(FinalDamage, DamageEvent, nullptr, Wielder);
	}
	// The call below would remove damage dealing after the first collision.
	// This should no longer be necessary, as the character now becomes immune.
	// SetDealsDamage(false);

	if (ACPPThirdPersonCharacter* Character = Cast<ACPPThirdPersonCharacter>(Wielder))
	{
		Character->UpdateLastHitEnemy(OtherActor);
	}
}

void AWeapon::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// UE_LOG(LogAdfectus, Warning, TEXT("%s ended overlap with %s"), *GetNameSafe(this), *GetNameSafe(OtherActor));
}

void AWeapon::SetWielder(AActor* Actor)
{
	if (AActor* Wielder = WielderPtr.Get())
	{
		WeaponMesh->MoveIgnoreActors.Remove(Wielder);
	}
	WielderPtr = Actor;
	if (Actor)
	{
		WeaponMesh->MoveIgnoreActors.Add(Actor);
	}
}

void AWeapon::SetDealsDamage(const bool bDealsDamage)
{
	const bool bValueChanged = DealsDamage != bDealsDamage;


	if (bValueChanged)
	{
		DealsDamage = bDealsDamage;

		if (DealsDamage)
		{
			if (TrailSystemFX)
			{
				// FName("trail_start")
				TrailInstance = UNiagaraFunctionLibrary::SpawnSystemAttached(
					TrailSystemFX, WeaponMesh, FName("trail_end"),
					FVector::ZeroVector, FRotator::ZeroRotator,
					EAttachLocation::KeepRelativeOffset, true);
			}
		}
		else
		{
			if (TrailInstance)
			{
				TrailInstance->Deactivate();
			}
		}
	}
}

void AWeapon::SetIsHeavyAttack(bool bIsHeavyAttack)
{
	IsHeavyAttack = bIsHeavyAttack;
}
