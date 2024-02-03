// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.


#include "DerivativePredictors.h"

#include "CPPThirdPerson/3Motion/TMLogging.h"
#include "CPPThirdPerson/Utils/AdfectusUtilsLibrary.h"

float UAdditiveFirstDerivativePredictor::Predict_Implementation(const TArray<float>& Values,
                                                                const TArray<float>& Predictions)
{
	if (!BasePredictor)
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: No Base Predictor is defined."), *GetNameSafe(this));
		return DefaultValue;
	}

	const float BasePrediction = IPredictor::Execute_Predict(BasePredictor.GetObject(), Values, Predictions);

	if (!FirstDerivativePredictor)
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: No First Derivative Predictor is defined."), *GetNameSafe(this));
		return BasePrediction;
	}

	if (Values.Num() < 2)
	{
		return BasePrediction;
	}

	TArray<float> FirstDerivativeValues;
	UAdfectusUtilsLibrary::Derivative(Values, FirstDerivativeValues);

	const float FirstDerivativePrediction = IPredictor::Execute_Predict(FirstDerivativePredictor.GetObject(),
	                                                                    FirstDerivativeValues, TArray<float>());

	return BasePrediction + FirstDerivativePrediction;
}

float UAdditiveFirstDerivativePredictor::PredictWithDesiredValue_Implementation(const TArray<float>& Values,
	const TArray<float>& Predictions, const float DesiredValue)
{
	if (!BasePredictor)
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: No Base Predictor is defined."), *GetNameSafe(this));
		return DefaultValue;
	}

	const float BasePrediction = IPredictor::Execute_PredictWithDesiredValue(
		BasePredictor.GetObject(), Values, Predictions, DesiredValue);

	if (!FirstDerivativePredictor)
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: No First Derivative Predictor is defined."), *GetNameSafe(this));
		return BasePrediction;
	}

	if (Values.Num() < 2)
	{
		return BasePrediction;
	}

	TArray<float> FirstDerivativeValues;
	UAdfectusUtilsLibrary::Derivative(Values, FirstDerivativeValues);

	const float FirstDerivativePrediction = IPredictor::Execute_PredictWithDesiredValue(
		FirstDerivativePredictor.GetObject(), FirstDerivativeValues, TArray<float>(), DesiredValue);

	return BasePrediction + FirstDerivativePrediction;
}

float UFirstDerivativeOnlyPredictor::Predict_Implementation(const TArray<float>& Values,
                                                            const TArray<float>& Predictions)
{
	if (!FirstDerivativePredictor)
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: No First Derivative Predictor is defined."), *GetNameSafe(this));
		return DefaultValue;
	}

	if (Values.Num() < 2)
	{
		return Values.Num() == 1 ? Values[0] : DefaultValue;
	}

	TArray<float> FirstDerivativeValues;
	UAdfectusUtilsLibrary::Derivative(Values, FirstDerivativeValues);

	const float FirstDerivativePrediction = IPredictor::Execute_Predict(FirstDerivativePredictor.GetObject(),
	                                                                    FirstDerivativeValues, TArray<float>());

	return FirstDerivativePrediction;
}

float UFirstDerivativeOnlyPredictor::PredictWithDesiredValue_Implementation(const TArray<float>& Values,
                                                                            const TArray<float>& Predictions,
                                                                            const float DesiredValue)
{
	if (!FirstDerivativePredictor)
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: No First Derivative Predictor is defined."), *GetNameSafe(this));
		return DefaultValue;
	}

	if (Values.Num() < 2)
	{
		return Values.Num() == 1 ? Values[0] : DefaultValue;
	}

	TArray<float> FirstDerivativeValues;
	UAdfectusUtilsLibrary::Derivative(Values, FirstDerivativeValues);

	const float FirstDerivativePrediction = IPredictor::Execute_PredictWithDesiredValue(
		FirstDerivativePredictor.GetObject(), FirstDerivativeValues, TArray<float>(), DesiredValue);

	return FirstDerivativePrediction;
}

float UAdditiveSecondDerivativePredictor::Predict_Implementation(const TArray<float>& Values,
                                                                 const TArray<float>& Predictions)
{
	if (!BasePredictor)
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: No Base Predictor is defined."), *GetNameSafe(this));
		return DefaultValue;
	}

	const float BasePrediction = IPredictor::Execute_Predict(BasePredictor.GetObject(), Values, Predictions);

	if (!FirstDerivativePredictor)
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: No First Derivative Predictor is defined."), *GetNameSafe(this));
		return BasePrediction;
	}

	if (Values.Num() < 2)
	{
		return BasePrediction;
	}

	TArray<float> FirstDerivativeValues;
	UAdfectusUtilsLibrary::Derivative(Values, FirstDerivativeValues);

	const float FirstDerivativePrediction = IPredictor::Execute_Predict(FirstDerivativePredictor.GetObject(),
	                                                                    FirstDerivativeValues, TArray<float>());

	if (!SecondDerivativePredictor)
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: No Second Derivative Predictor is defined."), *GetNameSafe(this));
		return BasePrediction + FirstDerivativePrediction;
	}

	if (FirstDerivativeValues.Num() < 2)
	{
		return BasePrediction + FirstDerivativePrediction;
	}

	TArray<float> SecondDerivativeValues;
	UAdfectusUtilsLibrary::Derivative(FirstDerivativeValues, SecondDerivativeValues);

	const float SecondDerivativePrediction = IPredictor::Execute_Predict(
		SecondDerivativePredictor.GetObject(), SecondDerivativeValues, TArray<float>());

	return BasePrediction + FirstDerivativePrediction + SecondDerivativePrediction;
}

float UAdditiveSecondDerivativePredictor::PredictWithDesiredValue_Implementation(const TArray<float>& Values,
	const TArray<float>& Predictions, const float DesiredValue)
{
	if (!BasePredictor)
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: No Base Predictor is defined."), *GetNameSafe(this));
		return DefaultValue;
	}

	const float BasePrediction = IPredictor::Execute_PredictWithDesiredValue(
		BasePredictor.GetObject(), Values, Predictions, DesiredValue);

	if (!FirstDerivativePredictor)
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: No First Derivative Predictor is defined."), *GetNameSafe(this));
		return BasePrediction;
	}

	if (Values.Num() < 2)
	{
		return BasePrediction;
	}

	TArray<float> FirstDerivativeValues;
	UAdfectusUtilsLibrary::Derivative(Values, FirstDerivativeValues);

	const float FirstDerivativePrediction = IPredictor::Execute_PredictWithDesiredValue(
		FirstDerivativePredictor.GetObject(), FirstDerivativeValues, TArray<float>(), DesiredValue);

	if (!SecondDerivativePredictor)
	{
		UE_LOG(Log3Motion, Warning, TEXT("%s: No Second Derivative Predictor is defined."), *GetNameSafe(this));
		return BasePrediction + FirstDerivativePrediction;
	}

	if (FirstDerivativeValues.Num() < 2)
	{
		return BasePrediction + FirstDerivativePrediction;
	}

	TArray<float> SecondDerivativeValues;
	UAdfectusUtilsLibrary::Derivative(FirstDerivativeValues, SecondDerivativeValues);

	const float SecondDerivativePrediction = IPredictor::Execute_PredictWithDesiredValue(
		SecondDerivativePredictor.GetObject(), SecondDerivativeValues, TArray<float>(), DesiredValue);

	return BasePrediction + FirstDerivativePrediction + SecondDerivativePrediction;
}
