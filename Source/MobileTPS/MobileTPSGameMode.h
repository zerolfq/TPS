// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MobileTPSGameMode.generated.h"

enum class EWaveState : uint8;
// 击杀事件
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled,AActor*,VictimActor,AActor*,KillerActor,AController*,KillerController);
UCLASS(minimalapi)
class AMobileTPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMobileTPSGameMode();

protected:
	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	int NrofBotsToSpawn;

	int WaveCount;
	UPROPERTY(EditDefaultsOnly,Category = GameMode)
	float TimeBetWeenwaves;
	UPROPERTY(EditDefaultsOnly, Category = GameMode)
	float MaxBotsToSpawn;

	bool bIsGameOver;
protected:
	UFUNCTION(BlueprintImplementableEvent,Category = GameMode)
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	// 开始生成机器人
	void StartWave();
	// 结束生成机器人
	void EndWave();
	// 设置时间生成下一波
	void PrePareForNextWave();

	void CheckWaveState();

	void CheckAnyPlayAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);

	void DistoryWave();
public:
	virtual void StartPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
public:
	UPROPERTY(BlueprintAssignable,Category = GameMode)
	FOnActorKilled OnActorKilled;
};



