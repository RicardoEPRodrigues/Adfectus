#include "TMWinLoseRatioModule.h"

#include "TMCharacterPerception.h"
#include "../TMLogging.h"
#include "../ThreeMotionAgentComponent.h"
#include "CPPThirdPerson/CPPThirdPersonCharacter.h"
#include "CPPThirdPerson/3Motion/TMTheoryOfMind.h"
#include "Kismet/GameplayStatics.h"
#include "CPPThirdPerson/3Motion/Personality.h"

#define VERBOSE_DEBUG_LOG
#define PREDICTION_WINDOW 15

UTMWinLoseRatioModule::UTMWinLoseRatioModule()
{
	Personality = LoadObject<UPersonality>(
		nullptr, TEXT(
			"Personality'/Game/Adfectus/Blueprints/Characters/Personalities/FighterPersonality_Default.FighterPersonality_Default'"),
		nullptr, LOAD_None, nullptr);
}

TArray<float> Differentiate(TArray<float> Values)
{
	TArray<float> Derivatives;

	for (int i = 1; i < Values.Num(); ++i)
	{
		Derivatives.Push(Values[i] - Values[i - 1]);
	}

	return Derivatives;
}

float MovingAverage(TArray<float> Values, int Window, const float Default)
{
	// This can be improved to 3 operations after the first moving average
	const int Size = Values.Num();

	if (Size == 0 || Window == 0) return Default;

	const float Diff = FMath::Max(0, Window - Size);

	const float NewWindow = Diff > 0 ? Size : Window;

	const float Weight = 1.0f / NewWindow;

	// If there are enough elements calculate moving average
	//float Total = (Diff * Default) / Window;
	//float Total = (Diff * Default) / Window;
	float Total = 0.0f;
	for (int i = Size - 1; i >= Size - NewWindow; --i)
	{
		Total += Values[i] * Weight;
	}

	return Total;
}

float WeightedMovingAverage(TArray<float> Values, int Window, const float Default)
{
	const int Size = Values.Num();

	if (Size == 0 || Window == 0) return Default;

	Window = Size < Window ? Size : Window;

	// Weight formula from weighted moving average: https://en.wikipedia.org/wiki/Moving_average
	const float Weight = 1.0f / ((Window * (Window + 1.0f)) * .5f);

	float Total = 0.0f;
	const float Diff = FMath::Max(.0f, Size - Window);

	// const float Denominator = (Window * (Window + 1.0f)) * .5f;

	FString s;
	for (int i = Size - 1; i >= Size - Window; --i)
	{
		const float Numerator = (static_cast<float>(i) + 1.0f - Diff);
		const float V = Values[i] * Numerator * Weight;
		// UE_LOG(LogTemp, Warning, TEXT("numerator - %f, denominator - %f, value - %f"), Numerator, Denominator, V);
		Total += V;
	}


	return Total;
}

float CalculateHitBoost(TArray<float> WinLoseRatioDerivative, const float MaxHits)
{
	const int Size = WinLoseRatioDerivative.Num();

	if (Size == 0 || MaxHits == 0) return 0.0;

	float HitBoost = 0.0f;
	for (int i = 0; i < Size; ++i)
	{
		HitBoost += FMath::Sign(WinLoseRatioDerivative[i]);
	}

	return (FMath::Clamp(HitBoost, -MaxHits, MaxHits) / MaxHits) * 1.0;
}

float UTMWinLoseRatioModule::BaseWinLoseRatio(float ANormalizedHealth, float BNormalizedHealth) const
{
	return FMath::Lerp(
		FMath::Clamp((ANormalizedHealth - BNormalizedHealth) * 0.5f + 0.5f, 0.0f, 1.0f),
		1.0f,
		1.0f - FMath::CeilToFloat(BNormalizedHealth));
}

TArray<float> UTMWinLoseRatioModule::GetWinLoseRatioOutcomesDerivativeForStage(
	ETMActionStage ActionStage, const ACPPThirdPersonCharacter* Actor) const
{
	TArray<float> ActionOutcomes = TArray<float>();

	// Searching for the outcomes that happened to a particular action stage associated with a particular actor
	// (Assuming that the only   actions that are perceived are attack actions)
	// Since we are finding the derivative we start at one (since it cannot be done if there are less than 2 elements)
	for (int i = 1; i < WinLoseRatioDataHistory.Num() - 1; ++i)
	{
		const FWinLoseRatioData WinLoseRatioData = WinLoseRatioDataHistory[i - 1];

		if (WinLoseRatioData.Actor != Actor || WinLoseRatioData.Action->Stage != ActionStage) continue;

		// Searching for the latest existing action stage that belongs to the current action
		// to find a possible outcome
		int end_i = -1;
		do
		{
			const FWinLoseRatioData NextWinLoseRatioData = WinLoseRatioDataHistory[i];

			if (NextWinLoseRatioData.Actor != Actor)
			{
				++i;
				continue;
			}

			if (NextWinLoseRatioData.Action->Stage > ActionStage)
			{
				end_i = i;
				++i;
				continue;
			}

			break;
		}
		while (i < WinLoseRatioDataHistory.Num() - 1);

		if (end_i != -1)
		{
			ActionOutcomes.Push(WinLoseRatioDataHistory[end_i].WinLoseRatio - WinLoseRatioData.WinLoseRatio);
			// if the loop above stopped because we reached a new action (i.e. one where the action stage is lower than the desired)
			// then we have to make sure we stay in the current index by decrementing it before it is incremented again
			--i;
		}
	}

	return ActionOutcomes;
}

TArray<float> UTMWinLoseRatioModule::GetWinLoseRatioOutcomesForStage(ETMActionStage ActionStage,
                                                                     const ACPPThirdPersonCharacter* Actor) const
{
	TArray<float> ActionOutcomes = TArray<float>();

	if (WinLoseRatioDataHistory.IsEmpty()) return ActionOutcomes;

	ActionOutcomes.Push(0.5);

	// Searching for the outcomes that happened to a particular action stage associated with a particular actor
	// (Assuming that the only actions that are perceived are attack actions)
	// Since we are finding the derivative we start at one (since it cannot be done if there are less than 2 elements)
	for (int i = 1; i < WinLoseRatioDataHistory.Num() - 1; ++i)
	{
		const FWinLoseRatioData WinLoseRatioData = WinLoseRatioDataHistory[i - 1];

		if (WinLoseRatioData.Actor != Actor || WinLoseRatioData.Action->Stage != ActionStage) continue;

		// Searching for the latest existing action stage that belongs to the current action
		// to find a possible outcome
		int end_i = -1;
		do
		{
			const FWinLoseRatioData NextWinLoseRatioData = WinLoseRatioDataHistory[i];

			if (NextWinLoseRatioData.Actor != Actor)
			{
				++i;
				continue;
			}

			if (NextWinLoseRatioData.Action->Stage > ActionStage)
			{
				end_i = i;
				++i;
				continue;
			}

			break;
		}
		while (i < WinLoseRatioDataHistory.Num() - 1);

		if (end_i != -1)
		{
			ActionOutcomes.Push(WinLoseRatioDataHistory[end_i].WinLoseRatio);
			// if the loop above stopped because we reached a new action (i.e. one where the action stage is lower than the desired)
			// then we have to make sure we stay in the current index by decrementing it before it is incremented again
			--i;
		}
	}

	return ActionOutcomes;
}

void UTMWinLoseRatioModule::SetWinLoseRatioType(const EWinLoseRatioType NewWinLoseRatioType)
{
	if (CalculateWinLoseRatio.IsBound() && NewWinLoseRatioType == WinLoseRatioType)
	{
		return;
	}
	
	switch (NewWinLoseRatioType)
	{
	case EWinLoseRatioType::ReactiveWinLoseRatio:
		CalculateWinLoseRatio.BindUFunction(this, "ReactiveWinLoseRatio");
		break;

	case EWinLoseRatioType::PredictedWinLoseRatioPerStage:
		CalculateWinLoseRatio.BindUFunction(this, "PredictedWinLoseRatio");
		break;

	case EWinLoseRatioType::PredictedWinLoseRatioPerStageWithAssertion:
		CalculateWinLoseRatio.BindUFunction(this, "PredictedWinLoseRatioWithAssertion");
		break;

	default: CalculateWinLoseRatio.BindLambda([]
		{
		});
	}

	// UE_LOG(Log3Motion, Log, TEXT("%s: Selected %s"), *GetNameSafe(this), *UEnum::GetValueAsString(NewWinLoseRatioType))

	WinLoseRatioType = NewWinLoseRatioType;
}

EWinLoseRatioType UTMWinLoseRatioModule::GetWinLoseRatioType() const { return WinLoseRatioType; }

void UTMWinLoseRatioModule::Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent)
{
	OwnerAgentComponent = OwnerComponent;
	if (!OwnerAgentComponent)
	{
		UE_LOG(Log3Motion, Warning, TEXT("Owner Agent Component is Null!"))
		return;
	}

	Owner = OwnerAgentComponent->GetOwner<ACPPThirdPersonCharacter>();
	if (!Owner)
	{
		UE_LOG(Log3Motion, Warning, TEXT("Owner is not a CPPThirdPersonCharacter!"))
		return;
	}

	SetWinLoseRatioType(WinLoseRatioType);

	WinLoseRatioHistory.Empty();

	WinLoseRatio = StartWinLoseRatio;
	WinLoseRatioHistory.Add(0.5);
}

void UTMWinLoseRatioModule::Execute_Implementation()
{
	CalculateWinLoseRatio.Execute();

	if (Personality)
	{
		for (UTMEmotion* AvailableEmotion : OwnerAgentComponent->TheoryOfMind->AvailableEmotions)
		{
			if (Personality->IsTraitActive(AvailableEmotion, WinLoseRatio))
			{
				// This is not copying the emotion, which can create issues.
				// Use UTMotionUtils::DuplicateEmotion if you want copies.
				OwnerAgentComponent->TheoryOfMind->DesiredEmotions.AddUnique(AvailableEmotion);
			}
			else
			{
				OwnerAgentComponent->TheoryOfMind->DesiredEmotions.Remove(AvailableEmotion);
			}
		}
	}


	GEngine->AddOnScreenDebugMessage(1, 0, FColor::Cyan,
	                                 FString::Printf(TEXT("Predicted WinLoseRatio: %f"), WinLoseRatio), false);

	GEngine->AddOnScreenDebugMessage(2, 0, FColor::Cyan,
	                                 FString::Printf(
		                                 TEXT("Base WinLoseRatio: %f"),
		                                 WinLoseRatioHistory.IsEmpty() ? 0.5f : WinLoseRatioHistory.Last()), false);

	GEngine->AddOnScreenDebugMessage(3, 0, FColor::Cyan,
	                                 FString::Printf(TEXT("Total history: %d"), WinLoseRatioDataHistory.Num()), false);

	GEngine->AddOnScreenDebugMessage(4, 0, FColor::Cyan, FString::Printf(TEXT("Moving Average: %f"), MvAverage), false);

	GEngine->AddOnScreenDebugMessage(5, 0, FColor::Cyan, FString::Printf(TEXT("Health offset: %f"), HealthOffset),
	                                 false);

	GEngine->AddOnScreenDebugMessage(6, 0, FColor::Cyan, FString::Printf(TEXT("First derivative: %f"), FirstDerivative),
	                                 false);

	GEngine->AddOnScreenDebugMessage(7, 0, FColor::Cyan,
	                                 FString::Printf(TEXT("Second derivative: %f"), SecondDerivative), false);

	GEngine->AddOnScreenDebugMessage(8, 0, FColor::Cyan, FString::Printf(TEXT("Hit Boost: %f"), WinLoseRatioBoost),
	                                 false);
}

void UTMWinLoseRatioModule::EndPlay_Implementation()
{
	// UE_LOG(LogTemp, Warning, TEXT("EndPlay has been called for UTMWinLoseRatioModule"));

	WinLoseRatio = StartWinLoseRatio;
	MvAverage = WinLoseRatio;
	FirstDerivative = 0.0f;
	SecondDerivative = 0.0f;
	HealthOffset = 0.0f;

	LastPercept = nullptr;
	LastPercepts.Empty();

	WinLoseRatioHistory.Empty();
	WinLoseRatioHistory.Add(0.5);
	FirstDerivatives.Empty();
	SecondDerivatives.Empty();
	WinLoseRatioDataHistory.Empty();

	CurrentWinLoseRatio = WinLoseRatio;
	WinLoseRatioBoostTimer = 0.0f;
	WinLoseRatioBoost = 0.0f;
	PredictionCharacter = nullptr;
}

void UTMWinLoseRatioModule::ReactiveWinLoseRatio()
{
	const auto PerceptHistory = OwnerAgentComponent->TheoryOfMind->PerceptHistory;

	if (!OwnerAgentComponent->TheoryOfMind->HasNewPercepts()) return;

	UTMPercept* MostRecentPercept = PerceptHistory.Last();

	if (LastPercept && *MostRecentPercept == *LastPercept) return;

	LastPercept = MostRecentPercept;

	if (const ACPPThirdPersonCharacter* Character = Cast<ACPPThirdPersonCharacter>(MostRecentPercept->Actor))
	{
		float OpponentNormalizedHealth = 1.0;
		float NormalizedHealth = 1.0;

		if (Character != Owner)
		{
			OpponentNormalizedHealth = Character->GetNormalizedHealth();
			NormalizedHealth = Owner->GetNormalizedHealth();
		}
		else
		{
			TArray<AActor*> Actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPPThirdPersonCharacter::StaticClass(), Actors);

			for (AActor* Actor : Actors)
			{
				if (const ACPPThirdPersonCharacter* CPPCharacter = Cast<ACPPThirdPersonCharacter>(Actor);
					CPPCharacter && CPPCharacter != Owner)
				{
					OpponentNormalizedHealth = CPPCharacter->GetNormalizedHealth();
					NormalizedHealth = Owner->GetNormalizedHealth();

					break;
				}
			}
		}

		WinLoseRatio = FMath::Lerp(
			FMath::Clamp((NormalizedHealth - OpponentNormalizedHealth) * 0.5f + 0.5f, 0.0f, 1.0f),
			1.0f,
			1.0f - FMath::CeilToFloat(OpponentNormalizedHealth));
	}
	else
	{
		UE_LOG(Log3Motion, Warning, TEXT("Failed to cast Opponent to ACPPThirdPersonCharacter!"))
	}
}

void UTMWinLoseRatioModule::PredictedWinLoseRatio()
{
	const auto PerceptHistory = OwnerAgentComponent->TheoryOfMind->PerceptHistory;

	if (!OwnerAgentComponent->TheoryOfMind->HasNewPercepts()) return;

	// Find the most recent percepts (mine and from the opponent)
	TArray<UTMPercept*> MostRecentPercepts = TArray<UTMPercept*>();

	if (LastPercepts.IsEmpty())
	{
		for (int i = PerceptHistory.Num() - 1; i >= 0; --i)
		{
			UTMPercept* Percept = PerceptHistory[i];

			if (Cast<ACPPThirdPersonCharacter>(Percept->Actor))
			{
				MostRecentPercepts.Push(Percept);
				// UE_LOG(LogTemp, Display, TEXT("Received new Percept!"));
			}
		}
	}
	else
	{
		// For each new percept check if it is not the same as one of the previous ones
		const UTMPercept* Latest = LastPercepts[0];
		for (int i = PerceptHistory.Num() - 1; i >= 0; --i)
		{
			UTMPercept* Percept = PerceptHistory[i];

			if (Percept->Time <= Latest->Time)
			{
				break;
			}

			if (Cast<ACPPThirdPersonCharacter>(Percept->Actor))
			{
				MostRecentPercepts.Push(Percept);
				// UE_LOG(LogTemp, Display, TEXT("Received new Percept!"));
			}
		}
	}

	// If there are no new percepts then ignore otherwise update last percepts
	if (MostRecentPercepts.IsEmpty()) return;
	LastPercepts = MostRecentPercepts;

	// UE_LOG(LogTemp, Display, TEXT("Received %d new percepts"), LastPercepts.Num());

	// For each new percept predict the WinLoseRatio
	TArray<float> PredictedWinLoseRatios = TArray<float>();
	TArray<const ACPPThirdPersonCharacter*> PredictionCharacters = TArray<const ACPPThirdPersonCharacter*>();
	TArray<float> BaseWinLoseRatios = TArray<float>();

	for (const UTMPercept* Percept : LastPercepts)
	{
		const ACPPThirdPersonCharacter* Character = Cast<ACPPThirdPersonCharacter>(Percept->Actor);
		const UTMCharacterPerceptData* CharacterPerceptData = Cast<UTMCharacterPerceptData>(Percept->Data);

		if (!CharacterPerceptData || !CharacterPerceptData->Action)
		{
			continue;
		}

		// UE_LOG(LogTemp, Display, TEXT("%s - %s"), *CharacterPerceptData->Action->Name,
		//        *UEnum::GetValueAsString(CharacterPerceptData->Action->Stage));

		// Start by calculating the Base WinLoseRatio
		float OpponentNormalizedHealth = 1.0f;
		float NormalizedHealth = 1.0f;

		// Only predict the WinLoseRatio when the opponent is in either anticipation stages
		const bool PredictWinLoseRatio =
			CharacterPerceptData->Action->Stage == ETMActionStage::Anticipation_Interruptible ||
			CharacterPerceptData->Action->Stage == ETMActionStage::Anticipation_Uninterruptible;

		// Here the assertion only serves to clear the perception character
		// so that a new prediction can be done
		// Only assert the prediction if the percept character is the same as the prediction
		const bool AssertPrediction = Character == this->PredictionCharacter && (CharacterPerceptData->Action->Stage
			== ETMActionStage::Follow_Through_Uninterruptible || CharacterPerceptData->Action->Stage ==
			ETMActionStage::Cancel);

		if (Character != Owner) // Is percept from opponent?
		{
			// UE_LOG(LogTemp, Display, TEXT("Percept is from opponent!"));

			OpponentNormalizedHealth = CharacterPerceptData->NormalizedHealth;
			NormalizedHealth = Owner->GetNormalizedHealth();
		}
		else // The percept is from self (assuming there are only ever two fighters)
		{
			// UE_LOG(LogTemp, Display, TEXT("Percept is from self!"));

			TArray<AActor*> Actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPPThirdPersonCharacter::StaticClass(), Actors);

			for (AActor* Actor : Actors)
			{
				if (const ACPPThirdPersonCharacter* CPPCharacter = Cast<ACPPThirdPersonCharacter>(Actor);
					CPPCharacter && CPPCharacter != Owner)
				{
					OpponentNormalizedHealth = CPPCharacter->GetNormalizedHealth();
					NormalizedHealth = CharacterPerceptData->NormalizedHealth;

					break;
				}
			}
		}

		const float BaseWinLoseRatio = UTMWinLoseRatioModule::BaseWinLoseRatio(
			NormalizedHealth,
			OpponentNormalizedHealth);

		if (PredictWinLoseRatio)
		{
			// Find the WinLoseRatio Outcomes for this particular stage (only considering attacks here)
			TArray<float> ActionOutComes = GetWinLoseRatioOutcomesForStage(
				CharacterPerceptData->Action->Stage, Character);

			if (ActionOutComes.Num() > 1)
			{
				const TArray<float> FirstDer = Differentiate(ActionOutComes);
				const TArray<float> SecondDer = Differentiate(FirstDer);

				// #ifdef VERBOSE_DEBUG_LOG
				// 				FString s = "Predicted outcome for stage:" + UEnum::GetValueAsString(
				// 					CharacterPerceptData->Action->Stage) + " - ";
				// 				for (int i = 0; i < FirstDer.Num(); ++i)
				// 				{
				// 					s += FString::SanitizeFloat(FirstDer[i]) + ", ";
				// 				}
				// 				UE_LOG(LogTemp, Display, TEXT("%s"), *s);
				// #endif

				this->MvAverage = WeightedMovingAverage(ActionOutComes, PREDICTION_WINDOW, 0.5f);
				this->FirstDerivative = WeightedMovingAverage(FirstDer, PREDICTION_WINDOW, 0.0f);
				this->SecondDerivative = WeightedMovingAverage(SecondDer, PREDICTION_WINDOW, 0.0f);

				float PredictedWinLoseRatio = FMath::Clamp(
					this->MvAverage + this->FirstDerivative + this->SecondDerivative, 0.0f, 1.0f);

				PredictedWinLoseRatios.Push(PredictedWinLoseRatio);
				PredictionCharacters.Push(Character);
				BaseWinLoseRatios.Push(BaseWinLoseRatio);

				const float NewPredictionDiff = PredictedWinLoseRatio - BaseWinLoseRatio;
				const float CurrentPredictionDiff = this->CurrentWinLoseRatio - BaseWinLoseRatio;

				// Only change prediction if there is no existing prediction or if the new prediction causes
				// a larger impact on the WinLoseRatio
				if (this->PredictionCharacter == nullptr || FMath::Abs(NewPredictionDiff) > FMath::Abs(
					CurrentPredictionDiff))
				{
					PredictedWinLoseRatios.Push(PredictedWinLoseRatio);
					PredictionCharacters.Push(Character);
					BaseWinLoseRatios.Push(BaseWinLoseRatio);
				}
			}
		}
		else if (AssertPrediction)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Asserted a prediction!"));
			PredictionCharacter = nullptr;
		}

		FWinLoseRatioData WinLoseRatioData;
		{
			WinLoseRatioData.Action = CharacterPerceptData->Action;
			WinLoseRatioData.Actor = Character;
			WinLoseRatioData.WinLoseRatio = BaseWinLoseRatio;
		}
		WinLoseRatioDataHistory.Push(WinLoseRatioData);
	}

	if (PredictedWinLoseRatios.IsEmpty()) return;

	// Now decide which prediction we should take into consideration (ours or the opponent)
	// Selected strategy: The one that causes a larger change in the win-lose ratio
	int SelectedIdx = 0.0f;
	float LargestDiff = -1.0f;

	for (int i = 0; i < PredictedWinLoseRatios.Num(); ++i)
	{
		if (const float Diff = FMath::Abs(PredictedWinLoseRatios[i] - CurrentWinLoseRatio); Diff > LargestDiff)
		{
			LargestDiff = Diff;
			SelectedIdx = i;
		}
	}

	// How should we incorporate the previous WinLoseRatio into the new one?
	this->CurrentWinLoseRatio = PredictedWinLoseRatios[SelectedIdx];
	this->PredictionCharacter = PredictionCharacters[SelectedIdx];
	this->WinLoseRatio = CurrentWinLoseRatio;
	this->WinLoseRatioHistory.Push(BaseWinLoseRatios[SelectedIdx]);
}

void UTMWinLoseRatioModule::PredictedWinLoseRatioWithAssertion()
{
	const auto PerceptHistory = OwnerAgentComponent->TheoryOfMind->PerceptHistory;

	// Increase boost overtime until reaching MaxTimer, only when there is actually a win-lose ratio boost available
	if (WinLoseRatioBoostTimer > 0.0)
	{
		this->WinLoseRatioBoostTimer -= GetWorld()->GetDeltaSeconds();
		this->WinLoseRatioBoostTimer = FMath::Max(0.0, this->WinLoseRatioBoostTimer);
		this->WinLoseRatio = CurrentWinLoseRatio + WinLoseRatioBoost * (WinLoseRatioBoostTimer /
			WinLoseRatioBoostMaxTimer);
		this->WinLoseRatio = FMath::Clamp(WinLoseRatio, 0.0f, 1.0f);
	}

	if (!OwnerAgentComponent->TheoryOfMind->HasNewPercepts()) return;

	// Find the most recent percepts (mine and from the opponent)
	TArray<UTMPercept*> MostRecentPercepts = TArray<UTMPercept*>();

	if (LastPercepts.IsEmpty())
	{
		for (int i = PerceptHistory.Num() - 1; i >= 0; --i)
		{
			UTMPercept* Percept = PerceptHistory[i];

			if (Cast<ACPPThirdPersonCharacter>(Percept->Actor))
			{
				MostRecentPercepts.Push(Percept);
				// UE_LOG(LogTemp, Display, TEXT("Received new Percept!"));
			}
		}
	}
	else
	{
		// For each new percept check if it is not the same as one of the previous ones
		const UTMPercept* Latest = LastPercepts[0];
		for (int i = PerceptHistory.Num() - 1; i >= 0; --i)
		{
			UTMPercept* Percept = PerceptHistory[i];

			if (Percept->Time <= Latest->Time)
			{
				break;
			}

			if (Cast<ACPPThirdPersonCharacter>(Percept->Actor))
			{
				MostRecentPercepts.Push(Percept);
				// UE_LOG(LogTemp, Display, TEXT("Received new Percept!"));
			}
		}
	}

	// If there are no new percepts then ignore otherwise update last percepts
	if (MostRecentPercepts.IsEmpty()) return;
	LastPercepts = MostRecentPercepts;

	// UE_LOG(LogTemp, Display, TEXT("Received %d new percepts"), LastPercepts.Num());

	// For each new percept predict the WinLoseRatio
	TArray<float> PredictedWinLoseRatios = TArray<float>();
	TArray<const ACPPThirdPersonCharacter*> PredictionCharacters = TArray<const ACPPThirdPersonCharacter*>();
	TArray<float> BaseWinLoseRatios = TArray<float>();

	for (const UTMPercept* Percept : LastPercepts)
	{
		const ACPPThirdPersonCharacter* Character = Cast<ACPPThirdPersonCharacter>(Percept->Actor);
		const UTMCharacterPerceptData* CharacterPerceptData = Cast<UTMCharacterPerceptData>(Percept->Data);

		if (!CharacterPerceptData || !CharacterPerceptData->Action)
		{
			continue;
		}

		// UE_LOG(LogTemp, Display, TEXT("%s - %s"), *CharacterPerceptData->Action->Name,
		//        *UEnum::GetValueAsString(CharacterPerceptData->Action->Stage));

		// Start by calculating the Base WinLoseRatio
		float OpponentNormalizedHealth = 1.0f;
		float NormalizedHealth = 1.0f;

		// Only predict the WinLoseRatio when the opponent is in either anticipation stages
		const bool PredictWinLoseRatio =
			CharacterPerceptData->Action->Stage == ETMActionStage::Anticipation_Interruptible ||
			CharacterPerceptData->Action->Stage == ETMActionStage::Anticipation_Uninterruptible;

		// Only assert the prediction if the percept character is the same as the prediction
		const bool AssertPrediction = Character == this->PredictionCharacter &&
		(CharacterPerceptData->Action->Stage == ETMActionStage::Follow_Through_Uninterruptible
			|| CharacterPerceptData->Action->Stage == ETMActionStage::Cancel);

		if (Character != Owner) // Is percept from opponent?
		{
			// UE_LOG(LogTemp, Display, TEXT("Percept is from opponent!"));

			OpponentNormalizedHealth = CharacterPerceptData->NormalizedHealth;
			NormalizedHealth = Owner->GetNormalizedHealth();
		}
		else // The percept is from self (assuming there are only ever two fighters)
		{
			// UE_LOG(LogTemp, Display, TEXT("Percept is from self!"));

			TArray<AActor*> Actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPPThirdPersonCharacter::StaticClass(), Actors);

			for (AActor* Actor : Actors)
			{
				if (const ACPPThirdPersonCharacter* CPPCharacter = Cast<ACPPThirdPersonCharacter>(Actor);
					CPPCharacter && CPPCharacter != Owner)
				{
					OpponentNormalizedHealth = CPPCharacter->GetNormalizedHealth();
					NormalizedHealth = CharacterPerceptData->NormalizedHealth;

					break;
				}
			}
		}

		const float BaseWinLoseRatio = UTMWinLoseRatioModule::BaseWinLoseRatio(
			NormalizedHealth,
			OpponentNormalizedHealth);

		if (PredictWinLoseRatio)
		{
			// Find the WinLoseRatio Outcomes for this particular stage (only considering attacks here)
			TArray<float> ActionOutComes = GetWinLoseRatioOutcomesForStage(
				CharacterPerceptData->Action->Stage, Character);

			if (ActionOutComes.Num() > 1)
			{
				const TArray<float> FirstDer = Differentiate(ActionOutComes);
				const TArray<float> SecondDer = Differentiate(FirstDer);

				// #ifdef VERBOSE_DEBUG_LOG
				// 				FString s = "Predicted outcome for stage:" + UEnum::GetValueAsString(
				// 					CharacterPerceptData->Action->Stage) + " - ";
				// 				for (int i = 0; i < FirstDer.Num(); ++i)
				// 				{
				// 					s += FString::SanitizeFloat(FirstDer[i]) + ", ";
				// 				}
				// 				UE_LOG(LogTemp, Display, TEXT("%s"), *s);
				// #endif

				this->MvAverage = WeightedMovingAverage(ActionOutComes, PREDICTION_WINDOW, 0.5f);
				this->FirstDerivative = WeightedMovingAverage(FirstDer, PREDICTION_WINDOW, 0.0f);
				this->SecondDerivative = WeightedMovingAverage(SecondDer, PREDICTION_WINDOW, 0.0f);

				float PredictedWinLoseRatio = FMath::Clamp(
					this->MvAverage + this->FirstDerivative + this->SecondDerivative, 0.0f, 1.0f);

				PredictedWinLoseRatios.Push(PredictedWinLoseRatio);
				PredictionCharacters.Push(Character);
				BaseWinLoseRatios.Push(BaseWinLoseRatio);

				const float NewPredictionDiff = PredictedWinLoseRatio - BaseWinLoseRatio;
				const float CurrentPredictionDiff = this->CurrentWinLoseRatio - BaseWinLoseRatio;

				// Only change prediction if there is no existing prediction or if the new prediction causes
				// a larger impact on the WinLoseRatio
				if (this->PredictionCharacter == nullptr || FMath::Abs(NewPredictionDiff) > FMath::Abs(
					CurrentPredictionDiff))
				{
					PredictedWinLoseRatios.Push(PredictedWinLoseRatio);
					PredictionCharacters.Push(Character);
					BaseWinLoseRatios.Push(BaseWinLoseRatio);
				}
			}
		}
		else if (AssertPrediction)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Asserted a prediction!"));

			const float LastWinLoseRatio = this->WinLoseRatioHistory.Last();
			const float Derivative = BaseWinLoseRatio - LastWinLoseRatio;
			const float DerivativePrediction = this->CurrentWinLoseRatio - LastWinLoseRatio;

			if (FMath::Abs(Derivative) != FMath::Abs(DerivativePrediction) && FMath::Abs(
				BaseWinLoseRatio - this->CurrentWinLoseRatio) > 0.05f)
			{
				this->WinLoseRatioBoostTimer = WinLoseRatioBoostMaxTimer;
				this->WinLoseRatioBoost = BaseWinLoseRatio - this->WinLoseRatio;
			}

			PredictionCharacter = nullptr;
		}

		FWinLoseRatioData WinLoseRatioData;
		{
			WinLoseRatioData.Action = CharacterPerceptData->Action;
			WinLoseRatioData.Actor = Character;
			WinLoseRatioData.WinLoseRatio = BaseWinLoseRatio;
		}
		WinLoseRatioDataHistory.Push(WinLoseRatioData);
	}

	if (PredictedWinLoseRatios.IsEmpty()) return;

	// Now decide which prediction we should take into consideration (ours or the opponent)
	// Selected strategy: The one that causes a larger change in the win-lose ratio
	// Alternative strategy: Combine both, for example using linear interpolation or by adding their
	// difference to the win-lose ratio, effectively assuming both actions will result in the predicted outcome.
	int SelectedIdx = 0.0f;
	float LargestDiff = -1.0f;

	for (int i = 0; i < PredictedWinLoseRatios.Num(); ++i)
	{
		if (const float Diff = FMath::Abs(PredictedWinLoseRatios[i] - CurrentWinLoseRatio); Diff > LargestDiff)
		{
			LargestDiff = Diff;
			SelectedIdx = i;
		}
	}

	this->CurrentWinLoseRatio = PredictedWinLoseRatios[SelectedIdx];
	this->PredictionCharacter = PredictionCharacters[SelectedIdx];
	this->WinLoseRatio = CurrentWinLoseRatio + WinLoseRatioBoost * (WinLoseRatioBoostTimer /
		WinLoseRatioBoostMaxTimer);
	this->WinLoseRatio = FMath::Clamp(WinLoseRatio, 0.0f, 1.0f);
	this->WinLoseRatioHistory.Push(BaseWinLoseRatios[SelectedIdx]);
}
