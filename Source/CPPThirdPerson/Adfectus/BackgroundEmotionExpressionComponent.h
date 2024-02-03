// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BackgroundEmotionExpressionComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CPPTHIRDPERSON_API UBackgroundEmotionExpressionComponent : public USceneComponent
{
	GENERATED_BODY()

	UPROPERTY()
	class UNiagaraComponent* BackgroundNiagaraComp = nullptr;
protected:
	
	/**
	 * The 3motion Agent Component to be found in the character this component is bound to.
	 */
	UPROPERTY(BlueprintReadWrite, Category="Emotion")
	class UThreeMotionAgentComponent* AgentComponent;

public:
	UBackgroundEmotionExpressionComponent();
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Category = "Emotion", EditAnywhere, BlueprintReadWrite)
	class UNiagaraSystem* BackgroundFX;

	UPROPERTY()
	float PreviousEmotionIndex = -1;
};
