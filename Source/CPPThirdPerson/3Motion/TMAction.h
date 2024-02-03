// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ETMActionStage.h"
#include "Engine/DataAsset.h"
#include "TMAction.generated.h"

/**
 * 3Motion Action. It represents an action to be manipulated inside 3Motion system.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UTMAction : public UDataAsset
{
	GENERATED_BODY()

public:
	UTMAction();

	/// Unique name of the action. Used for Identification.
	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite)
	FString Name;

	/// Description of the action. Used for text based logging and UI.
	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite)
	FString Description;

	/// Current stage of the action. Used for percepts information retrieval.
	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite)
	ETMActionStage Stage = ETMActionStage::None;
	
	bool operator==(const UTMAction& Action) const;
};
