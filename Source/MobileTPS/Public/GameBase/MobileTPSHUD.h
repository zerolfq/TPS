// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MobileTPSHUD.generated.h"

/**
 * 
 */
UCLASS()
class MOBILETPS_API AMobileTPSHUD : public AHUD
{
	GENERATED_BODY()
public:
	AMobileTPSHUD();
private:
	class UAimWidget* AimWidget;
	//class UStateWidget* StateWidget;
public:
	void ChangeAimWidgetImageLocation(float Offset);

	//void ChangeStateWidgetScore(float Score);

	//void ChangeStatewidgetHP(float HpPercent);
};
