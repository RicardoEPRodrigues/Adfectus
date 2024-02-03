// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Utils/Interactor.h"
#include "CPPThirdPersonCharacter.generated.h"

#define RIGHT_WEAPON_SOCKET "Right_Weapon_Socket"
#define LEFT_WEAPON_SOCKET "Left_Weapon_Socket"
#define LEFT_SHIELD_SOCKET "Left_Shield_Socket"
#define HEAD_SOCKET "Head_Socket"

UCLASS(config = Game)
class ACPPThirdPersonCharacter : public ACharacter, public IInteractor
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraShakeSourceComponent* CameraShake;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* HeadMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat|FX", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* AttackEffortAudio;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat|FX", meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* OnHitFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat|FX", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* OnHitAudio;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat|FX", meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* OnBlockFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat|FX", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* OnBlockAudio;

	/* Between Player Collision */

	UPROPERTY(EditAnywhere, Category=Character)
	class USphereComponent* PlayerToPlayerCollisionComponent;

	/* Weapon Mechanics */

	/// Attaches equipped weapon to socket and sets this actor has its wielder.
	void AttachWeapon();

	/// Attaches equipped offhand weapon to socket and sets this actor has its wielder.
	void AttachOffhandWeapon();

	/// Attaches equipped shield to socket and sets this actor has its wielder.
	void AttachShield();

	/* Attack/Block Mechanics */
	/** Similar to IsAttack, but stores direct input to be processed */
	bool IsAttacking_Input;
	bool IsHeavyAttacking_Input;
	bool IsBlocking_Input;
	bool IsSprinting_Input;

protected:
	virtual void BeginPlay() override;

	/**
	 * Timer to call SetupInput().
	 */
	FTimerHandle SetupInputTimer;

	UFUNCTION(BlueprintCallable)
	void SetupInput();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	class UInputAction* ActionMove;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	class UInputAction* ActionLook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	class UInputAction* ActionAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	class UInputAction* ActionHeavyAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	class UInputAction* ActionBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	class UInputAction* ActionInteract;

	void Move(const struct FInputActionValue& Value);

	void Look(const struct FInputActionValue& Value);

	void Interact(const struct FInputActionValue& Value);

	void FaceForward();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Weapon Mechanics */

	UPROPERTY(Category = "Combat|Weapon", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AWeapon* EquippedWeapon;

	UPROPERTY(Category = "Combat|Weapon", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AWeapon* EquippedOffhandWeapon;

	UPROPERTY(Category = "Combat|Weapon", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AShield* EquippedShield;

	/* Sprint Mechanics */
	float MaxWalkSpeed;
	float WalkSpeedTemp;

	UFUNCTION(BlueprintCallable, Category = "Character|Sprinting")
	virtual void Sprint_Input(const struct FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Character|Sprinting")
	virtual void StopSprint_Input(const struct FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Character|Sprinting")
	virtual void Sprint();

	UFUNCTION(BlueprintCallable, Category = "Character|Sprinting")
	virtual void StopSprint();

	/* Attack/Block Mechanics */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartAttack_Input(const struct FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StopAttack_Input(const struct FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartHeavyAttack_Input(const struct FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StopHeavyAttack_Input(const struct FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartBlock_Input(const struct FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StopBlock_Input(const struct FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartHeavyAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StopAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StartBlock();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void StopBlock();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void OnBeginAttackUninterruptableAnticipation();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void OnBeginAttackFollowthrough();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool CanInterrupt;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interact)
	TArray<class AInteractable*> Interactables;

	float InteractablesHighlightTimerDuration = .1f;
	FTimerHandle InteractablesHighlightTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	class ACPPThirdPersonCharacter* LastHitEnemy;

	/**
	 * @brief If true the character is immune to damage.
	 */
	UPROPERTY(Category = "Combat|Health", EditAnywhere, BlueprintReadWrite)
	bool IsImmune = false;

	UPROPERTY(Category = "Combat|Health", EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* DamageVignetteMaterial;

	UPROPERTY()
	class UMaterialInstanceDynamic* DamageVignetteMaterialInstance;

	// EMOTION EXPRESSION
	/**
	 * This colors the character mesh to match the color of the felt emotion.
	 */
	UPROPERTY(Category = "Emotion", EditAnywhere, BlueprintReadWrite)
	class UEmotionColoredOutline* EmotionColoredOutlineComponent;

	/**
	 * Component to be added to the face of the character to express emotions through emojis
	 */
	UPROPERTY(Category= "Emotion", EditAnywhere, BlueprintReadWrite)
	class UEmojiFacialExpressionWidgetComponent* EmojiWidgetComponent;

	/**
	 * Component to be added to the character to express emotions through backgrounds that show up on top of the character.
	 */
	UPROPERTY(Category= "Emotion", EditAnywhere, BlueprintReadWrite)
	class UBackgroundEmotionExpressionComponent* BackgroundComponent;

public:
	UPROPERTY(Category = Character, EditAnywhere, BlueprintReadWrite)
	FString CharacterName = "Bones";

	ACPPThirdPersonCharacter();

	virtual void BeginDestroy() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	/** Returns CameraBoom sub-object **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera sub-object **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/* Health Mechanics */

	UPROPERTY(Category = "Combat|Health", EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0", UIMin = "0"))
	float Health;

	UPROPERTY(Category = "Combat|Health", EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0", UIMin = "0"))
	float MaxHealth;

	UFUNCTION(Category="Combat|Health", BlueprintPure)
	float GetNormalizedHealth() const;

	/**
	 * @brief Period of time in seconds in which the character remains immune to damage, after taking damage.
	 * 0 means that character is never immune.
	 */
	UPROPERTY(Category = "Combat|Health", EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0", UIMin = "0"))
	float ImmunePeriodAfterDamage = 1.0f;

	UPROPERTY()
	FTimerHandle ImmuneTimerHandle;

	/** Check if the character is alive. Equivalent to checking if Health > 0. */
	UFUNCTION(BlueprintPure, Category = "Combat|Health")
	virtual bool IsAlive();

	virtual float TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent,
	                         class AController* EventInstigator, class AActor* DamageCauser) override;

	/** Check if the character is able to move. The character can't move if it is dead, attacking, or blocking. */
	UFUNCTION(BlueprintCallable, Category = Character)
	virtual bool CanMove();

	/** Kills the Character. Sets Health to 0 and disables movement. 
	 * It is called when the character takes damage and its health gets to 0. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Health")
	virtual void Die();

	/** Brings Character back to life! Sets Health to MaxHealth and re-enables movement. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Health")
	virtual void Revive();

	/** Resets the variables defined in this character, but does not call the Reset() function. */
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void SoftReset();

	virtual void Reset() override;

	/* Sprint Mechanics */

	UPROPERTY(Category = "Character|Sprinting", BlueprintReadOnly, VisibleAnywhere)
	bool IsSprinting;

	UPROPERTY(Category = "Character|Sprinting", EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0", UIMin = "0"))
	float MaxSprintSpeed;

	/* Attack/Block Mechanics */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool IsAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool IsHeavyAttacking;

	/**
	 * @brief True if the character is currently performing the block action.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool IsBlocking;

	/** If true, when blocking, the player will parry instead. 
	 * Blocking is held until the player releases the button, while parry is held for just some milliseconds.
	 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsDefaultParry;

	/** 
	 * @brief If true, the character can perform a new action (attack or block). False otherwise.
	 * Used along with AnimationTimeout to forbid new actions before the timeout is over.
	 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool CanPerform = true;

	/**
	 * @brief Defines how long a character should wait until it can block again. Value in seconds.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AnimationTimeout = 0.2f;

	UPROPERTY()
	FTimerHandle AnimationTimerHandle;

	/** Equips given weapon on the character. If a weapon is already equipped, the equipped weapon will be dropped. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	virtual void EquipWeapon(class AWeapon* Weapon);

	/** Equips a copy of the given weapon on the character. If a weapon is already equipped, the equipped weapon will be destroyed. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	virtual void EquipWeaponCopy(class AWeapon* Weapon);

	/** Drops Equipped Weapon if it exists. If destroy parameter is true, the weapon is destroyed instead of being dropped. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	virtual void DropWeapon(bool bDestroy = false);

	/** Equips given offhand weapon on the character. If a weapon is already equipped, the equipped weapon will be dropped. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	virtual void EquipOffhandWeapon(class AWeapon* Weapon);

	/** Equips a copy of the given offhand weapon on the character. If a weapon is already equipped, the equipped weapon will be destroyed. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	virtual void EquipOffhandWeaponCopy(class AWeapon* Weapon);

	/** Drops equipped offhand Weapon if it exists. If destroy parameter is true, the weapon is destroyed instead of being dropped. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	virtual void DropOffhandWeapon(bool bDestroy = false);

	/** Equips given shield on the character. If a shield is already equipped, the equipped shield will be dropped. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	virtual void EquipShield(class AShield* Shield);

	/** Equips a copy of the given shield on the character. If a shield is already equipped, the equipped shield will be destroyed. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	virtual void EquipShieldCopy(class AShield* Shield);

	/** Drops Equipped Shield if it exists. If destroy parameter is true, the shield is destroyed instead of being dropped. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	virtual void DropShield(bool bDestroy = false);

	/* Jump Mechanics */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	bool IsJumping;

	virtual void Jump() override;

	virtual void StopJumping() override;

	virtual void IsAvailable_Implementation(class AInteractable* Interactable) override;

	virtual void IsUnavailable_Implementation(class AInteractable* Interactable) override;

	UFUNCTION(BlueprintCallable, Category = Interaction)
	void InteractWithNearest();

	UFUNCTION(BlueprintCallable, Category = Interaction)
	void HighlightNearestInteractable();

	/**
	 * Checks if the Character has Interactables close enough to interact with.
	 * 
	 * @note This is just a check if the Interactables internal array is not empty.
	 * @return True if there are Interactables nearby, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category = Interaction)
	bool HasInteractables() const;

	/**
	 * Return a reference to the last enemy character to be hit by this character.
	 * 
	 * @return Reference to the last enemy character to be hit.
	 */
	UFUNCTION(BlueprintPure, Category = Enemy)
	class ACPPThirdPersonCharacter* GetLastHitEnemy() const;
	UFUNCTION(BlueprintCallable, Category = Enemy)
	void ResetLastHitEnemy();
	UFUNCTION(BlueprintCallable, Category = Enemy)
	void UpdateLastHitEnemy(class ACPPThirdPersonCharacter* Actor);
	void UpdateLastHitEnemy(class AActor* Actor);

	UPROPERTY(Category = "3Motion", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UThreeMotionAgentComponent* AgentComp;


	// Implementation of action stage changes events

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionStageChangeDelegate);

	UPROPERTY(BlueprintAssignable, Category="Character")
	FActionStageChangeDelegate OnInterruptibleAnticipationEvent;

	UPROPERTY(BlueprintAssignable, Category="Character")
	FActionStageChangeDelegate OnUninterruptibleAnticipationEvent;

	UPROPERTY(BlueprintAssignable, Category="Character")
	FActionStageChangeDelegate OnUninterruptibleFollowthroughEvent;

	UPROPERTY(BlueprintAssignable, Category="Character")
	FActionStageChangeDelegate OnInterruptibleFollowthroughEvent;

	UPROPERTY(BlueprintAssignable, Category="Character")
	FActionStageChangeDelegate OnInterruptedEvent;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterChangeDelegate, class ACPPThirdPersonCharacter*, Character);

	UPROPERTY(BlueprintAssignable, Category="Character")
	FCharacterChangeDelegate OnDeathEvent;

	UPROPERTY(BlueprintAssignable, Category="Character")
	FCharacterChangeDelegate OnReviveEvent;

	UPROPERTY(BlueprintAssignable, Category="Character")
	FCharacterChangeDelegate OnHealthChangedEvent;


	// Implementation of combat related events

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FCombatEventDelegate, class AActor*, Attacker, class AActor*, Target,
	                                             float, TargetMaxHealth, float, TargetPreviousHealth, float,
	                                             TargetHealth, float, HealthDifference);

	UPROPERTY(BlueprintAssignable, Category="Combat|Events")
	FCombatEventDelegate CombatEvent_OnHit;
	UPROPERTY(BlueprintAssignable, Category="Combat|Events")
	FCombatEventDelegate CombatEvent_OnBlock;
};
