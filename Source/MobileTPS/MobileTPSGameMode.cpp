// Copyright Epic Games, Inc. All Rights Reserved.

#include "MobileTPSGameMode.h"
#include "MobileTPSCharacter.h"
#include "GameBase/MobileTPSHUD.h"
#include "GameBase/MobileTPSPlayerController.h"
#include "GameBase/MobileTPSGameStateBase.h"
#include "GameBase/MobileTPSPlayerState.h"
#include "UObject/ConstructorHelpers.h"
#include "Component/HealthComponent.h"
#include "TimerManager.h"
#include "Runtime/Engine/Public/EngineUtils.h"

AMobileTPSGameMode::AMobileTPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BluePrints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL) 
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	GameStateClass = AMobileTPSGameStateBase::StaticClass();
	HUDClass = AMobileTPSHUD::StaticClass();
	PlayerControllerClass = AMobileTPSPlayerController::StaticClass();
	PlayerStateClass = AMobileTPSPlayerState::StaticClass();
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	TimeBetWeenwaves = 2.0;
	MaxBotsToSpawn = 32.0f;
	WaveCount = 0.0f;
	bIsGameOver = false;
}

void AMobileTPSGameMode::StartPlay() {
	Super::StartPlay();

	PrePareForNextWave();
}

void AMobileTPSGameMode::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	if (bIsGameOver) return;
	CheckWaveState();

	CheckAnyPlayAlive();
}

void AMobileTPSGameMode::SpawnBotTimerElapsed(){
	SpawnNewBot();
	NrofBotsToSpawn--;
	if (NrofBotsToSpawn <= 0) {
		EndWave();
	}
}

void AMobileTPSGameMode::StartWave(){
	WaveCount++;
	NrofBotsToSpawn = 2 * WaveCount;
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner,this,&AMobileTPSGameMode::SpawnBotTimerElapsed,1.0f,true,0.0f);

	SetWaveState(EWaveState::WaveInProgress);
}

void AMobileTPSGameMode::EndWave(){
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	SetWaveState(EWaveState::WaittingToComplete);
	//PrePareForNextWave();
}

void AMobileTPSGameMode::PrePareForNextWave(){
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart,this,&AMobileTPSGameMode::StartWave, TimeBetWeenwaves,false);

	SetWaveState(EWaveState::WaitingToStart);
}


void AMobileTPSGameMode::CheckWaveState(){
	
	bool bISPreParingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NrofBotsToSpawn > 0 || bISPreParingForWave) return;

	bool bIsAnyBotAlive = false;
	for (TActorIterator<APawn> It(GetWorld()) ; It; ++It) {
		APawn* TestPawn = *It;
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled()) continue;
		UHealthComponent* HealthComp = Cast<UHealthComponent>(TestPawn->GetComponentByClass(UHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f) {
			bIsAnyBotAlive = true;
			break;
		}
	}
	if (!bIsAnyBotAlive) {

		SetWaveState(EWaveState::WaveComplete);
		PrePareForNextWave();
	}
}


void AMobileTPSGameMode::CheckAnyPlayAlive(){
	if (!GetWorld()->GetPlayerControllerIterator()) {
		UE_LOG(LogTemp, Warning, TEXT("No Player"));
		return;
	}
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++) {
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn()) {
			APawn* MyPawn =PC->GetPawn();
			UHealthComponent* HealthComp = Cast<UHealthComponent>(MyPawn->GetComponentByClass(UHealthComponent::StaticClass()));
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f) {
				return ;
			}
		}
	}
	GameOver();
}


void AMobileTPSGameMode::GameOver(){
	EndWave();
	SetWaveState(EWaveState::GameOver);
	bIsGameOver = true;

	AMobileTPSGameStateBase* GS = GetGameState<AMobileTPSGameStateBase>(); // »ñÈ¡ÓÎÏ·×´Ì¬
	DistoryWave();
	if (ensure(GS)) {
		GS->OnGameOver();
	}

	UE_LOG(LogTemp,Warning,TEXT("Game Over"));

}

void AMobileTPSGameMode::SetWaveState(EWaveState NewState) {
	AMobileTPSGameStateBase* GS = GetGameState<AMobileTPSGameStateBase>();
	if (ensure(GS)) {
		GS->SetWaveState(NewState);
	}
}

void AMobileTPSGameMode::DistoryWave(){
	for (TActorIterator<APawn> It(GetWorld()); It; ++It) {
		APawn* TestPawn = *It;
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled()){
			TestPawn->DetachFromControllerPendingDestroy();
			AMobileTPSCharacter * TPSPawn = Cast<AMobileTPSCharacter>(TestPawn);
			if (TPSPawn) TPSPawn->DestroyWeapon();

		}
	}
}

