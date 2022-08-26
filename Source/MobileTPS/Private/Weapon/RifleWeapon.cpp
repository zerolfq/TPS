// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RifleWeapon.h"
#include "MobileTPS/MobileTPS.h"
#include "GameBase/MobileTPSPlayerController.h"
#include "GameBase/MobileTPSHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Components/AudioComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "MobileTPS/MobileTPSCharacter.h"
#include "MobileTPS/MobileTPS.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

ARifleWeapon::ARifleWeapon(){
	FireTime = 0.0;
	AimOffset = 0.5; 
	InitOffset = 0.0;
}

void ARifleWeapon::Fire(){
	AMobileTPSCharacter* MyOwner = Cast<AMobileTPSCharacter>(GetOwner());
	if (MyOwner) {
		FVector EyeLocation = MyOwner->GetCamera()->GetComponentLocation();
		FRotator EyeRotation = MyOwner->GetCamera()->GetComponentRotation();

		const int32 RandomSeed = FMath::Rand();
		FRandomStream WeaponRandomStream(RandomSeed);
		const float ConeHalfAngle = FMath::DegreesToRadians((InitOffset + FMath::Min(FireTime,3.0f)) * AimOffset);
		FVector EyeRotationV = WeaponRandomStream.VRandCone(EyeRotation.Vector(), ConeHalfAngle, ConeHalfAngle);

		FVector EndTrace = EyeLocation + (EyeRotationV * 10000);

		FVector TraceEndPoint = EndTrace;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FHitResult Hit;
		bool Result = GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, EndTrace, COLLISION_WEAPON, QueryParams);
		EPhysicalSurface Surfacetype = SurfaceType_Default;
		if (Result) {
			AActor* HitActor = Hit.GetActor();
			if (Hit.Component->IsSimulatingPhysics()){
				float RandomIntensity = FMath::RandRange(200.0f, 500.0f);
				Hit.Component->AddImpulseAtLocation(EyeRotation.Vector() * RandomIntensity, GetActorLocation());
			}
			float CurrDamage = BaseDamage;
			Surfacetype = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			if (Surfacetype == FleshVulnerable) CurrDamage *= 2;

			UGameplayStatics::ApplyPointDamage(HitActor, CurrDamage, EyeRotation.Vector(), Hit, MyOwner->GetInstigatorController(), this, DamageType);
			PlayImpactEffects(Surfacetype,Hit.ImpactPoint);
			TraceEndPoint = Hit.ImpactPoint;
		}
		PlayFireEffects(TraceEndPoint);
		if (GetLocalRole() == ROLE_Authority) {
			HitScanTrace.TraceTo = TraceEndPoint;
			HitScanTrace.SurfaceType = Surfacetype;
		}

		FireTime = GetWorld()->TimeSeconds - StartFireTime;
		LastFire = GetWorld()->TimeSeconds;
		ChangeHUD();
	}

}

void ARifleWeapon::StartFire(){
	if (GetLocalRole() < ROLE_Authority) ServerStartFire();
	Super::StartFire();
	FireAC = PlayWeaponSound(FireSound);
	float FirstFire = FMath::Max(LastFire + FireRate - GetWorld()->TimeSeconds, 0.0f);
	if (WeaponState == EWeaponState::Equipping) FirstFire = 0.5;
	WeaponState = EWeaponState::Firing;
	StartFireTime = GetWorld()->TimeSeconds + FirstFire;
	GetWorldTimerManager().SetTimer(TimeHandle_Fire, this, &ARifleWeapon::Fire, FireRate, true, FirstFire);
}

void ARifleWeapon::StopFire(){
	if (GetLocalRole() < ROLE_Authority) ServerStopFire();
	WeaponState = EWeaponState::Idle;
	GetWorldTimerManager().ClearTimer(TimeHandle_Fire);
	if (FireAC) {
		FireAC->FadeOut(0.1f, 0.0f);
		FireAC = NULL;
	}
	FireTime = 0.0;
	StartFireTime = -1;
	ChangeHUD();
}

void ARifleWeapon::PlayFireEffects(FVector TraceEndPoint){
	if (MuzzleEffect)
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);

	FVector MuzzleLocator = MeshComp->GetSocketLocation(MuzzleSocketName);

	UParticleSystemComponent* TraceEffectComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleLocator);
	if (TraceEffectComp) {
		TraceEffectComp->SetVectorParameter(TraceTargetName, TraceEndPoint);
	}
	
	APawn* MyOwner = Cast<APawn>(GetOwner());

	if (MyOwner) {
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC) {
			PC->ClientStartCameraShake(FireCamShake);
		}
	}
}

void ARifleWeapon::PlayImpactEffects(EPhysicalSurface Surfacetype,FVector ImpactPoint){

	UParticleSystem* SelectedEffect = nullptr;
	float CurrDamage = BaseDamage;
	switch (Surfacetype)
	{
	case FleshDefault:
		SelectedEffect = VulnerImpactEffect;
		break;
	case FleshVulnerable:
		CurrDamage *= 2;
		SelectedEffect = VulnerImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}
	if (SelectedEffect) {
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		
		FVector ShotDir = ImpactPoint - MuzzleLocation;
		ShotDir.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDir.Rotation());
	}
		
}

void ARifleWeapon::ServerStartFire_Implementation(){
	StartFire();
}

bool ARifleWeapon::ServerStartFire_Validate(){
	return true;
}

void ARifleWeapon::ServerStopFire_Implementation(){
	StopFire();
}

bool ARifleWeapon::ServerStopFire_Validate(){
	return true;
}

void ARifleWeapon::ChangeHUD(){
	AMobileTPSHUD* HUD = Cast<AMobileTPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (HUD) {
		HUD->ChangeAimWidgetImageLocation(FireTime);
	}
}

