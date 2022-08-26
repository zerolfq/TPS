// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/AimWidget.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/UMG/Public/Components/CanvasPanelSlot.h"

UAimWidget::UAimWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer){
	FireOffset = 2.0f;
	InitOffset = 8.0f;
}
bool UAimWidget::Initialize(){
	if (!Super::Initialize()) return false;

	ImageBottom = Cast<UImage>(GetWidgetFromName(TEXT("Bottom")));
	ImageTop = Cast<UImage>(GetWidgetFromName(TEXT("Top")));

	ImageRight = Cast<UImage>(GetWidgetFromName(TEXT("Right")));
	ImageLeft = Cast<UImage>(GetWidgetFromName(TEXT("Left")));

	return true;
}

void UAimWidget::ChangeImageLocation(float Offset){
	Offset = FMath::Min(Offset,3.0f);
	Offset = Offset * FireOffset;
	Cast<UCanvasPanelSlot>(ImageTop->Slot)->SetPosition(FVector2D(0.0, -(InitOffset + Offset)));
	Cast<UCanvasPanelSlot>(ImageBottom->Slot)->SetPosition(FVector2D(0.0, InitOffset + Offset));
	Cast<UCanvasPanelSlot>(ImageRight->Slot)->SetPosition(FVector2D(InitOffset + Offset,0.0));
	Cast<UCanvasPanelSlot>(ImageLeft->Slot)->SetPosition(FVector2D(-InitOffset - Offset, 0.0));
}
