// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/SWeapon.h"
#include "RifleWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MOBILETPS_API ARifleWeapon : public ASWeapon
{
	GENERATED_BODY()
public:
	ARifleWeapon();
public:
	virtual void Fire() override;
	virtual void StartFire() override;
	virtual void StopFire() override;
	virtual void PlayFireEffects(FVector TraceEndPoint) override;
	virtual void PlayImpactEffects(EPhysicalSurface Surfacetype, FVector ImpactPoint);
public:
	UFUNCTION(Server,Reliable,WithValidation)
		void ServerStartFire();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStopFire();

public:
	void ChangeHUD();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		class UParticleSystem* TraceEffect;
	UPROPERTY(BlueprintReadOnly)
		float FireTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		class UParticleSystem* VulnerImpactEffect;
	float StartFireTime;
	UPROPERTY(EditAnywhere)
		float AimOffset;
	UPROPERTY(EditAnywhere)
		float InitOffset;

};
