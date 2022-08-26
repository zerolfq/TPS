// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AimWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOBILETPS_API UAimWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UAimWidget(const FObjectInitializer& ObjectInitializer);
public:
	virtual bool Initialize() override;

	void ChangeImageLocation(float Offset);
private:
	class UImage* ImageTop;
	class UImage* ImageBottom;
	class UImage* ImageRight;
	class UImage* ImageLeft;

	UPROPERTY(EditAnywhere)
		float FireOffset;
	UPROPERTY(EditAnywhere)
	float InitOffset;
};
