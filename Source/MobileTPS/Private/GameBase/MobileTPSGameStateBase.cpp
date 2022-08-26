// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBase/MobileTPSGameStateBase.h"
#include "Net/UnrealNetwork.h"

void AMobileTPSGameStateBase::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(WaveState, OldState);
}

void AMobileTPSGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMobileTPSGameStateBase,WaveState);
}

void AMobileTPSGameStateBase::SetWaveState(EWaveState NewState){
	// C++����ͼ�Ĳ��� RepNotifyֻ��C++������
	if (GetLocalRole() == ROLE_Authority) {
		EWaveState OldState = WaveState;
		WaveState = NewState;
		OnRep_WaveState(OldState);
	}
}
