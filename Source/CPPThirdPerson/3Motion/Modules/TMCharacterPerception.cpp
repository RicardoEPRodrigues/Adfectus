#include "TMCharacterPerception.h"

#include "CPPThirdPerson/CPPThirdPersonCharacter.h"
#include "CPPThirdPerson/3Motion/ThreeMotionAgentComponent.h"
#include "CPPThirdPerson/3Motion/TMotionUtils.h"
#include "CPPThirdPerson/3Motion/TMTheoryOfMind.h"
#include "Kismet/GameplayStatics.h"

bool UTMCharacterPerceptData::Equals_Implementation(const UTMPerceptData* Other)
{
	if (const UTMCharacterPerceptData* PerceptData = Cast<UTMCharacterPerceptData>(Other))
	{
		// Either the pointers are equal or their values are equal.
		bool IsActionEqual = this->Action == PerceptData->Action;
		if (!IsActionEqual && this->Action && PerceptData->Action)
		{
			IsActionEqual = *(this->Action) == *(PerceptData->Action);
		}
		bool IsEmotionEqual = this->Emotions == PerceptData->Emotions;
		if (!IsEmotionEqual && this->Emotions.Num() == PerceptData->Emotions.Num())
		{
			// Compare each Emotion value
			for (int i = 0; i < this->Emotions.Num(); ++i)
			{
				IsEmotionEqual = *(this->Emotions[i]) == *(PerceptData->Emotions[i]);
			}
		}

		const bool IsHealthEqual = this->Health == PerceptData->Health;

		// The high tolerance allows us to not constantly fill the percepts with location data.
		//const bool IsTransformEqual = this->Transform.Equals(PerceptData->Transform, 50);

		return IsActionEqual && IsEmotionEqual && IsHealthEqual;
	}

	return false;
}

TArray<UTMPercept*> UTMCharacterPerception::FindPercepts_Implementation(const UWorld* World)
{
	TArray<AActor*> ActorsToFind;
	UGameplayStatics::GetAllActorsOfClass(World, ACPPThirdPersonCharacter::StaticClass(), ActorsToFind);

	TArray<UTMPercept*> ActorPercepts;
	for (AActor* Actor : ActorsToFind)
	{
		if (ACPPThirdPersonCharacter* Character = Cast<ACPPThirdPersonCharacter>(Actor))
		{
			UTMCharacterPerceptData* PerceptData = NewObject<UTMCharacterPerceptData>(OwnerAgentComponent);
			
			// Create a copy of the current action.
			if (const UTMAction* Action = Character->AgentComp->TheoryOfMind->CurrentAction)
			{
				if (const TWeakObjectPtr<UTMPercept>* LastPercept = PreviousPercepts.Find(Actor); LastPercept && LastPercept->IsValid())
				{
					const UTMCharacterPerceptData* LastPerceptData = Cast<UTMCharacterPerceptData>(LastPercept->Get()->Data);

					// Avoid recreating a percept for the same action stage
					if (LastPerceptData->Action && Action && *LastPerceptData->Action == *Action && LastPerceptData->Action->Stage == Action->Stage) continue;
				}

				// Ignoring Walk and Sprint actions
				if (Action->Name.Contains("Walk") || Action->Name.Contains("Sprint")) continue;
				
				PerceptData->Action = UTMotionUtils::DuplicateAction(Action, PerceptData);
			}

			// Create a copy of the current emotion.
			if (!Character->AgentComp->TheoryOfMind->CurrentEmotions.IsEmpty())
			{
				for (int i = 0; i < Character->AgentComp->TheoryOfMind->CurrentEmotions.Num(); ++i)
				{
					UTMEmotion* EmotionDuplicate = UTMotionUtils::DuplicateEmotion(
						Character->AgentComp->TheoryOfMind->CurrentEmotions[i], PerceptData);
					PerceptData->Emotions.Add(EmotionDuplicate);
				}
			}

			PerceptData->Health = Character->Health;
			PerceptData->NormalizedHealth = Character->GetNormalizedHealth();
			PerceptData->Transform = Character->GetTransform();

			UTMPercept* Percept = UTMotionUtils::CreatePercept(
				Character,
				GetWorld()->GetTimeSeconds(),
				OwnerAgentComponent,
				PerceptData
			);

			ActorPercepts.Add(Percept);
		}
	}

	return ActorPercepts;
}
