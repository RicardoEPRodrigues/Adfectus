// Copyright 2017-2022 Ricardo Rodrigues. All Rights Reserved.


#include "IPredictor.h"


// Add default functionality here for any IPredictor functions that are not pure virtual.

float IPredictor::PredictWithDesiredValue_Implementation(const TArray<float>& Values, const TArray<float>& Predictions,
                                                          const float DesiredValue)
{
	return Predict(Values, Predictions);
}
