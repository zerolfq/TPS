// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MobileTPSGameStateBase.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EWaveState : uint8 {
	WaitingToStart = 0,
	WaveInProgress,
	WaittingToComplete,
	WaveComplete,
	GameOver
};
UCLASS()
class MOBILETPS_API AMobileTPSGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
protected:
	UFUNCTION()
		void OnRep_WaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category = GameState)
		void WaveStateChanged(EWaveState NewState, EWaveState OldState);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = GameState)
		EWaveState WaveState;
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	UFUNCTION(NetMulticast, Reliable)
		void OnGameOver();
public:
	void SetWaveState(EWaveState NewState);
	
};
