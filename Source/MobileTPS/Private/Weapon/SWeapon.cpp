// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SWeapon.h"
#include "MobileTPS/MobileTPSCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TraceTargetName = "BeamEnd";
	BaseDamage = 20.0f;

	WeaponRate = 600.0f;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay(){
	Super::BeginPlay();
}

void ASWeapon::Fire(){

}

void ASWeapon::StartFire(){
	if (WeaponState == EWeaponState::Equipping) return;
}

void ASWeapon::StopFire(){
}

void ASWeapon::PlayFireEffects(FVector TraceEndPoint){
}

void ASWeapon::PlayImpactEffects(EPhysicalSurface Surfacetype, FVector ImpactPoint){
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASWeapon::StopWeaponAnimation(UAnimMontage* WeaponAnimation){
	AMobileTPSCharacter* MyPawn = Cast<AMobileTPSCharacter>(GetOwner());
	if (MyPawn) {
		if (WeaponAnimation) {
			MyPawn->StopAnimMontage(WeaponAnimation);
		}
	}
}

float ASWeapon::PlayWeaponAnimation(UAnimMontage* WeaponAnimation){
	float Duration = 0.0f;
	UE_LOG(LogTemp,Warning,TEXT("Play WeaponAnimation"))
	AMobileTPSCharacter* MyPawn  = Cast<AMobileTPSCharacter>(GetOwner());
	if (MyPawn) {
		UE_LOG(LogTemp, Warning, TEXT("Play WeaponAnimation"))
		if (WeaponAnimation){
			UE_LOG(LogTemp, Warning, TEXT("Play WeaponAnimation"))
			Duration = MyPawn->PlayAnimMontage(WeaponAnimation);
		}
	}
	return Duration;
}

void ASWeapon::OnEquip(){
	WeaponState = EWeaponState::Equipping;
	float Duration = PlayWeaponAnimation(EquipAnimotage);
	if (Duration <= 0.0f) Duration = 0.5;

	EquipStartedTime = GetWorld()->GetTimeSeconds();
	EquipDuration = Duration;

	GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &ASWeapon::OnEquipFinsh, Duration, false);

	PlayWeaponSound(EquipSound);
}

void ASWeapon::OnUnEquip(){
	StopFire();
	if (WeaponState == EWeaponState::Equipping) {
		StopWeaponAnimation(EquipAnimotage);

		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	}
	if (WeaponState == EWeaponState::Firing) {
		StopFire();
	}
	WeaponState = EWeaponState::UnEquip;
	AMobileTPSCharacter* MyPawn = Cast<AMobileTPSCharacter>(GetOwner());
	if (MyPawn) {
		AttachMeshToPawn(MyPawn->GetBackSockeName());
	}
}

void ASWeapon::OnEquipFinsh(){
	WeaponState = EWeaponState::Idle;
	AMobileTPSCharacter* MyPawn = Cast<AMobileTPSCharacter>(GetOwner());
	if (MyPawn) {
		AttachMeshToPawn(MyPawn->GetEquipSockeName());
		MyPawn->SetCurWeapon(this);
	}
}

void ASWeapon::AttachMeshToPawn(FName SockeName){
	AMobileTPSCharacter* MyPawn = Cast<AMobileTPSCharacter>(GetOwner());
	if (MyPawn) {
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		AttachToComponent(MyPawn->GetMesh(),Rules, SockeName);
	}
}

UAudioComponent* ASWeapon::PlayWeaponSound(class USoundBase* Sound){
	UAudioComponent* AC = NULL;
	AMobileTPSCharacter* MyPawn = Cast<AMobileTPSCharacter>(GetOwner());
	if (Sound && MyPawn) {
		AC = UGameplayStatics::SpawnSoundAttached(Sound, MyPawn->GetRootComponent());
	}
	return AC;
}

void ASWeapon::OnRep_HitScanTrace(){
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType,HitScanTrace.TraceTo);
}

