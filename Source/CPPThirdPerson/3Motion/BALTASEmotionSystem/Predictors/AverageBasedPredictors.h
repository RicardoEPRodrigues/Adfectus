// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPredictor.h"
#include "UObject/Object.h"
#include "AverageBasedPredictors.generated.h"

/**
 * Predictor based on a moving average of a given window size.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UMovingAveragePredictor : public UObject, public IPredictor
{
	GENERATED_BODY()

public:
	/**
	 * @brief Size of the window the predictor will use to view data from Values in a Predict() call.
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	int WindowSize = 5;

	/**
	 * Return value when there aren't enough values to compute the prediction.
	 * Defaults to 0.5, assuming a normalized data.
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	float DefaultValue = .5f;
	
	virtual float Predict_Implementation(const TArray<float>& Values, const TArray<float>& Predictions) override;
};

/**
 * Predictor based on a weighted moving average of a given window size and respective weights.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UWeightedMovingAveragePredictor : public UObject, public IPredictor
{
	GENERATED_BODY()

public:
	/**
	 * @brief Size (`Weights.Num()`) and weights of the window the predictor will use to compute the average.
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	TArray<float> Weights {0.5f, .2f, .1f, .1f, .1f};

	/**
	 * Return value when there aren't enough values to compute the prediction.
	 * Defaults to 0.5, assuming a normalized data.
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	float DefaultValue = .5f;
	
	virtual float Predict_Implementation(const TArray<float>& Values, const TArray<float>& Predictions) override;
};

/**
 * Predictor based on an Exponential moving average.
 * This makes heavy use of the predictions list to store previous averages.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UExponentialMovingAveragePredictor : public UObject, public IPredictor
{
	GENERATED_BODY()

public:

	/**
	 * Return value when there aren't enough values to compute the prediction.
	 * Defaults to 0.5, assuming a normalized data.
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	float DefaultValue = .5f;

	/**
	 * Value used to smooth the curve of the moving average.
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	float Smoothing = 2.0f;

	virtual float Predict_Implementation(const TArray<float>& Values, const TArray<float>& Predictions) override;
};

/**
 * Predictor based on the simple approximation defined by Martinho.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UMartinhoSimplePredictor : public UObject, public IPredictor
{
	GENERATED_BODY()

public:

	/**
	 * Return value when there aren't enough values to compute the prediction.
	 * Defaults to 0.5, assuming a normalized data.
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	float DefaultValue = .5f;
	
	/**
	 * Value to multiply the exogenous value.
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	float ExogenousFactor = 10.0f;

	virtual float Predict_Implementation(const TArray<float>& Values, const TArray<float>& Predictions) override;

	virtual float PredictWithDesiredValue_Implementation(const TArray<float>& Values, const TArray<float>& Predictions,
		const float DesiredValue) override;
};