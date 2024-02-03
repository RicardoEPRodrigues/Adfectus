// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.


#include "Emotivector.h"

#include "TMBaltasModule.h"
#include "CPPThirdPerson/3Motion/TMLogging.h"
#include "CPPThirdPerson/Utils/AdfectusUtilsLibrary.h"
#include "Predictors/AverageBasedPredictors.h"
#include "Predictors/DerivativePredictors.h"
#include "Predictors/IPredictor.h"
#include "CPPThirdPerson/CPPThirdPersonCharacter.h"

UEmotivector::UEmotivector()
{
}

void UEmotivector::Initialize_Implementation(UTMBaltasModule* Module)
{
	OwnerModule = Module;
	Owner = Module->Owner;
	OwnerAgentComponent = Module->OwnerAgentComponent;

	if (!Predictor)
	{
		UExponentialMovingAveragePredictor* BasePredictor = NewObject<UExponentialMovingAveragePredictor>();
		UAdditiveSecondDerivativePredictor* MainPredictor = NewObject<UAdditiveSecondDerivativePredictor>();
		MainPredictor->BasePredictor = BasePredictor;
		MainPredictor->FirstDerivativePredictor = BasePredictor;
		MainPredictor->SecondDerivativePredictor = BasePredictor;
		Predictor = MainPredictor;
	}

	if (!MetaPredictor)
	{
		UExponentialMovingAveragePredictor* AveragePredictor = NewObject<UExponentialMovingAveragePredictor>();
		// AveragePredictor->WindowSize = 100;
		AveragePredictor->DefaultValue = 0.01f;
		MetaPredictor = AveragePredictor;
	}
}

void UEmotivector::Reset_Implementation()
{
	Values.Empty();
	Predictions.Empty();
	PredictionErrors.Empty();
	MetaPredictions.Empty();
	SalientSensations.Empty();
}


void UEmotivector::AddValue(float Value)
{
	if (!(Cast<IPredictor>(Predictor) ||
			(Predictor && Predictor->GetClass()->ImplementsInterface(UPredictor::StaticClass())))
		|| !(Cast<IPredictor>(MetaPredictor) ||
			(MetaPredictor && MetaPredictor->GetClass()->ImplementsInterface(UPredictor::StaticClass())))
	)
	{
		UE_LOG(Log3Motion, Warning, TEXT("Predictors are not well defined."))
		return;
	}

	// Check how new value compares with predictions.
	if (Values.Num() > 0 && Predictions.Num() > 0)
	{
		const float Prediction = Predictions.Last();
		const float PreviousValue = Values[Values.Num() - 1];

		const float ExpectedReward = Prediction - PreviousValue;
		const float SensedReward = Value - PreviousValue;

		const float PredictionError = FMath::Abs(ExpectedReward - SensedReward);

		PredictionErrors.Add(PredictionError);

		float Tolerance = PredictionError;
		if (!MetaPredictions.IsEmpty())
		{
			Tolerance = MetaPredictions.Last();
		}

		// UE_LOG(Log3Motion, Log, TEXT("%s - %s -> Value: %f | Prediction: %f | PredictionError: %f | Tolerance: %f"),
		//        *GetNameSafe(this->Owner), *GetNameSafe(this), Value, Prediction, PredictionError, Tolerance);
		// UE_LOG(Log3Motion, Log, TEXT("%s - %s -> ExpectedReward: %f | SensedReward: %f"), *GetNameSafe(this->Owner),
		//        *GetNameSafe(this), ExpectedReward, SensedReward);

		if (PredictionError > Tolerance)
		{
			// if ExpectedReward < 0 then we expect punishment
			// if ExpectedReward >= 0 then we expect reward
			// Sensation->Valence = ExpectedReward < 0 ? ESensationValence::Punishment : ESensationValence::Reward;

			// if SensedReward < 0 then we sensed punishment
			// if SensedReward >= 0 then we sensed reward
			// Sensation->Type = SensedReward < ExpectedReward
			// 	                  ? ESensationType::WorseThanExpected
			// 	                  : ESensationType::BetterThanExpected;

			// NOTE: The commented code below takes into account the AsExpected value and allows a Sensation to be created from it.
			// if (PredictionError < Tolerance)
			// {
			// 	Sensation->Type = ESensationType::AsExpected;
			// }
			// else
			// {
			// 	// if SensedReward < 0 then we sensed punishment
			// 	// if SensedReward >= 0 then we sensed reward
			// 	Sensation->Type = SensedReward < ExpectedReward
			// 						  ? ESensationType::WorseThanExpected
			// 						  : ESensationType::BetterThanExpected;
			// }

			float Salience = UAdfectusUtilsLibrary::Exogenous(Value, Prediction);
			if (UseDesiredValue)
			{
				Salience += UAdfectusUtilsLibrary::Endogenous(Value, Prediction, DesiredValue);
			}

			ESensationValence Valence = ExpectedReward < 0 ? ESensationValence::Punishment : ESensationValence::Reward;
			if (ExpectedReward == 0)
			{
				Valence = SensedReward < 0 ? ESensationValence::Punishment : ESensationValence::Reward;
			}
			const ESensationType Type = SensedReward < ExpectedReward
				                            ? ESensationType::WorseThanExpected
				                            : ESensationType::BetterThanExpected;
			USensation* Sensation = MakeSensation(Valence, Type, Salience);

			// UE_LOG(Log3Motion, Log, TEXT("%s - %s -> Sensation: %s %s | Salience: %f"), *GetNameSafe(this->Owner),
			//        *GetNameSafe(this), *UEnum::GetValueAsString(Valence), *UEnum::GetValueAsString(Type), Salience);
			SalientSensations.Add(Sensation);
		}

		MetaPredictions.Add(IPredictor::Execute_Predict(MetaPredictor, PredictionErrors, MetaPredictions));
	}

	Values.Add(ClampValues ? FMath::Clamp(Value, ClampInterval.Min, ClampInterval.Max) : Value);
	float PredictionValue;
	if (UseDesiredValue)
	{
		PredictionValue = IPredictor::Execute_PredictWithDesiredValue(Predictor, Values, Predictions,
		                                                              DesiredValue);
	}
	else
	{
		PredictionValue = IPredictor::Execute_Predict(Predictor, Values, Predictions);
	}
	Predictions.Add(ClampValues
		                ? FMath::Clamp(PredictionValue, ClampInterval.Min, ClampInterval.Max)
		                : PredictionValue);
}

TArray<USensation*> UEmotivector::Update()
{
	OnUpdate();

	TArray<USensation*> Sensations(SalientSensations);
	SalientSensations.Empty();
	return Sensations;
}

void UEmotivector::OnUpdate_Implementation()
{
}

USensation* UEmotivector::MakeSensation(const ESensationValence Valence, const ESensationType Type,
                                        const float Salience, const float Lifetime, const float SensationImportance)
{
	USensation* Sensation = NewObject<USensation>(this);
	Sensation->Emotivector = this;

	Sensation->Valence = Valence;

	Sensation->Type = Type;

	Sensation->InitialSalience = Salience;
	Sensation->Salience = Sensation->InitialSalience;

	Sensation->InitialLifetime = Lifetime;
	Sensation->Lifetime = Sensation->InitialLifetime;

	Sensation->Importance = SensationImportance;

	return Sensation;
}
