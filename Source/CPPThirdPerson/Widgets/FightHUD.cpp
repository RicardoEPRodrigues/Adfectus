// Fill out your copyright notice in the Description page of Project Settings.


#include "FightHUD.h"

#include "../BestOutOfGameMode.h"
#include "CPPThirdPerson/CPPThirdPersonCharacter.h"
#include "Kismet/GameplayStatics.h"

void UFightHUD::OnCharacterHealthChanged(ACPPThirdPersonCharacter* Character)
{
	int Id = UGameplayStatics::GetPlayerControllerID(Character->GetLocalViewingPlayerController());
	
	if (Id < 0) return;
	
	UpdatePlayerHealth(Id, Character->GetNormalizedHealth());
}

void UFightHUD::OnPlayerWinRound(uint8 Player1Wins, uint8 Player2Wins)
{
	UpdatePlayerWins(Player1Wins, Player2Wins);
}

