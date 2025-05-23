// Copyright 2021 Dmitry Karpukhin. All Rights Reserved.

#include "BlockoutToolsParent.h"
#include "Components/SceneComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values
ABlockoutToolsParent::ABlockoutToolsParent(const FObjectInitializer  &ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Structure to hold one-time initialization
	struct FConstructorStatics
	{
		//Find billboard icon texture
		ConstructorHelpers::FObjectFinderOptional<UTexture2D> BillboardIcon;
		//Find material objects
		ConstructorHelpers::FObjectFinderOptional<UMaterialInterface> BlockoutGridParentMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInterface> BlockoutCustomMaterial;

		FConstructorStatics()
			: BillboardIcon(TEXT("Texture2D'/BlockoutToolsPlugin/Textures/T_Blockout_Actor_Icon'"))
			, BlockoutGridParentMaterial(TEXT("MaterialInterface'/BlockoutToolsPlugin/Materials/MI_BlockoutTools_Grid.MI_BlockoutTools_Grid'"))
			, BlockoutCustomMaterial(TEXT("MaterialInterface'/BlockoutToolsPlugin/Materials/MI_BlockoutTools_Custom.MI_BlockoutTools_Custom'"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	//Set default values
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;

	bNetUseOwnerRelevancy = true;

	BlockoutGridParent = ConstructorStatics.BlockoutGridParentMaterial.Get();
	BlockoutCustomMaterial = ConstructorStatics.BlockoutCustomMaterial.Get();

	bBlockoutCastShadows = true;

//	bRunConstructionScriptOnDrag = false;

	//Set root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Type::Static);
	RootComponent = Root;

#if WITH_EDITOR
	//Set billboard
	Billboard = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	if (Billboard)
	{
		Billboard->SetMobility(EComponentMobility::Type::Static);
		Billboard->SetupAttachment(RootComponent);
		Billboard->Sprite = ConstructorStatics.BillboardIcon.Get();
		Billboard->bIsScreenSizeScaled = true;
		Billboard->ScreenSize = 0.0005f;
	}
#endif
}

// Called when the game starts or when spawned
void ABlockoutToolsParent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlockoutToolsParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlockoutToolsParent::RerunConstructionScript()
{
#if WITH_EDITOR
	RerunConstructionScripts();
#endif
}

void ABlockoutToolsParent::BlockoutSetMaterial()
{
	// Create MID of a blockout parent grid material
	if (BlockoutGridParent != NULL && BlockoutGridMID == NULL)
	{
		BlockoutGridMID = UMaterialInstanceDynamic::Create(BlockoutGridParent, this);
	}

	// Set material type
	switch (BlockoutMaterialType)
	{
	case 0:
		if (BlockoutGridMID != NULL)
		{
			BlockoutGridMID->SetVectorParameterValue("Color", BlockoutMaterialColor);

			if (bBlockoutMaterialUseGrid == true)
			{
				BlockoutGridMID->SetScalarParameterValue("GridAlpha", 1.0f);
				BlockoutGridMID->SetScalarParameterValue("WorldAligned", bBlockoutWorldAligned);
				BlockoutGridMID->SetScalarParameterValue("GridSize", BlockoutMaterialGridSize);
				BlockoutGridMID->SetScalarParameterValue("CheckerLuminance", BlockoutMaterialCheckerLuminance);
			}
			else {
				BlockoutGridMID->SetScalarParameterValue("GridAlpha", 0.0f);
			}

			if (bBlockoutMaterialUseTopColor == true)
			{
				BlockoutGridMID->SetScalarParameterValue("TopColorAlpha", 1.0f);
			}
			else {
				BlockoutGridMID->SetScalarParameterValue("TopColorAlpha", 0.0f);
			}

			BlockoutGridMID->SetVectorParameterValue("TopColor", BlockoutMaterialTopColor);

			BlockoutCurrentMaterial = BlockoutGridMID;
		}
		break;

	case 1:
		BlockoutCurrentMaterial = BlockoutCustomMaterial;
		break;

	}

	// Assign material, collision profile and shadow to meshes
	this->GetComponents(BlockoutMeshComponents);
	for (auto StaticMeshComponent : BlockoutMeshComponents)
	{
		StaticMeshComponent->SetMaterial(0, BlockoutCurrentMaterial);

/*
		StaticMeshComponent->SetCustomPrimitiveDataVector4(0, BlockoutMaterialColor);

		if (bBlockoutMaterialUseGrid == true)
		{
			StaticMeshComponent->SetCustomPrimitiveDataFloat(8, 1.0f);
			StaticMeshComponent->SetCustomPrimitiveDataFloat(9, BlockoutMaterialGridSize);
			StaticMeshComponent->SetCustomPrimitiveDataFloat(10, BlockoutMaterialCheckerLuminance);
		}
		else {
			StaticMeshComponent->SetCustomPrimitiveDataFloat(8, 0.0f);
		}

		if (bBlockoutMaterialUseTopColor == true)
		{
			StaticMeshComponent->SetCustomPrimitiveDataFloat(11, 1.0f);
		}
		else {
			StaticMeshComponent->SetCustomPrimitiveDataFloat(11, 0.0f);
		}

		StaticMeshComponent->SetCustomPrimitiveDataVector4(4, BlockoutMaterialTopColor);
*/

		StaticMeshComponent->SetCastShadow(bBlockoutCastShadows);

		if (bBlockoutEnableCustomCollision) {
			StaticMeshComponent->SetCollisionProfileName(BlockoutCustomCollisionProfileName);
		}
	}

	SetActorEnableCollision(bBlockoutEnableCollisions);
}
