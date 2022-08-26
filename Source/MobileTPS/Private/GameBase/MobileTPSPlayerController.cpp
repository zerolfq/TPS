// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBase/MobileTPSPlayerController.h"
#include "GameBase/MobileTPSHUD.h"
#include "Widget/StateWidget.h"

AMobileTPSPlayerController::AMobileTPSPlayerController(){
	ConstructorHelpers::FClassFinder<UUserWidget> StateWidgetClass(TEXT("/Game/Widget/State"));
	SubStateClass = StateWidgetClass.Class;
}

void AMobileTPSPlayerController::BeginPlay(){
	if (IsLocalController()) {
		StateWidget = CreateWidget<UStateWidget>(GetWorld(), SubStateClass);
		if (StateWidget) {
			StateWidget->AddToViewport();
		}
	}
}

void AMobileTPSPlayerController::ChangeHUDScore(float Score){
	if (StateWidget)
		StateWidget->ChangeTextScore(Score);
}

void AMobileTPSPlayerController::ChangeHUDHP(float HpPercent){
	if (StateWidget)
		StateWidget->ChangeOnHealth(HpPercent);
}
