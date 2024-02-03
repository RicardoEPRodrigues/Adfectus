// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreUObject.h"
#include "TMModule.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CPPTHIRDPERSON_API UTMModule : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite)
	bool IsEnabled = true;

	/**
	 * Function called to initialize the module after the agent is already setup.
	 * Normally initialized during BeginPlay of @class UThreeMotionAgentComponent
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "3Motion")
	void Initialize(class UThreeMotionAgentComponent* OwnerComponent);

	/**
	 * Function called to execute the module during gameplay.
	 * Normally called on the TickComponent of @class UThreeMotionAgentComponent
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "3Motion")
	void Execute();

	/**
	 * Function called when the environment ends the gameplay.
	 * Normally called on the EndPlay of @class UThreeMotionAgentComponent
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "3Motion")
	void EndPlay();
};
