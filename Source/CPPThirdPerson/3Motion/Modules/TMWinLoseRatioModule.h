#pragma once
#include "CPPThirdPerson/3Motion/ETMActionStage.h"
#include "CPPThirdPerson/3Motion/TMAction.h"
#include "CPPThirdPerson/3Motion/TMModule.h"
#include "CPPThirdPerson/3Motion/TMPercept.h"
#include "TMWinLoseRatioModule.generated.h"

UENUM(BlueprintType)
enum class EWinLoseRatioType : uint8
{
	StaticWinLoseRatio UMETA(DisplayName="Static Win-Lose Ratio"),
	ReactiveWinLoseRatio UMETA(DisplayName="Ractive Win-Lose Ratio"),
	PredictedWinLoseRatioPerStage UMETA(DisplayName="Predicted Win-Lose Ratio Per Stage"),
	PredictedWinLoseRatioPerStageWithAssertion UMETA(DisplayName="Predicted Win-Lose Ratio Per Stage With Assertion")
};

struct FWinLoseRatioData
{
	const class ACPPThirdPersonCharacter* Actor = nullptr;
	const UTMAction* Action = nullptr; 
	float WinLoseRatio = 0.5f;
};

/**
 * This module is responsible for calculating the win-lose ratio based of the most recent percept 
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UTMWinLoseRatioModule : public UTMModule
{
	GENERATED_BODY()

	DECLARE_DELEGATE(FWinLoseRatioDelegate)
	
	UPROPERTY(Category = "3Motion", EditAnywhere, meta=(ClampMin="0.0", ClampMax="1.0"))
	float StartWinLoseRatio = 0.5f;
	
	UPROPERTY(Category = "3Motion", EditAnywhere, BlueprintGetter=GetWinLoseRatioType, BlueprintSetter=SetWinLoseRatioType)
	EWinLoseRatioType WinLoseRatioType = EWinLoseRatioType::ReactiveWinLoseRatio;
	
	UPROPERTY()
	class ACPPThirdPersonCharacter* Owner;

	UPROPERTY()
	UThreeMotionAgentComponent* OwnerAgentComponent;

	UPROPERTY(Category = "3Motion", VisibleAnywhere, BlueprintGetter=GetWinLoseRatio)
	float WinLoseRatio;
	UPROPERTY()
	const ACPPThirdPersonCharacter* PredictionCharacter;

	float MvAverage;
	float FirstDerivative;
	float SecondDerivative;
	float HealthOffset;
	
	UPROPERTY()
	UTMPercept* LastPercept;
	UPROPERTY()
	TArray<UTMPercept*> LastPercepts = TArray<UTMPercept*>();
	
	TArray<float> WinLoseRatioHistory = TArray<float>();
	TArray<float> FirstDerivatives = TArray<float>();
	TArray<float> SecondDerivatives = TArray<float>();
	TArray<FWinLoseRatioData> WinLoseRatioDataHistory = TArray<FWinLoseRatioData>();
	float CurrentWinLoseRatio;
	float WinLoseRatioBoostTimer = 0.0f;
	float WinLoseRatioBoostMaxTimer = 10.0f;
	float WinLoseRatioBoost = 0.0f;
	
	FWinLoseRatioDelegate CalculateWinLoseRatio;

	UFUNCTION()
	void ReactiveWinLoseRatio();
	UFUNCTION()
	void PredictedWinLoseRatio();
	UFUNCTION()
	void PredictedWinLoseRatioWithAssertion();

	float BaseWinLoseRatio(float ANormalizedHealth, float BNormalizedHealth) const;
	TArray<float> GetWinLoseRatioOutcomesDerivativeForStage(ETMActionStage ActionStage, const ACPPThirdPersonCharacter* Actor) const;

	TArray<float> GetWinLoseRatioOutcomesForStage(ETMActionStage ActionStage, const ACPPThirdPersonCharacter* Actor) const;

public:

	UTMWinLoseRatioModule();
	
	/**
	 * @brief Class defining the personality with emotions limits of the character.
	 */
	UPROPERTY(Category = "3Motion", EditAnywhere, BlueprintReadWrite)
	class UPersonality* Personality;

	UFUNCTION(Category = "3Motion", BlueprintGetter)
	float GetWinLoseRatio() const { return WinLoseRatio; }

	UFUNCTION(Category = "3Motion", BlueprintSetter)
	void SetWinLoseRatioType(const EWinLoseRatioType NewWinLoseRatioType);

	UFUNCTION(Category = "3Motion", BlueprintGetter)
	EWinLoseRatioType GetWinLoseRatioType() const;
	
	virtual void Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent) override;
	virtual void Execute_Implementation() override;
	virtual void EndPlay_Implementation() override;
	
};
