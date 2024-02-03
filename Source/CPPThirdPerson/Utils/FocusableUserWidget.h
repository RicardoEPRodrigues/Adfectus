// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "FocusableUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class CPPTHIRDPERSON_API UFocusableUserWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	void SearchAndAddButtons(UWidget* widget);

public:
	UFocusableUserWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(Category = "User Interface", EditAnywhere, BlueprintReadWrite)
	TArray<UButton*> Buttons;
};
