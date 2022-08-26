// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBase/MobileTPSHUD.h"
#include "Widget/AimWidget.h"
#include "Widget/StateWidget.h"

AMobileTPSHUD::AMobileTPSHUD(){
	ConstructorHelpers::FClassFinder<UUserWidget> AimWidgetClass(TEXT("/Game/Widget/Aim"));
	AimWidget = CreateWidget<UAimWidget>(GetWorld(), AimWidgetClass.Class);
	if (AimWidget && GetWorld()->GetFirstPlayerController()->IsLocalController()) {
		AimWidget->AddToViewport();
	}

	//ConstructorHelpers::FClassFinder<UUserWidget> StateWidgetClass(TEXT("/Game/Widget/State"));
	//StateWidget = CreateWidget<UStateWidget>(GetWorld(), StateWidgetClass.Class);
	//if (StateWidget && GetWorld()->GetFirstPlayerController()->IsLocalController()) {
	//	StateWidget->AddToViewport();
	//}
}

void AMobileTPSHUD::ChangeAimWidgetImageLocation(float Offset){
	AimWidget->ChangeImageLocation(Offset);
}

//void AMobileTPSHUD::ChangeStateWidgetScore(float Score){
//	if (StateWidget) StateWidget->ChangeTextScore(Score);
//}
//
//void AMobileTPSHUD::ChangeStatewidgetHP(float HpPercent){
//	if (StateWidget) StateWidget->ChangeOnHealth(HpPercent);
//}
