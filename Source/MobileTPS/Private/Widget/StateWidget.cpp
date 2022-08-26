// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/StateWidget.h"
#include "UMG/Public/Components/TextBlock.h"
#include "UMG/Public/Components/ProgressBar.h"

UStateWidget::UStateWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer){

}

bool UStateWidget::Initialize(){
	if (!Super::Initialize()) return false;
	TextScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("Score")));
	HpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HpBar")));

	TextScore->SetText(FText::FromString(FString("0.0")));
	return true;
}

void UStateWidget::ChangeTextScore(float Score){
	if (IsValid(TextScore))
		TextScore->SetText(FText::FromString(FString::Printf(TEXT("%0.1f"),Score)));
}

void UStateWidget::ChangeOnHealth(float HpPercent){
	if (IsValid(TextScore))
	{
		HpPercent = FMath::Clamp(HpPercent, 0.f, 1.f);
		HpBar->SetPercent(HpPercent);
		if (HpPercent <= 0.f){
			HpBar->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
