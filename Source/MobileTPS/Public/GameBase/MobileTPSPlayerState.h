// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MobileTPSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MOBILETPS_API AMobileTPSPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,Category = PlayerState)
	void AddScore(float ScoreDelta);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	UFUNCTION()
		void OnRep_ScoreChange();
protected:
	UPROPERTY(ReplicatedUsing = OnRep_ScoreChange)
	float ClientScore;
};
