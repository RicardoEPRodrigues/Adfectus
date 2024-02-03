// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TMEmotion.generated.h"

/**
 * 3Motion Emotion. It represents an emotion to be manipulated inside 3Motion system.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UTMEmotion : public UDataAsset
{
	GENERATED_BODY()

public:
	UTMEmotion();
	
	bool operator==(const UTMEmotion& Emotion) const;

	/// Unique name of the emotion. Used for Identification.
	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite)
	FString Name;
	
	/// Description of the emotion. Used for text based logging and UI.
	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite,
		meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
	float Intensity;

	/// List of reasons why this emotion was cast.
	/// These reasons are previously perceived percepts.
	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite)
	TArray<class UTMPercept*> Reasons; 
};
