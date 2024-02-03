// Fill out your copyright notice in the Description page of Project Settings.


#include "TMEmotion.h"

UTMEmotion::UTMEmotion() : Name("Default Emotion"), Description("Default Emotion Description"), Intensity(1)
{
}

bool UTMEmotion::operator==(const UTMEmotion& Emotion) const
{
	return this->Name.Equals(Emotion.Name) && FMath::IsNearlyEqual(this->Intensity, Emotion.Intensity);
}
