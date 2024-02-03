// Fill out your copyright notice in the Description page of Project Settings.

#include "FocusableUserWidget.h"

#include "Engine.h"
#include "Components/PanelWidget.h"

UFocusableUserWidget::UFocusableUserWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UFocusableUserWidget::NativeConstruct()
{
	SearchAndAddButtons(GetRootWidget());

	// UE_LOG(LogTemp, Warning, TEXT("My Construct %d"), Buttons.Num());

	Super::NativeConstruct();
}

void UFocusableUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UFocusableUserWidget::SearchAndAddButtons(UWidget* widget)
{
	// Get the root panel widget (if you know its a panel, otherwise cast to the widget you have as root)
	if (UPanelWidget* panelWidget = Cast<UPanelWidget>(widget))
	{
		int32 ChildrenCount = panelWidget->GetChildrenCount();

		for (int32 i = 0; i < ChildrenCount; i++)
		{
			UWidget* innnerWidget = panelWidget->GetChildAt(i);

			if (!innnerWidget)
			{
				continue;
			}

			// if the current child is of widgettype i want to save add it to the array
			if (UPanelWidget* panel = Cast<UPanelWidget>(innnerWidget))
			{
				SearchAndAddButtons(panel);
			}
			if (UButton* btn = Cast<UButton>(innnerWidget))
			{
				Buttons.Add(btn);
			}
		}
	}
}
