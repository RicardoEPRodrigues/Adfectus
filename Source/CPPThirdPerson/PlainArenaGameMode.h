#pragma once
#include "BestOutOfGameMode.h"
#include "EGameStage.h"
#include "PlainArenaGameMode.generated.h"

UCLASS()
class CPPTHIRDPERSON_API APlainArenaGameMode : public ABestOutOfGameMode
{
	GENERATED_BODY()

public:
	APlainArenaGameMode();

	void SwapCharacters(APlayerController* ControllerA, APlayerController* ControllerB);

	virtual void OnAnimationEnd(EGameStage Stage) override;
};
