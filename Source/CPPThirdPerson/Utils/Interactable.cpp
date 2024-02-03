// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"

#include "../AdfectusLogging.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Interactor.h"


// Sets default values
AInteractable::AInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(RootComponent);

	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Forward Arrow"));
	ForwardArrow->SetRelativeLocation({0, 0, 10.0f});
	ForwardArrow->SetupAttachment(RootComponent);

	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	InteractableMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AInteractable::OnBeginOverlap);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AInteractable::OnEndOverlap);
	
	if (InteractableMesh)
	{
		InteractableMesh->SetCustomDepthStencilValue(1);
	}

}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractable::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	if (!bIsAvailable)
	{
		return;
	}
	// UE_LOG(LogAdfectus, Log, TEXT("%s %s began overlap with %s %s."), *GetNameSafe(OverlappedComponent), *GetNameSafe(this), *GetNameSafe(OtherComp), *GetNameSafe(OtherActor));

	// Try to Execute on C++ layer
	if (Cast<IInteractor>(OtherActor) ||
		// or, Execute Interface on Blueprint layer instead
		OtherActor->GetClass()->ImplementsInterface(UInteractor::StaticClass()))
	{
		IInteractor::Execute_IsAvailable(OtherActor, this);
		Interactors.Add(OtherActor);
	}
}

void AInteractable::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bIsAvailable)
	{
		return;
	}
	// UE_LOG(LogAdfectus, Log, TEXT("%s ended overlap with %s"), *GetNameSafe(this), *GetNameSafe(OtherActor));

	// Try to Execute on C++ layer
	if (Cast<IInteractor>(OtherActor) ||
		// or, Execute Interface on Blueprint layer instead
		OtherActor->GetClass()->ImplementsInterface(UInteractor::StaticClass()))
	{
		IInteractor::Execute_IsUnavailable(OtherActor, this);
		Interactors.Remove(OtherActor);
	}
}

void AInteractable::Interact_Implementation(AActor* Actor)
{
	if (!bIsAvailable)
	{
		return;
	}

	// UE_LOG(LogAdfectus, Verbose, TEXT("%s interacted with %s."), *GetNameSafe(Actor), *GetNameSafe(this));

	OnInteraction.Broadcast(Actor);
}

void AInteractable::SetHighlight_Implementation(const bool bShow)
{
	if (!bIsAvailable)
	{
		return;
	}

	if (!InteractableMesh)
	{
		return;
	}

	InteractableMesh->SetRenderCustomDepth(bShow);
}


bool AInteractable::IsAvailable() const
{
	return bIsAvailable;
}

void AInteractable::SetAvailable(const bool bAvailable)
{
	if (this->bIsAvailable == bAvailable)
	{
		return;
	}

	this->bIsAvailable = bAvailable;

	// Empty Interactors
	for (auto InteractorPtr : Interactors)
	{
		if (const auto Interactor = InteractorPtr.Get())
		{
			IInteractor::Execute_IsUnavailable(Interactor, this);
		}
	}
	Interactors.Empty();

	if (bAvailable)
	{
		TArray<AActor*> OverlappingActors;
		CollisionSphere->GetOverlappingActors(OverlappingActors);
		for (auto OverlappingActor : OverlappingActors)
		{
			if (Cast<IInteractor>(OverlappingActor) ||
				OverlappingActor->GetClass()->ImplementsInterface(UInteractor::StaticClass()))
			{
				IInteractor::Execute_IsAvailable(OverlappingActor, this);
				Interactors.Add(OverlappingActor);
			}
		}
	}
	else
	{
		SetHighlight(false);
	}
}
