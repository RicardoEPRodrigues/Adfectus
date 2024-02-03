// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TMTheoryOfMind.generated.h"

USTRUCT(BlueprintType)
struct FPerceptContainer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TWeakObjectPtr<class UTMPercept>> Percepts;
};

/**
 * @brief Class that holds the theory of mind of the agent. 
 * It holds all data that can reflect the world the agent is perceiving.
 * It includes emotions and actions the agent can feel.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UTMTheoryOfMind : public UDataAsset
{
	GENERATED_BODY()
	
	/// Indicates whether new percepts have been added to the history this frame
	bool _HasNewPercepts = false;
	
public:

	UTMTheoryOfMind();

	/**
	 * List of available actions for the agent to execute.
	 * @note Action class only stores values and does not contain active behavior.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<class UTMAction*> AvailableActions;

	/**
	 * List of available emotions for the agent to feel.
	 * @note Emotion class only stores values and does not contain active behavior.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<class UTMEmotion*> AvailableEmotions;

	/**
	 * Action desired to be performed by this actor. Can be null. To be set by Decision Modules.
	 */
	UPROPERTY(BlueprintReadWrite)
	class UTMAction* DesiredAction;

	/**
	 * Emotion desired to be felt by this actor. Can be null. To be set by React Modules.
	 */
	UPROPERTY(BlueprintReadWrite)
	TArray<class UTMEmotion*> DesiredEmotions;

	/**
	 * Action being currently performed by this actor. Can be null.
	 */
	UPROPERTY(BlueprintReadWrite)
	class UTMAction* CurrentAction;

	/**
	 * Emotion being currently felt by this actor. Can be null.
	 */
	UPROPERTY(BlueprintReadWrite)
	TArray<class UTMEmotion*> CurrentEmotions;

	/**
	 * An integer that determines the number of items to be kept in the History of perceived.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PerceptHistoryWindow = 500;

	/**
	 * History list of perceived percepts seen by the agent.
	 * New events are placed at the end when using the AddPerceptToHistory function.
	 */
	UPROPERTY(BlueprintReadWrite)
	TArray<class UTMPercept*> PerceptHistory;

	/**
	 * History list per actor of perceived percepts seen by the agent.
	 * New events are placed at the end when using the AddPerceptToHistory function.
	 */
	UPROPERTY(BlueprintReadWrite)
	TMap<class AActor*, FPerceptContainer> PerceptHistoryPerActor;

	/// Adds percepts to history and history per actor members.
	/// Should be used instead of the direct Add members of the structures.
	UFUNCTION(Category="3Motion", BlueprintCallable)
	void AddPerceptToHistory(class UTMPercept* Percept);

	/**
	 * @brief Removes old percepts if the limit is reached. 
	 */
	UFUNCTION(Category="3Motion", BlueprintCallable)
	void RemoveOldPercepts();

	/**
	 * @brief Indicates whether new percepts have been added to the history this frame
	 */
	UFUNCTION(Category="3Motion", BlueprintCallable)
	bool HasNewPercepts();

	/**
	 * @brief Clears old state from all fields
	 */
	UFUNCTION(Category="3Motion", BlueprintCallable)
	void Reset();

	/**
	 * Get the current emotion with the highest intensity. Useful if you only want to get a single emotion.
	 * @return The strongest emotion. Can be NULL if no emotion is being felt.
	 */
	UFUNCTION(Category="3Motion", BlueprintNativeEvent)
	class UTMEmotion* GetStrongestEmotion();
};
