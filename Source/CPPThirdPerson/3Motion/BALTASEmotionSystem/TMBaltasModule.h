// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPThirdPerson/3Motion/TMModule.h"
#include "TMBaltasModule.generated.h"

/**
 * BALTAS - Belief and Anticipation Laced Tenacious Affect System
 * This is a React module for 3Motion that implements an Emotional System based on Emotivector.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UTMBaltasModule : public UTMModule
{
	GENERATED_BODY()

public:
	UTMBaltasModule();

	virtual void Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent) override;
	virtual void Execute_Implementation() override;
	virtual void EndPlay_Implementation() override;

	/**
	 * Selects most salient sensation and applies its emotion.
	 * @param PossibleEmotions Emotions and Sensations to select from.
	 * @return Emotion selected to be felt.
	 */
	static class UTMEmotion* SelectMostSalientEmotion(TArray<TPair<class USensation*, class UTMEmotion*>> PossibleEmotions);

	/**
	 * Selects most common emotion by frequency.
	 * @param PossibleEmotions Emotions and Sensations to select from.
	 * @return Emotion selected to be felt.
	 */
	static class UTMEmotion* SelectMostCommonEmotion(TArray<TPair<class USensation*, class UTMEmotion*>> PossibleEmotions);

	/**
	 * Selects most common emotion by frequency. If multiple exist with the same frequency, selects most salient.
	 * @param PossibleEmotions Emotions and Sensations to select from.
	 * @return Emotion selected to be felt.
	 */
	static class UTMEmotion* SelectRarityEmotion(TArray<TPair<class USensation*, class UTMEmotion*>> PossibleEmotions);
	
	/**
	 * Selects the emotion with highest salience, but it will accumulate the salience on repeated emotions.
	 * @param PossibleEmotions Emotions and Sensations to select from.
	 * @return Emotion selected to be felt.
	 */
	static class UTMEmotion* SelectPowerfulEmotion(TArray<TPair<class USensation*, class UTMEmotion*>> PossibleEmotions);

	UPROPERTY()
	class ACPPThirdPersonCharacter* Owner;

	UPROPERTY()
	class UThreeMotionAgentComponent* OwnerAgentComponent;

	/**
	 * Class defining the personality with emotions limits of the character.
	 */
	UPROPERTY(Category = "BALTAS", EditAnywhere, BlueprintReadWrite)
	class UBaltasPersonality* Personality;

	/**
	 * The emotivectors to predict values.
	 */
	UPROPERTY(Category = "BALTAS", EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<class UEmotivector*> Emotivectors;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="BALTAS")
	TArray<class USensation*> SalientSensations;
};
