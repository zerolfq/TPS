// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StateWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOBILETPS_API UStateWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UStateWidget(const FObjectInitializer& ObjectInitializer);
public:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable)
		void ChangeTextScore(float Score);
	UFUNCTION(BlueprintCallable)
		void ChangeOnHealth(float HpPercent);
private:
	class UTextBlock* TextScore;
	class UProgressBar* HpBar;
};
