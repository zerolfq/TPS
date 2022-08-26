// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MobileTPSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MOBILETPS_API AMobileTPSPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMobileTPSPlayerController();
public:
	virtual void BeginPlay() override;
private:
	class UStateWidget* StateWidget;
	TSubclassOf<class UUserWidget> SubStateClass;
public:
	UFUNCTION(BlueprintCallable)
		void ChangeHUDScore(float Score);
	UFUNCTION(BlueprintCallable)
		void ChangeHUDHP(float HpPercent);
	UFUNCTION(BlueprintImplementableEvent)
		void SetGameOverWidget(float Score_A, float Score_B, float Kill_A, float Kill_B);
};
