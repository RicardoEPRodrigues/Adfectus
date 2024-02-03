// Fill out your copyright notice in the Description page of Project Settings.


#include "AdfectusUtilsLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

FRotator UAdfectusUtilsLibrary::MakeRotatorFromDirections(const FVector A, const FVector B)
{
	// Get Axis
	FVector RotationAxis = FVector::CrossProduct(A, B);
	RotationAxis.Normalize();

	// Get Angle in Radians
	const float DotProduct = FVector::DotProduct(A, B);
	const float RotationAngle = acosf(DotProduct);

	return UKismetMathLibrary::RotatorFromAxisAndAngle(RotationAxis, FMath::RadiansToDegrees(RotationAngle));
}


bool UAdfectusUtilsLibrary::DetectCollision(const AActor* A, const AActor* B, FHitResult& OutHit, float PreferBLocation)
{
	if (A && B && (B != A))
	{
		TArray<FHitResult> Results;
		auto ALoc = A->GetActorLocation();
		auto BLoc = B->GetActorLocation();
		auto CenterLoc = FMath::Lerp(ALoc, BLoc, PreferBLocation);
		auto CollisionRadius = FVector::Dist(ALoc, BLoc) * .5f;

		// spherical sweep 
		A->GetWorld()->SweepMultiByObjectType(Results, CenterLoc, BLoc,
		                                      FQuat::Identity, 0,
		                                      FCollisionShape::MakeSphere(CollisionRadius),
		                                      FCollisionQueryParams::DefaultQueryParam
		);

		for (auto HitResult : Results)
		{
			if (B->GetUniqueID() == HitResult.GetActor()->GetUniqueID())
			{
				OutHit = HitResult;
				// DrawDebugSphere(A->GetWorld(), HitResult.Location, 10.f, 12, FColor::Red, false, 5.0f);
				return true;
			}
		}
	}
	return false;
}

float UAdfectusUtilsLibrary::Exogenous(const float& Value, const float& Expected)
{
	return FMath::Pow(Value - Expected, 2.0f);
}

float UAdfectusUtilsLibrary::Endogenous(const float& Value, const float& Expected, const float& DesiredValue)
{
	const float DeltaSquared = FMath::Pow(Value - DesiredValue, 2.0f);
	const float DeltaSquaredExpectation = FMath::Pow(Value - Expected, 2.0f);

	return DeltaSquared - DeltaSquaredExpectation;
}

void UAdfectusUtilsLibrary::Derivative(const TArray<float>& Values, TArray<float>& Out)
{
	Out.Empty();
	for (int i = 1; i < Values.Num(); ++i)
	{
		Out.Add(Values[i] - Values[i - 1]);
	}
}


ULocalPlayer* UAdfectusUtilsLibrary::GetLocalPlayer(APlayerController* Controller)
{
	if (!Controller || !Controller->GetWorld())
	{
		return nullptr;
	}
	
	return GEngine->GetLocalPlayerFromControllerId(Controller->GetWorld(),
	                                               UGameplayStatics::GetPlayerControllerID(Controller));
}
