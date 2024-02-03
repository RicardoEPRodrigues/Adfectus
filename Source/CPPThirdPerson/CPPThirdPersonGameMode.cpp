// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CPPThirdPersonGameMode.h"
#include "CPPThirdPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACPPThirdPersonGameMode::ACPPThirdPersonGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
		TEXT("/Game/Adfectus/Blueprints/Characters/AdfectusCharacter_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(
		TEXT("/Game/Adfectus/Blueprints/Characters/Adfectus_Controller_Player"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}
