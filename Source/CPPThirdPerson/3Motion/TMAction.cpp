// Fill out your copyright notice in the Description page of Project Settings.


#include "TMAction.h"

UTMAction::UTMAction() : Name("Default Action"), Description("Default Action Description")
{
}

bool UTMAction::operator==(const UTMAction& Action) const
{
	return this->Name.Equals(Action.Name) && this->Stage == Action.Stage;
}
