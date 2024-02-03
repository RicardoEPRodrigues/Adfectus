// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TMPercept.generated.h"

/**
 * This class represents the data associated with a percept.
 * It cannot be instantiated and should be extended to 
 */
UCLASS(Abstract)
class CPPTHIRDPERSON_API UTMPerceptData : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * @brief Compares the data to check if two data instances are equal.
	 * @param Other Data to be compared to.
	 * @return True if the data is considered equal. False, otherwise.
	 */
	UFUNCTION(Category="3Motion", BlueprintNativeEvent, BlueprintCallable)
	bool Equals(const UTMPerceptData* Other);
};

/**
 * This class represents a percept, or an impression of an object obtained by use of the senses, in 3Motion.
 */
UCLASS()
class CPPTHIRDPERSON_API UTMPercept : public UDataAsset
{
	GENERATED_BODY()

public:
	/// Actor to which this percept refers to.
	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite)
	class AActor* Actor;

	/// Time at which the percept was created, collected by using the current Time in seconds.
	/// @code
	/// GetWorld()->GetTimeSeconds();
	/// @endcode 
	UPROPERTY(Category="3Motion", EditAnywhere, BlueprintReadWrite)
	float Time = -1;

	/// Contains any data related to this percept (e.g. character health, character position, etc...)
	UPROPERTY(Category="3Motion", VisibleAnywhere, BlueprintReadOnly)
	UTMPerceptData* Data;
	
	bool operator==(const UTMPercept& Other) const;

	/**
	 * @brief Similar to operator== but does not take into account Time member.
	 * It compares the actions by pointer and values if possible.
	 * @param Other Percept to compare to.
	 * @return True if both percepts are equivalent. False, otherwise.
	 */
	bool IsEquivalent(const UTMPercept& Other) const;
};
