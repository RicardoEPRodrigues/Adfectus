// Fill out your copyright notice in the Description page of Project Settings.


#include "TMPercept.h"

bool UTMPerceptData::Equals_Implementation(const UTMPerceptData* Other)
{
	return true;
}

bool UTMPercept::operator==(const UTMPercept& Other) const
{
	return IsEquivalent(Other) && FMath::IsNearlyEqual(this->Time, Other.Time);
}

bool UTMPercept::IsEquivalent(const UTMPercept& Other) const
{
	bool IsDataEqual = this->Data == Other.Data;
	if (!IsDataEqual)
	{
		IsDataEqual = this->Data->Equals(Other.Data);
	}
	return this->Actor == Other.Actor && IsDataEqual;
}
