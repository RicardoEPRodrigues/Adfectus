// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CPPThirdPerson/3Motion/TMModule.h"
#include "UObject/Object.h"
#include "TMDirectMapPerformModule.generated.h"

/**
 * Direct Mapping Perform Module
 * This is a perform module for 3Motion that directly maps the desired emotions and actions to the current correspondents.
 */
UCLASS(Blueprintable, EditInlineNew)
class CPPTHIRDPERSON_API UTMDirectMapPerformModule : public UTMModule
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class ACPPThirdPersonCharacter* Owner;

	UPROPERTY()
	class UThreeMotionAgentComponent* OwnerAgentComponent;

public:
	virtual void Initialize_Implementation(UThreeMotionAgentComponent* OwnerComponent) override;
	virtual void Execute_Implementation() override;
	virtual void EndPlay_Implementation() override;
};
