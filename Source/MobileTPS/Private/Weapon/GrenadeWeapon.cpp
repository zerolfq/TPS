// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GrenadeWeapon.h"
#include "Camera/CameraComponent.h"
#include "Weapon/Bullet.h"
#include "Kismet/GameplayStatics.h"
#include "MobileTPS/MobileTPSCharacter.h"

AGrenadeWeapon::AGrenadeWeapon(){
	
}

void AGrenadeWeapon::Fire(){
	AMobileTPSCharacter* MyOwner = Cast<AMobileTPSCharacter>(GetOwner());
	if (MyOwner) {
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

		ServerFire(MuzzleLocation, MuzzleRotation.Vector());
		//if (GetLocalRole() == ROLE_Authority) {
		//	const int32 RandomSeed = FMath::Rand();
		//	FRandomStream WeaponRandomStream(RandomSeed);
		//	const float ConeHalfAngle = FMath::DegreesToRadians(5.0);
		//	FVector Offset = WeaponRandomStream.VRandCone(MuzzleRotation.Vector(), ConeHalfAngle, ConeHalfAngle);
		//	UE_LOG(LogTemp, Warning, TEXT("This Vector %f %f %f"), Offset.X, Offset.Y, Offset.Z);
		//	HitScanTrace.TraceTo = Offset;
		//}
	}
}

void AGrenadeWeapon::StartFire(){
	Super::StartFire();
	WeaponState = EWeaponState::Firing;
	FireAC = PlayWeaponSound(FireSound);
	Fire();
}

void AGrenadeWeapon::StopFire(){
	WeaponState = EWeaponState::Idle;
}

void AGrenadeWeapon::PlayFireEffects(FVector TraceEndPoint){
	if (MuzzleEffect)
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
}


void AGrenadeWeapon::ServerFire_Implementation(FVector ActorLoction, FVector_NetQuantizeNormal ACtorRotation){
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	ActorSpawnParams.Instigator = Cast<AMobileTPSCharacter>(GetOwner());
	ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(ASBulletClass, ActorLoction, ACtorRotation.Rotation(), ActorSpawnParams);
	if (Bullet) {
		Bullet->SetOwner(GetOwner());
	}
}

bool AGrenadeWeapon::ServerFire_Validate(FVector ActorLoction, FVector_NetQuantizeNormal ACtorRotation){
	return true;
}
