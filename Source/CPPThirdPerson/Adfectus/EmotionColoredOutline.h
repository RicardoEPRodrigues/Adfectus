// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmotionColoredOutline.generated.h"

/**
 * Provides an outline to a mesh with different colors depending on the felt emotions. 
 * The outline is implemented with a post processing outline based on this work https://www.parallelcube.com/2017/12/05/outline-effect-part-1/
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CPPTHIRDPERSON_API UEmotionColoredOutline : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEmotionColoredOutline();

protected:
	/**
	 * The 3motion Agent Component to be found in the character this component is bound to.
	 */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Emotion")
	class UThreeMotionAgentComponent* AgentComponent;

	/**
	 * Stencil step for confidence emotion.
	 */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Emotion")
	int StencilStepConfidence = 40;
	/**
	 * Stencil step for Defensiveness emotion.
	 */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Emotion")
	int StencilStepDefensiveness = 50;
	/**
	 * Stencil step for Fear emotion.
	 */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Emotion")
	int StencilStepFear = 60;
	/**
	 * Stencil step for Rage emotion.
	 */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Emotion")
	int StencilStepRage = 70;

	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/**
	 * The meshes to be illuminated when the outline is activated.
	 */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Emotion")
	TArray<class UMeshComponent*> OutlinedMeshes;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
