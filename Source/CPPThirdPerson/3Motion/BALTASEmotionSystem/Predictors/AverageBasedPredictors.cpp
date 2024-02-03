// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.


#include "AverageBasedPredictors.h"

#include "CPPThirdPerson/Utils/AdfectusUtilsLibrary.h"

float UMovingAveragePredictor::Predict_Implementation(const TArray<float>& Values, const TArray<float>& Predictions)
{
	if (Values.IsEmpty())
	{
		return DefaultValue;
	}

	float Average = 0;
	const int Size = WindowSize > Values.Num() ? Values.Num() : WindowSize;
	for (int i = 0; i < Size; ++i)
	{
		Average += Values[Values.Num() - 1 - i] / Size;
	}

	return Average;
}

float UWeightedMovingAveragePredictor::Predict_Implementation(const TArray<float>& Values,
                                                              const TArray<float>& Predictions)
{
	if (Values.IsEmpty())
	{
		return DefaultValue;
	}

	float Average = 0;
	int Size = Weights.Num();
	float Extra = 0;
	if (Size > Values.Num())
	{
		Size = Values.Num();
		const float Diff = Weights.Num() - Values.Num();
		for (int i = 0; i < Diff; i++)
		{
			Extra += Weights[Weights.Num() - 1 - i];
		}

		Extra /= Weights.Num() - Diff;
	}

	for (int i = 0; i < Size; i++)
	{
		Average += Values[Values.Num() - 1 - i] * (Weights[i] + Extra);
	}

	return Average;
}

float UExponentialMovingAveragePredictor::Predict_Implementation(const TArray<float>& Values,
                                                                 const TArray<float>& Predictions)
{
	if (Values.IsEmpty())
	{
		return DefaultValue;
	}

	const float Value = Values[Values.Num() - 1];

	float LastPrediction;
	// if there are no previous predictions assume the same value
	if (Predictions.Num() == 0)
		LastPrediction = Value;
	else
		LastPrediction = Predictions[Predictions.Num() - 1];

	// Note that the LastPrediction holds the value of the moving average.
	// See more: https://www.investopedia.com/ask/answers/122314/what-exponential-moving-average-ema-formula-and-how-ema-calculated.asp

	const float k = Smoothing / (Values.Num() + 1);
	return (Value * k) + (LastPrediction * (1 - k));
}

float UMartinhoSimplePredictor::Predict_Implementation(const TArray<float>& Values, const TArray<float>& Predictions)
{
	if (Values.IsEmpty())
	{
		return DefaultValue;
	}

	const float Value = Values[Values.Num() - 1];

	float LastPrediction;
	// if there are no previous predictions assume the same value
	if (Predictions.Num() == 0)
		LastPrediction = Value;
	else
		LastPrediction = Predictions[Predictions.Num() - 1];

	const float a = UAdfectusUtilsLibrary::Exogenous(Value, LastPrediction) * ExogenousFactor;

	return LastPrediction * (1.0f - a) + Value * a;
}

float UMartinhoSimplePredictor::PredictWithDesiredValue_Implementation(const TArray<float>& Values,
                                                                       const TArray<float>& Predictions,
                                                                       const float DesiredValue)
{
	if (Values.IsEmpty())
	{
		return DefaultValue;
	}

	const float Value = Values[Values.Num() - 1];

	float LastPrediction;
	// if there are no previous predictions assume the same value
	if (Predictions.Num() == 0)
		LastPrediction = Value;
	else
		LastPrediction = Predictions[Predictions.Num() - 1];

	float a = UAdfectusUtilsLibrary::Exogenous(Value, LastPrediction) * ExogenousFactor;

	a += FMath::Pow(UAdfectusUtilsLibrary::Endogenous(Value, LastPrediction, DesiredValue), 2) * a;

	return LastPrediction * (1 - a) + Value * a;
}
