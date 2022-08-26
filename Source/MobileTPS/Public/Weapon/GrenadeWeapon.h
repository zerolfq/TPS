// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/SWeapon.h"
#include "GrenadeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MOBILETPS_API AGrenadeWeapon : public ASWeapon
{
	GENERATED_BODY()
public:
	AGrenadeWeapon();
public:
	virtual void Fire() override;
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void PlayFireEffects(FVector TraceEndPoint) override;
public:
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire(FVector ActorLoction, FVector_NetQuantizeNormal ACtorRotation);
protected:
	UPROPERTY(EditAnywhere, Category = "Bullet")
		TSubclassOf<class ABullet> ASBulletClass;
};
