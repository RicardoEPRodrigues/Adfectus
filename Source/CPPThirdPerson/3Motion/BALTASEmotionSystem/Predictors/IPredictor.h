// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPredictor.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UPredictor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CPPTHIRDPERSON_API IPredictor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * @brief Computes a new prediction based on provided values.
	 * @param Values Values recorded until now.
	 * @param Predictions Previous predictions computed from the given values.
	 * @return Prediction computed based on the given values and previous predictions.
	 */
	UFUNCTION(Category="BALTAS", BlueprintCallable, BlueprintNativeEvent)
	float Predict(const TArray<float>& Values, const TArray<float>& Predictions);

	/**
	 * @brief Computes a new prediction based on provided values.
	 * @param Values Values recorded until now.
	 * @param Predictions Previous predictions computed from the given values.
	 * @param DesiredValue Value of a desired goal. Can be used to influence predictions.
	 * @return Prediction computed based on the given values and previous predictions.
	 */
	UFUNCTION(Category="BALTAS", BlueprintCallable, BlueprintNativeEvent)
	float PredictWithDesiredValue(const TArray<float>& Values, const TArray<float>& Predictions,
	const float DesiredValue);
	virtual float PredictWithDesiredValue_Implementation(const TArray<float>& Values, const TArray<float>& Predictions,
								  const float DesiredValue);
};
