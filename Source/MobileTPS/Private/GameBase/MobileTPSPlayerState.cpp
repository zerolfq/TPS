// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBase/MobileTPSPlayerState.h"
#include "GameBase/MobileTPSPlayerController.h"
#include "Net/UnrealNetwork.h"

void AMobileTPSPlayerState::AddScore(float ScoreDelta){
	Score += ScoreDelta;
	if (HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("This is Server"));
		ClientScore = Score;
		OnRep_ScoreChange();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("This is Client"));
	}
}

void AMobileTPSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AMobileTPSPlayerState, ClientScore, COND_OwnerOnly);
}

void AMobileTPSPlayerState::OnRep_ScoreChange(){
	AController* APC = this->GetPawn()->GetController();
	AMobileTPSPlayerController* PC = Cast<AMobileTPSPlayerController>(APC);
	if (PC && PC->IsLocalController()) {
		PC->ChangeHUDScore(GetScore());
	}
}
