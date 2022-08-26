// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBase/MobileTPSGameStateBase.h"
#include "GameBase/MobileTPSPlayerController.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"
#include "GameBase/MobileTPSPlayerState.h"
#include "Runtime/AIModule/Classes/AIController.h"

void AMobileTPSGameStateBase::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(WaveState, OldState);
}

void AMobileTPSGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMobileTPSGameStateBase,WaveState);
}

void AMobileTPSGameStateBase::OnGameOver_Implementation(){
	TArray<int> Score;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++) {
		AMobileTPSPlayerController* PC = Cast<AMobileTPSPlayerController>(It->Get());
		if (PC && PC->IsLocalController()) {
			Score.Add(PC->GetPlayerState<AMobileTPSPlayerState>()->GetScore());
		}
	}
	float Score_A = 0.0,Score_B = 0.0;
	if (Score.Num()> 0) Score_A = Score[0];
	if (Score.Num() > 1) Score_B = Score[1];
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++) {
		AMobileTPSPlayerController* PC = Cast<AMobileTPSPlayerController>(It->Get());
		if (PC && PC->IsLocalController()) {
			PC->SetGameOverWidget(Score_A, Score_B,Score_A / 20.0,Score_B / 20.0);
			PC->bShowMouseCursor = true;
			APawn* MyPawn = PC->GetPawn();
			if (MyPawn) MyPawn->DisableInput(PC);
		}
	}
}

void AMobileTPSGameStateBase::SetWaveState(EWaveState NewState){
	// C++与蓝图的差异 RepNotify只在C++上适用
	if (GetLocalRole() == ROLE_Authority) {
		EWaveState OldState = WaveState;
		WaveState = NewState;
		OnRep_WaveState(OldState);
	}
}
