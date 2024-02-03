// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Emotivector.generated.h"

/**
* Sensation Valence Enum
* Defined a reward or punishment of a Sensation
*/
UENUM(BlueprintType)
enum class ESensationValence : uint8
{
	Punishment UMETA(DisplayName = "Punishment"),
	Reward UMETA(DisplayName = "Reward"),
	Other UMETA(DisplayName = "Other"),
};

/**
* Sensation Type Enum
* Defined difference between expected result and actual result reflected in a Sensation.
*/
UENUM(BlueprintType)
enum class ESensationType : uint8
{
	WorseThanExpected UMETA(DisplayName = "WorseThanExpected"),
	AsExpected UMETA(DisplayName = "AsExpected"),
	BetterThanExpected UMETA(DisplayName = "BetterThanExpected"),
	Other UMETA(DisplayName = "Other"),
};

/**
 * Class Sensation. Holds information regarding a difference in expected result and actual result.
 * Used in Emotivectors to describe changes in them.
 */
UCLASS(Blueprintable)
class CPPTHIRDPERSON_API USensation : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion)
	ESensationValence Valence = ESensationValence::Other;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion)
	ESensationType Type = ESensationType::AsExpected;

	/**
	 * Initial Salience of the Sensation.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion)
	float InitialSalience = 0.0f;
	/**
	 * Current Salience of the Sensation.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion)
	float Salience = 0.0f;

	/**
	 * Initial lifetime of the Sensation in Seconds. 
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion)
	float InitialLifetime = 5.0f;

	/**
	 * Current lifetime of the Sensation in Seconds. 
	*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category=Emotion)
	float Lifetime = -1.0f;

	/**
	 * Importance of the Sensation. Used to calculate the Salience of the Sensation. Value between 0 and 1.
	 */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category=Emotion)
	float Importance = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion)
	TWeakObjectPtr<class UEmotivector> Emotivector;
};

/**
 * Base class for the Emotivector. A concept developed in Martinho, C., & Paiva, A. (2006). Using Anticipation to Create Believable Behaviour. Proceedings of the National Conference on Artificial Intelligence, 1, 175–180. http://www.aaai.org/Papers/AAAI/2006/AAAI06-028.pdf
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UEmotivector : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class ACPPThirdPersonCharacter* Owner;
	UPROPERTY()
	class UThreeMotionAgentComponent* OwnerAgentComponent;

public:
	UEmotivector();

	/**
	 * Module that generates and manages this Emotivector.
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category=Emotion)
	class UTMBaltasModule* OwnerModule;

	/**
	 * Function called to initialize the Emotivector after the Module is already setup.
	 * Normally initialized during Initialize of @class UTMBaltasModule
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Emotion)
	void Initialize(class UTMBaltasModule* Module);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Emotion)
	void Reset();

	/**
	 * List of values for the emotivector to use for predictions.
	 */
	UPROPERTY(BlueprintReadOnly, Category=Emotion)
	TArray<float> Values{};

	/**
	 * List of previous predictions computed by this emotivector. Should be no longer than `Values.Num() - 1`.
	 */
	UPROPERTY(BlueprintReadOnly, Category=Emotion)
	TArray<float> Predictions{};

	/**
	 * Predictor used to calculate the expected values based on the Values list.
	 */
	UPROPERTY(BlueprintReadWrite, Category=Emotion)
	class UObject* Predictor;

	/**
	 * List of prediction errors from Sensed and Expected Rewards.
	*/
	UPROPERTY(BlueprintReadOnly, Category=Emotion)
	TArray<float> PredictionErrors{};

	/**
	 * List of predicted prediction errors based on the prediction errors from Sensed and Expected Rewards.
	*/
	UPROPERTY(BlueprintReadOnly, Category=Emotion)
	TArray<float> MetaPredictions{};

	/**
	 * Predictor used to calculate the expected prediction errors based on the prediction errors from Sensed and Expected Rewards.
	 */
	UPROPERTY(BlueprintReadWrite, Category=Emotion)
	class UObject* MetaPredictor;

	/**
	 * List of salient sensations to output when calling the Update function.
	 */
	UPROPERTY(BlueprintReadWrite, Category=Emotion)
	TArray<USensation*> SalientSensations;

	/**
	 * True if you desire to use a value that the emotivector hopes to reach.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion)
	bool UseDesiredValue = false;

	/**
	 * A value that the emotivector hopes to reach.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion)
	float DesiredValue = 0.5f;

	/**
	 * How important this emotivector is to the module it is held in.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Emotion, meta=(ClampMin=0.0f))
	float Importance = 1.0f;

	/**
	 * Determines if the values added to Values and Predictions should be clamped.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Emotion|Clamp")
	bool ClampValues = false;

	/**
	 * Value interval in which to clamp Values and Predictions. Only clamps if ClampValues is true.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Emotion|Clamp")
	FFloatInterval ClampInterval = FFloatInterval(0.0f, 1.0f);

	/**
	 * Adds a value to the list of computed values and runs a prediction on this new value.
	 * If the sensation is salient, it will be added to the SalientSensations list.
	 * @param Value Value to be added to the current computed values
	 */
	UFUNCTION(BlueprintCallable, Category=Emotion)
	void AddValue(float Value);

	/**
	 * Updates the emotivector so that it computes sensations based on new values.
	 * @return An array of new salient sensations
	 */
	UFUNCTION(BlueprintCallable, Category=Emotion)
	TArray<class USensation*> Update();

	/**
	 * Implementable function that is called before the computation of new sensations in the Update function.
	 */
	UFUNCTION(BlueprintNativeEvent, Category=Emotion)
	void OnUpdate();

	/**
	 * Create a Sensation in the context of this Emotivector.
	 * @param Valence Valence of the created Sensation 
	 * @param Type Type of the created Sensation
	 * @param Salience Salience of the created Sensation
	 * @param Lifetime Duration of the Sensation in seconds.
	 * @param Importance Importance of the Sensation. A value between 0 and 1. It impacts the salience of the Sensation.
	 * @return Pointer to the newly created Sensation.
	 */
	UFUNCTION(BlueprintCallable, Category=Emotion)
	USensation* MakeSensation(const ESensationValence Valence, const ESensationType Type, const float Salience,
	                          const float Lifetime = 5.0f, const float SensationImportance = 1.0f);
};
