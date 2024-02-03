// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CPPThirdPersonCharacter.h"
#include "EngineClasses.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DungeonArena/Weapon.h"
#include "DungeonArena/Shield.h"
#include "Utils/Interactable.h"
#include "AdfectusLogging.h"
#include "3Motion/ThreeMotionAgentComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Adfectus/BackgroundEmotionExpressionComponent.h"
#include "Adfectus/EmojiFacialExpressionWidgetComponent.h"
#include "Adfectus/EmotionColoredOutline.h"
#include "Utils/AdfectusUtilsLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"

//////////////////////////////////////////////////////////////////////////
// ACPPThirdPersonCharacter

ACPPThirdPersonCharacter::ACPPThirdPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(28.0f, 96.0f);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
	GetCapsuleComponent()->SetCollisionProfileName(FName("Pawn"));

	PlayerToPlayerCollisionComponent = CreateDefaultSubobject<USphereComponent>(
		TEXT("Player To Player Collision Component"));
	PlayerToPlayerCollisionComponent->InitSphereRadius(120.0f);
	PlayerToPlayerCollisionComponent->SetCollisionProfileName(FName("PlayerToPlayer"));
	PlayerToPlayerCollisionComponent->CanCharacterStepUpOn = ECB_No;
	PlayerToPlayerCollisionComponent->SetShouldUpdatePhysicsVolume(false);
	PlayerToPlayerCollisionComponent->SetCanEverAffectNavigation(false);
	PlayerToPlayerCollisionComponent->SetupAttachment(RootComponent);


	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	InputMapping = LoadObject<UInputMappingContext>(
		nullptr, TEXT(
			"/Script/EnhancedInput.InputMappingContext'/Game/Adfectus/Input/Combat_MappingContext.Combat_MappingContext'"),
		nullptr, LOAD_None, nullptr);

	ActionMove = LoadObject<UInputAction>(
		nullptr, TEXT(
			"/Script/EnhancedInput.InputAction'/Game/Adfectus/Input/IA_Move.IA_Move'"),
		nullptr, LOAD_None, nullptr);
	ActionLook = LoadObject<UInputAction>(
		nullptr, TEXT(
			"/Script/EnhancedInput.InputAction'/Game/Adfectus/Input/IA_Look.IA_Look'"),
		nullptr, LOAD_None, nullptr);
	ActionAttack = LoadObject<UInputAction>(
		nullptr, TEXT(
			"/Script/EnhancedInput.InputAction'/Game/Adfectus/Input/IA_Attack.IA_Attack'"),
		nullptr, LOAD_None, nullptr);
	ActionHeavyAttack = LoadObject<UInputAction>(
		nullptr, TEXT(
			"/Script/EnhancedInput.InputAction'/Game/Adfectus/Input/IA_HeavyAttack.IA_HeavyAttack'"),
		nullptr, LOAD_None, nullptr);
	ActionBlock = LoadObject<UInputAction>(
		nullptr, TEXT(
			"/Script/EnhancedInput.InputAction'/Game/Adfectus/Input/IA_Block.IA_Block'"),
		nullptr, LOAD_None, nullptr);
	ActionInteract = LoadObject<UInputAction>(
		nullptr, TEXT(
			"/Script/EnhancedInput.InputAction'/Game/Adfectus/Input/IA_Interact.IA_Interact'"),
		nullptr, LOAD_None, nullptr);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 3.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->SetWalkableFloorAngle(45.0f);
	MaxSprintSpeed = 900.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 250.0f; // The camera follows at this distance behind the character
	CameraBoom->SocketOffset.Set(0.0f, -100.0f, 50.0f);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 20;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 20;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,
	                              USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	CameraShake = CreateDefaultSubobject<UCameraShakeSourceComponent>(TEXT("CameraShake"));
	CameraShake->SetupAttachment(FollowCamera);
	static ConstructorHelpers::FClassFinder<UCameraShakeBase> CameraShakeBPClass(
		TEXT("Blueprint'/Game/Adfectus/Blueprints/Characters/BP_CameraShake_OnHit.BP_CameraShake_OnHit_C'"));
	if (CameraShakeBPClass.Class != nullptr)
	{
		CameraShake->CameraShake = CameraShakeBPClass.Class;
	}

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// Create the Head Mesh
	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Head Mesh"));
	HeadMesh->SetupAttachment(GetMesh(), TEXT(HEAD_SOCKET));
	HeadMesh->SetCollisionProfileName(FName("NoCollision"));

	// Weapons and Shield Initialization
	EquippedWeapon = nullptr;
	EquippedOffhandWeapon = nullptr;
	EquippedShield = nullptr;

	MaxHealth = 100.0f;
	Health = MaxHealth;

	IsDefaultParry = true;

	CanInterrupt = true;

	// Agent Component
	AgentComp = CreateDefaultSubobject<UThreeMotionAgentComponent>(TEXT("AgentComponent"));

	// FX
	OnHitFX = LoadObject<UNiagaraSystem>(
		nullptr, TEXT("NiagaraSystem'/Game/Adfectus/VFX/FX_OnHitSystem.FX_OnHitSystem'"), nullptr, LOAD_None, nullptr);
	OnBlockFX = LoadObject<UNiagaraSystem>(
		nullptr, TEXT("NiagaraSystem'/Game/Adfectus/VFX/FX_OnBlockSystem.FX_OnBlockSystem'"), nullptr, LOAD_None,
		nullptr);

	// Audio

	AttackEffortAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("Attack Effort SFX"));
	AttackEffortAudio->SetupAttachment(RootComponent);
	AttackEffortAudio->bAutoActivate = false;
	AttackEffortAudio->VolumeMultiplier = .3f;
	static ConstructorHelpers::FObjectFinder<USoundCue> AttackEffortAudioBPObject(
		TEXT("SoundCue'/Game/Adfectus/SFX/SC_Effort.SC_Effort'"));
	if (AttackEffortAudioBPObject.Object != nullptr)
	{
		AttackEffortAudio->SetSound(AttackEffortAudioBPObject.Object);
	}

	OnHitAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("On Hit SFX"));
	OnHitAudio->SetupAttachment(RootComponent);
	OnHitAudio->bAutoActivate = false;
	OnHitAudio->VolumeMultiplier = .5f;
	static ConstructorHelpers::FObjectFinder<USoundCue> OnHitAudioBPObject(
		TEXT("SoundCue'/Game/Adfectus/SFX/SC_OnHit.SC_OnHit'"));
	if (OnHitAudioBPObject.Object != nullptr)
	{
		OnHitAudio->SetSound(OnHitAudioBPObject.Object);
	}
	OnBlockAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("On Block SFX"));
	OnBlockAudio->SetupAttachment(RootComponent);
	OnBlockAudio->bAutoActivate = false;
	OnBlockAudio->VolumeMultiplier = .5f;
	static ConstructorHelpers::FObjectFinder<USoundCue> OnBlockAudioBPObject(
		TEXT("SoundCue'/Game/Adfectus/SFX/SC_OnBlock.SC_OnBlock'"));
	if (OnBlockAudioBPObject.Object != nullptr)
	{
		OnBlockAudio->SetSound(OnBlockAudioBPObject.Object);
	}

	DamageVignetteMaterial = LoadObject<UMaterialInterface>(
		nullptr, TEXT("Material'/Game/Adfectus/Materials/M_DamageVignette.M_DamageVignette'"), nullptr, LOAD_None,
		nullptr);


	// EMOTION EXPRESSION
	EmotionColoredOutlineComponent = CreateDefaultSubobject<UEmotionColoredOutline>(TEXT("Emotion Colored Outline"));
	EmotionColoredOutlineComponent->OutlinedMeshes.Add(GetMesh());
	EmotionColoredOutlineComponent->OutlinedMeshes.Add(HeadMesh);

	EmojiWidgetComponent = CreateDefaultSubobject<UEmojiFacialExpressionWidgetComponent>(
		TEXT("Emoji Expression Widget"));
	EmojiWidgetComponent->SetupAttachment(GetMesh(), TEXT(HEAD_SOCKET));
	EmojiWidgetComponent->SetRelativeLocation({0, 26.5, 24});
	EmojiWidgetComponent->SetRelativeRotation(FQuat::MakeFromEuler({0, -5, 90}));
	EmojiWidgetComponent->SetRelativeScale3D({0.1f, 0.1f, 0.1f});

	BackgroundComponent = CreateDefaultSubobject<UBackgroundEmotionExpressionComponent>(
		TEXT("Background Emotion Outline"));
	BackgroundComponent->SetupAttachment(RootComponent);
	BackgroundComponent->SetRelativeLocation({0, 0, 50});
}

void ACPPThirdPersonCharacter::BeginDestroy()
{
	// Drop and destroy weapon.
	DropWeapon(true);
	DropShield(true);
	DropOffhandWeapon(true);

	// Super Destroy should be the last call.
	Super::BeginDestroy();
}

void ACPPThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	APawn::AddControllerPitchInput(10);

	MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	Health = MaxHealth;

	GetWorldTimerManager().SetTimer(InteractablesHighlightTimerHandle, this,
	                                &ACPPThirdPersonCharacter::HighlightNearestInteractable,
	                                InteractablesHighlightTimerDuration, true);


	// Damage Vignette
	if (DamageVignetteMaterial)
	{
		DamageVignetteMaterialInstance = UMaterialInstanceDynamic::Create(DamageVignetteMaterial, this);
		if (DamageVignetteMaterialInstance)
		{
			const float VignetteFactor = (GetNormalizedHealth() - 1.0f) * 0.5f;
			DamageVignetteMaterialInstance->SetScalarParameterValue("VignetteFactor", VignetteFactor);

			const FWeightedBlendable Blend{1, DamageVignetteMaterialInstance};
			FollowCamera->PostProcessSettings.WeightedBlendables.Array.Add(Blend);
		}
	}
}

void ACPPThirdPersonCharacter::SetupInput()
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController());
		InputMapping && PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
}

void ACPPThirdPersonCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetupInput();
}

void ACPPThirdPersonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// HighlightNearestInteractable();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACPPThirdPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings

	// PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACPPThirdPersonCharacter::Jump);
	// PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACPPThirdPersonCharacter::StopJumping);

	//PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACPPThirdPersonCharacter::Sprint_Input);
	//PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACPPThirdPersonCharacter::StopSprint_Input);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ActionMove, ETriggerEvent::Triggered, this, &ACPPThirdPersonCharacter::Move);
		EnhancedInputComponent->BindAction(ActionLook, ETriggerEvent::Triggered, this, &ACPPThirdPersonCharacter::Look);
		EnhancedInputComponent->BindAction(ActionInteract, ETriggerEvent::Started, this,
		                                   &ACPPThirdPersonCharacter::Interact);

		EnhancedInputComponent->BindAction(ActionAttack, ETriggerEvent::Started, this,
		                                   &ACPPThirdPersonCharacter::StartAttack_Input);
		EnhancedInputComponent->BindAction(ActionAttack, ETriggerEvent::Completed, this,
		                                   &ACPPThirdPersonCharacter::StopAttack_Input);

		EnhancedInputComponent->BindAction(ActionHeavyAttack, ETriggerEvent::Started, this,
		                                   &ACPPThirdPersonCharacter::StartHeavyAttack_Input);
		EnhancedInputComponent->BindAction(ActionHeavyAttack, ETriggerEvent::Completed, this,
		                                   &ACPPThirdPersonCharacter::StopHeavyAttack_Input);

		EnhancedInputComponent->BindAction(ActionBlock, ETriggerEvent::Started, this,
		                                   &ACPPThirdPersonCharacter::StartBlock_Input);
		EnhancedInputComponent->BindAction(ActionBlock, ETriggerEvent::Completed, this,
		                                   &ACPPThirdPersonCharacter::StopBlock_Input);
	}
}

/////////////////////////////////////////////////////////////////

void ACPPThirdPersonCharacter::Look(const FInputActionValue& Value)
{
	if (!IsAlive())
	{
		return;
	}

	const FVector2D Input = Value.Get<FVector2D>();

	APawn::AddControllerYawInput(Input.X);

	APawn::AddControllerPitchInput(Input.Y);
}

void ACPPThirdPersonCharacter::Move(const FInputActionValue& Value)
{
	if (!CanMove())
	{
		return;
	}

	const FVector2D Input = Value.Get<FVector2D>();

	if (Input.Y != 0.0f)
	{
		const FVector CameraForward = FollowCamera->GetForwardVector();
		const FVector Direction(CameraForward.X, CameraForward.Y, 0);

		// // find out which way is forward
		// const FRotator Rotation = Controller->GetControlRotation();
		// const FRotator YawRotation(0, Rotation.Yaw, 0);

		// // get forward vector
		// const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Input.Y);
	}

	if (Input.X != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Input.X);
	}
}

void ACPPThirdPersonCharacter::Interact(const FInputActionValue& Value)
{
	InteractWithNearest();
}

void ACPPThirdPersonCharacter::Jump()
{
	// if (!CanMove()) return;
	// Super::Jump();
	// IsJumping = true;
	UE_LOG(LogAdfectus, Warning, TEXT("Player Jump is disabled."))
}

void ACPPThirdPersonCharacter::StopJumping()
{
	// Super::StopJumping();
	// IsJumping = false;
}

void ACPPThirdPersonCharacter::Sprint_Input(const struct FInputActionValue& Value)
{
	if (!IsAlive())
	{
		return;
	}

	IsSprinting_Input = true;
	Sprint();
}

void ACPPThirdPersonCharacter::StopSprint_Input(const struct FInputActionValue& Value)
{
	if (!IsAlive())
	{
		return;
	}

	IsSprinting_Input = false;
	StopSprint();
}

void ACPPThirdPersonCharacter::Sprint()
{
	if (IsBlocking || IsAttacking || IsHeavyAttacking)
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = MaxSprintSpeed;
	IsSprinting = true;
}

void ACPPThirdPersonCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	IsSprinting = false;
}

void ACPPThirdPersonCharacter::FaceForward()
{
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FRotator Rot = UKismetMathLibrary::MakeRotFromX(Direction);
		SetActorRotation(Rot);
	}
}

/////////////////////////////////////////////////////////////////

void ACPPThirdPersonCharacter::StartAttack_Input(const struct FInputActionValue& Value)
{
	if (!IsAlive())
	{
		return;
	}

	IsAttacking_Input = true;
	StartAttack();
}

void ACPPThirdPersonCharacter::StopAttack_Input(const struct FInputActionValue& Value)
{
	if (!IsAlive())
	{
		return;
	}

	IsAttacking_Input = false;
	if (CanInterrupt)
	{
		StopAttack();
		UE_LOG(LogAdfectus, Log, TEXT("%s: Light Attack Interrupt"), *GetNameSafe(this));
	}
}

void ACPPThirdPersonCharacter::StartHeavyAttack_Input(const struct FInputActionValue& Value)
{
	if (!IsAlive())
	{
		return;
	}

	IsHeavyAttacking_Input = true;
	StartHeavyAttack();
}

void ACPPThirdPersonCharacter::StopHeavyAttack_Input(const struct FInputActionValue& Value)
{
	if (!IsAlive())
	{
		return;
	}

	IsHeavyAttacking_Input = false;
	if (CanInterrupt)
	{
		StopAttack();
		UE_LOG(LogAdfectus, Log, TEXT("%s: Heavy Attack Interrupt"), *GetNameSafe(this));
	}
}

void ACPPThirdPersonCharacter::StartBlock_Input(const struct FInputActionValue& Value)
{
	if (!IsAlive())
	{
		return;
	}

	IsBlocking_Input = true;
	StartBlock();
}

void ACPPThirdPersonCharacter::StopBlock_Input(const struct FInputActionValue& Value)
{
	if (!IsAlive())
	{
		return;
	}

	IsBlocking_Input = false;
	if (!IsDefaultParry)
	{
		StopBlock();
	}
}

void ACPPThirdPersonCharacter::StartAttack()
{
	if (IsBlocking || IsAttacking || IsHeavyAttacking)
	{
		return;
	}
	if (!CanPerform)
	{
		return;
	}

	// FaceForward( );
	IsAttacking = true;
	StopSprint();

	if (EquippedWeapon != nullptr)
	{
		EquippedWeapon->SetIsHeavyAttack(false);
	}

	OnInterruptibleAnticipationEvent.Broadcast();
	UE_LOG(LogAdfectus, Log, TEXT("%s: Light Attack in Interruptible Anticipation"), *GetNameSafe(this));
}

void ACPPThirdPersonCharacter::StartHeavyAttack()
{
	if (IsBlocking || IsAttacking || IsHeavyAttacking)
	{
		return;
	}
	if (!CanPerform)
	{
		return;
	}

	// FaceForward( );
	IsHeavyAttacking = true;
	StopSprint();

	if (EquippedWeapon != nullptr)
	{
		EquippedWeapon->SetIsHeavyAttack(true);
	}

	OnInterruptibleAnticipationEvent.Broadcast();
	UE_LOG(LogAdfectus, Log, TEXT("%s: Heavy Attack in Interrruptible Anticipation"), *GetNameSafe(this));
}

void ACPPThirdPersonCharacter::StopAttack()
{
	// If StopAttack is called during interruptible anticipation, then CanInterrupt will be true. Kinda hacky perhaps?
	if (CanInterrupt)
	{
		OnInterruptedEvent.Broadcast();
	}
	else
	{
		OnInterruptibleFollowthroughEvent.Broadcast();
	}

	IsAttacking = false;
	IsHeavyAttacking = false;
	CanInterrupt = true;

	if (CanInterrupt)
	{
		if (AnimationTimeout > 0)
		{
			CanPerform = false;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([&]
			{
				CanPerform = true;

				if (IsHeavyAttacking_Input)
				{
					StartHeavyAttack();
				}

				if (IsAttacking_Input)
				{
					StartAttack();
				}

				if (IsBlocking_Input)
				{
					StartBlock();
				}

				if (IsSprinting_Input)
				{
					Sprint();
				}
			});
			GetWorld()->GetTimerManager().SetTimer(AnimationTimerHandle, TimerDelegate, AnimationTimeout, false);
		}
	}
	else
	{
		CanPerform = true;

		if (IsHeavyAttacking_Input)
		{
			StartHeavyAttack();
		}

		if (IsAttacking_Input)
		{
			StartAttack();
		}

		if (IsBlocking_Input)
		{
			StartBlock();
		}

		if (IsSprinting_Input)
		{
			Sprint();
		}
	}
}

void ACPPThirdPersonCharacter::StartBlock()
{
	if (IsBlocking || IsAttacking || IsHeavyAttacking)
	{
		return;
	}
	if (!CanPerform)
	{
		return;
	}

	// FaceForward();
	IsBlocking = true;
	StopSprint();

	OnInterruptibleAnticipationEvent.Broadcast();
	UE_LOG(LogAdfectus, Log, TEXT("%s: Block in Interruptible Anticipation"), *GetNameSafe(this));
}

void ACPPThirdPersonCharacter::StopBlock()
{
	OnInterruptibleFollowthroughEvent.Broadcast();

	IsBlocking = false;

	if (AnimationTimeout > 0)
	{
		CanPerform = false;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([&]
		{
			CanPerform = true;

			if (IsHeavyAttacking_Input)
			{
				StartHeavyAttack();
			}

			if (IsAttacking_Input)
			{
				StartAttack();
			}

			if (IsSprinting_Input)
			{
				Sprint();
			}
		});
		GetWorld()->GetTimerManager().SetTimer(AnimationTimerHandle, TimerDelegate, AnimationTimeout, false);
	}
}

void ACPPThirdPersonCharacter::OnBeginAttackUninterruptableAnticipation()
{
	if (!IsAttacking && !IsHeavyAttacking)
	{
		return;
	}

	CanInterrupt = false;
	if (EquippedWeapon != nullptr)
	{
		EquippedWeapon->SetDealsDamage(true);
	}

	if (AttackEffortAudio)
	{
		AttackEffortAudio->Play();
	}

	OnUninterruptibleAnticipationEvent.Broadcast();
}

void ACPPThirdPersonCharacter::OnBeginAttackFollowthrough()
{
	if (EquippedWeapon != nullptr)
	{
		EquippedWeapon->SetDealsDamage(false);
	}

	OnUninterruptibleFollowthroughEvent.Broadcast();
}

/////////////////////////////////////////////////////////////////

float ACPPThirdPersonCharacter::TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent,
                                           AController* EventInstigator, AActor* DamageCauser)
{
	if (IsImmune)
	{
		return 0;
	}

	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (IsBlocking)
	{
		// BLOCK
		CombatEvent_OnBlock.Broadcast(DamageCauser, this, MaxHealth, Health, Health, 0);

		if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{
			// ReSharper disable once CppCStyleCast
			const FPointDamageEvent* const PointDamageEvent = (FPointDamageEvent*)&DamageEvent;
			if (OnBlockFX)
			{
				const FRotator Rotator = UAdfectusUtilsLibrary::MakeRotatorFromDirections(
					FVector::UpVector, PointDamageEvent->HitInfo.ImpactNormal);
				// UNiagaraComponent* NiagaraComp =
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(), OnBlockFX, PointDamageEvent->HitInfo.Location, Rotator);
			}
		}
		if (OnBlockAudio)
		{
			OnBlockAudio->Play();
		}
	}
	else
	{
		// HIT
		const float PreviousHealth = Health;
		Health -= Damage;
		OnHealthChangedEvent.Broadcast(this);
		CombatEvent_OnHit.Broadcast(DamageCauser, this, MaxHealth, PreviousHealth, Health, Damage);

		if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		{
			// ReSharper disable once CppCStyleCast
			const FPointDamageEvent* const PointDamageEvent = (FPointDamageEvent*)&DamageEvent;
			if (OnHitFX)
			{
				const FRotator Rotator = UAdfectusUtilsLibrary::MakeRotatorFromDirections(
					FVector::UpVector, PointDamageEvent->HitInfo.ImpactNormal);
				// UNiagaraComponent* NiagaraComp =
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(), OnHitFX, PointDamageEvent->HitInfo.Location, Rotator);
				// Parameters can be set like this (see documentation for further info) - the names and type must match the user exposed parameter in the Niagara System
				// NiagaraComp->SetNiagaraVariableFloat(FString("StrengthCoef"), CoefStrength);
			}
		}
		CameraShake->Start();
		if (OnHitAudio)
		{
			OnHitAudio->Play();
		}

		// Update Damage Vignette
		if (DamageVignetteMaterialInstance)
		{
			const float VignetteFactor = (1.0f - GetNormalizedHealth()) * 0.5f;
			DamageVignetteMaterialInstance->SetScalarParameterValue("VignetteFactor", VignetteFactor);
		}
	}

	if (Health <= 0)
	{
		Die();
	}
	else
	{
		// If not dead, then make character immune for a bit.
		if (ImmunePeriodAfterDamage > 0)
		{
			IsImmune = true;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([&]
			{
				IsImmune = false;
			});
			GetWorld()->GetTimerManager().SetTimer(ImmuneTimerHandle, TimerDelegate, ImmunePeriodAfterDamage, false);
		}
	}
	return Damage;
}

float ACPPThirdPersonCharacter::GetNormalizedHealth() const
{
	return FMath::Clamp(Health / MaxHealth, 0.0f, 1.0f);
}

bool ACPPThirdPersonCharacter::IsAlive()
{
	return Health > 0;
}

bool ACPPThirdPersonCharacter::CanMove()
{
	// if (!IsAlive()) return false;
	// bool IsFalling = GetCharacterMovement()->IsFalling();
	// if (!IsFalling && (IsAttacking || IsHeavyAttacking || IsBlocking)) {
	//     return false;
	// }

	return true;
}

void ACPPThirdPersonCharacter::Die()
{
	Health = 0;
	this->GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionProfileName(FName("DeadCharacter"));
	PlayerToPlayerCollisionComponent->SetCollisionProfileName(FName("NoCollision"));

	IsAttacking_Input = false;
	IsHeavyAttacking_Input = false;
	IsBlocking_Input = false;
	IsSprinting_Input = false;
	IsImmune = false;
	IsAttacking = false;
	IsHeavyAttacking = false;
	IsBlocking = false;
	StopSprint();

	if (EquippedWeapon)
	{
		EquippedWeapon->SetDealsDamage(false);
	}

	OnDeathEvent.Broadcast(this);
}

void ACPPThirdPersonCharacter::Revive()
{
	Health = MaxHealth;
	OnHealthChangedEvent.Broadcast(this);
	OnReviveEvent.Broadcast(this);
	this->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCapsuleComponent()->SetCollisionProfileName(FName("Pawn"));
	PlayerToPlayerCollisionComponent->SetCollisionProfileName(FName("PlayerToPlayer"));

	if (DamageVignetteMaterialInstance)
	{
		const float VignetteFactor = (1.0f - GetNormalizedHealth()) * 0.5f;
		DamageVignetteMaterialInstance->SetScalarParameterValue("VignetteFactor", VignetteFactor);
	}
}

void ACPPThirdPersonCharacter::Reset()
{
	Super::Reset();
	SoftReset();
}

void ACPPThirdPersonCharacter::SoftReset()
{
	AgentComp->Reset();
	Revive();
}

/////////////////////////////////////////////////////////////////

void ACPPThirdPersonCharacter::AttachWeapon()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}

	EquippedWeapon->SetWielder(this);
	MoveIgnoreActorAdd(EquippedWeapon);
	EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	                                  TEXT(RIGHT_WEAPON_SOCKET));
}

void ACPPThirdPersonCharacter::EquipWeapon(AWeapon* Weapon)
{
	if (Weapon != nullptr)
	{
		/* Drop Equipped Weapon */
		DropWeapon();

		EquippedWeapon = Weapon;

		AttachWeapon();
	}
	else
	{
		UE_LOG(LogAdfectus, Warning, TEXT("Weapon is null."))
	}
}

void ACPPThirdPersonCharacter::EquipWeaponCopy(AWeapon* Weapon)
{
	if (Weapon != nullptr)
	{
		/* Destroy Equipped Weapon. */
		DropWeapon(true);

		/* Create a copy of the weapon and attaching it. */
		FActorSpawnParameters parameters;
		parameters.Template = Weapon;
		EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(Weapon->GetClass(), parameters);

		AttachWeapon();
	}
}

void ACPPThirdPersonCharacter::DropWeapon(bool bDestroy)
{
	if (EquippedWeapon != nullptr)
	{
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		MoveIgnoreActorRemove(EquippedWeapon);
		if (bDestroy)
		{
			EquippedWeapon->Destroy();
		}
		// else activate physics and change the collisions profile
		EquippedWeapon = nullptr;
	}
}

void ACPPThirdPersonCharacter::AttachOffhandWeapon()
{
	if (EquippedOffhandWeapon == nullptr)
	{
		return;
	}

	EquippedOffhandWeapon->SetWielder(this);
	MoveIgnoreActorAdd(EquippedOffhandWeapon);
	EquippedOffhandWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	                                         TEXT(LEFT_WEAPON_SOCKET));
}

void ACPPThirdPersonCharacter::EquipOffhandWeapon(AWeapon* Weapon)
{
	if (Weapon != nullptr)
	{
		/* Drop Equipped Weapon */
		DropOffhandWeapon();

		EquippedOffhandWeapon = Weapon;

		AttachOffhandWeapon();
	}
}

void ACPPThirdPersonCharacter::EquipOffhandWeaponCopy(AWeapon* Weapon)
{
	if (Weapon != nullptr)
	{
		/* Destroy Equipped Weapon. */
		DropOffhandWeapon(true);

		/* Create a copy of the weapon and attaching it. */
		FActorSpawnParameters parameters;
		parameters.Template = Weapon;
		EquippedOffhandWeapon = GetWorld()->SpawnActor<AWeapon>(Weapon->GetClass(), parameters);

		AttachOffhandWeapon();
	}
}

void ACPPThirdPersonCharacter::DropOffhandWeapon(bool bDestroy)
{
	if (EquippedOffhandWeapon != nullptr)
	{
		EquippedOffhandWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		MoveIgnoreActorRemove(EquippedOffhandWeapon);
		if (bDestroy)
		{
			EquippedOffhandWeapon->Destroy();
		}
		EquippedOffhandWeapon = nullptr;
	}
}

void ACPPThirdPersonCharacter::AttachShield()
{
	if (EquippedShield == nullptr)
	{
		return;
	}

	EquippedShield->SetWielder(this);
	EquippedShield->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	                                  TEXT(LEFT_SHIELD_SOCKET));
}

void ACPPThirdPersonCharacter::EquipShield(AShield* Shield)
{
	if (Shield != nullptr)
	{
		/* Drop Equipped Shield */
		DropShield();

		EquippedShield = Shield;

		AttachShield();
	}
}

void ACPPThirdPersonCharacter::EquipShieldCopy(AShield* Shield)
{
	if (Shield != nullptr)
	{
		/* Destroy Equipped Shield. */
		DropShield(true);

		/* Create a copy of the shield and attaching it. */
		FActorSpawnParameters parameters;
		parameters.Template = Shield;
		EquippedShield = GetWorld()->SpawnActor<AShield>(Shield->GetClass(), parameters);

		AttachShield();
	}
}

void ACPPThirdPersonCharacter::DropShield(bool bDestroy)
{
	if (EquippedShield != nullptr)
	{
		EquippedShield->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		if (bDestroy)
		{
			EquippedShield->Destroy();
		}
		EquippedShield = nullptr;
	}
}

void ACPPThirdPersonCharacter::IsAvailable_Implementation(AInteractable* Interactable)
{
	// UE_LOG(LogAdfectus, Warning, TEXT("%s began interaction collision with %s."), *GetNameSafe(this), *GetNameSafe(interactable));
	Interactables.AddUnique(Interactable);
}

void ACPPThirdPersonCharacter::IsUnavailable_Implementation(AInteractable* Interactable)
{
	// UE_LOG(LogAdfectus, Warning, TEXT("%s ended interaction collision with %s"), *GetNameSafe(this), *GetNameSafe(interactable));
	Interactables.Remove(Interactable);
	Interactable->SetHighlight(false);
}

void ACPPThirdPersonCharacter::InteractWithNearest()
{
	float MinDistance = -1;
	AInteractable* Closest = nullptr;
	for (AInteractable* Inter : Interactables)
	{
		if (const float Distance = Inter->GetDistanceTo(this); MinDistance == -1 || Distance < MinDistance)
		{
			MinDistance = Distance;
			Closest = Inter;
		}
	}
	if (Closest == nullptr)
	{
		return;
	}

	// UE_LOG(LogAdfectus, Display, TEXT("%s interacts with %s"), *GetNameSafe(this), *GetNameSafe(closest));
	Closest->SetHighlight(false);
	Closest->Interact(this);
}

void ACPPThirdPersonCharacter::HighlightNearestInteractable()
{
	float MinDistance = -1;
	AInteractable* Closest = nullptr;
	for (AInteractable* Inter : Interactables)
	{
		Inter->SetHighlight(false);
		if (const float Distance = Inter->GetDistanceTo(this); MinDistance == -1 || Distance < MinDistance)
		{
			MinDistance = Distance;
			Closest = Inter;
		}
	}
	if (Closest == nullptr)
	{
		return;
	}

	Closest->SetHighlight(true);
}

bool ACPPThirdPersonCharacter::HasInteractables() const
{
	return Interactables.Num() > 0;
}


ACPPThirdPersonCharacter* ACPPThirdPersonCharacter::GetLastHitEnemy() const
{
	return LastHitEnemy;
}

void ACPPThirdPersonCharacter::ResetLastHitEnemy()
{
	LastHitEnemy = nullptr;
}

void ACPPThirdPersonCharacter::UpdateLastHitEnemy(ACPPThirdPersonCharacter* Actor)
{
	LastHitEnemy = Actor;
}

void ACPPThirdPersonCharacter::UpdateLastHitEnemy(AActor* Actor)
{
	ACPPThirdPersonCharacter* Character = Cast<ACPPThirdPersonCharacter>(Actor);
	if (Character == nullptr)
	{
		return;
	}

	UpdateLastHitEnemy(Character);
}
