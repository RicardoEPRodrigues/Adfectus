// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPredictor.h"
#include "UObject/Object.h"
#include "DerivativePredictors.generated.h"

/**
 * Predictor based on a base predictor and adding a prediction of the first derivative of previously perceived values.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UAdditiveFirstDerivativePredictor : public UObject, public IPredictor
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
	 * Predictor used on the base values
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	TScriptInterface<IPredictor> BasePredictor;

	/**
	 * Predictor used on the First derivative of the base values.
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	TScriptInterface<IPredictor> FirstDerivativePredictor;

	virtual float Predict_Implementation(const TArray<float>& Values, const TArray<float>& Predictions) override;

	virtual float PredictWithDesiredValue_Implementation(const TArray<float>& Values, const TArray<float>& Predictions,
	                                                     const float DesiredValue) override;
};


/**
 * Predictor based on a first derivative of previously perceived values.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UFirstDerivativeOnlyPredictor : public UObject, public IPredictor
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
	 * Predictor used on the First derivative of the base values.
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	TScriptInterface<IPredictor> FirstDerivativePredictor;

	virtual float Predict_Implementation(const TArray<float>& Values, const TArray<float>& Predictions) override;

	virtual float PredictWithDesiredValue_Implementation(const TArray<float>& Values, const TArray<float>& Predictions,
	                                                     const float DesiredValue) override;
};


/**
 * Predictor based on a base predictor and adding a prediction of the first and second derivative of previously perceived values.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UAdditiveSecondDerivativePredictor : public UObject, public IPredictor
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
	 * Predictor used on the base values
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	TScriptInterface<IPredictor> BasePredictor;

	/**
	 * Predictor used on the First derivative of the base values.
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	TScriptInterface<IPredictor> FirstDerivativePredictor;

	/**
	 * Predictor used on the Second derivative of the base values.
	 */
	UPROPERTY(Category="BALTAS", BlueprintReadWrite, EditAnywhere)
	TScriptInterface<IPredictor> SecondDerivativePredictor;

	virtual float Predict_Implementation(const TArray<float>& Values, const TArray<float>& Predictions) override;

	virtual float PredictWithDesiredValue_Implementation(const TArray<float>& Values, const TArray<float>& Predictions,
	                                                     const float DesiredValue) override;
};
