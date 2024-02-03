// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"

#include "EmojiFacialExpressionWidgetComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CPPTHIRDPERSON_API UEmojiFacialExpressionWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	
	/**
	 * The 3motion Agent Component to be found in the character this component is bound to.
	 */
	UPROPERTY(BlueprintReadWrite, Category="Emotion")
	class UThreeMotionAgentComponent* AgentComponent;

public:
	UEmojiFacialExpressionWidgetComponent();
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Category = "Emotion", EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* EmojiMaterial;

	UPROPERTY()
	float PreviousEmotionIndex = -1;
};
