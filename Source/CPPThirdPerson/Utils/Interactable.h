// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS()
class CPPTHIRDPERSON_API AInteractable : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Category = Interaction, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionSphere;

	UPROPERTY(Category = Interaction, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ForwardArrow;

	UPROPERTY(Category = Interaction, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* InteractableMesh;

	/**
	 * @brief Determines if this interactable is available or should be ignored.
	 */
	UPROPERTY(Category="Interactable", VisibleAnywhere, BlueprintGetter=IsAvailable, BlueprintSetter=SetAvailable)
	bool bIsAvailable = true;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> Interactors;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AInteractable();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = Interaction)
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Category = Interaction)
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Interaction)
	void Interact(class AActor* Actor);

	/**
	 * Function that turns on or off the highlight on the mesh using CustomDepth.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=Interaction)
	void SetHighlight(bool bShow);

	UFUNCTION(BlueprintGetter, Category="Interactable")
	bool IsAvailable() const;

	UFUNCTION(BlueprintSetter, Category="Interactable")
	void SetAvailable(bool bAvailable);

	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionDelegate, class AActor*, Actor);

	/**
	 * @brief Called when an interaction with this Interactable occurs.
	 */
	UPROPERTY(Category="Interactable", BlueprintCallable, BlueprintAssignable)
	FInteractionDelegate OnInteraction;
};
