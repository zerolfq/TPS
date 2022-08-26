// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Equipping,
		OnGround,
		UnEquip
	};
}

USTRUCT()
struct FHitScanTrace {
	GENERATED_BODY()

public:
	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> SurfaceType;
	UPROPERTY()
		FVector_NetQuantize TraceTo;
};


UCLASS()
class MOBILETPS_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
		virtual void Fire();
		virtual void StartFire();
		virtual void StopFire();
		virtual void PlayFireEffects(FVector TraceEndPoint);
		virtual void PlayImpactEffects(EPhysicalSurface Surfacetype, FVector ImpactPoint);
		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void StopWeaponAnimation(UAnimMontage* WeaponAnimation);

	float PlayWeaponAnimation(UAnimMontage* WeaponAnimation);

	void OnEquip();

	void OnUnEquip();

	void OnEquipFinsh();

	void AttachMeshToPawn(FName SockeName);

	UAudioComponent* PlayWeaponSound(class USoundBase* Sound);
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
		float FireRate;
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
		float WeaponRate;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<UCameraShakeBase> FireCamShake;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		class UParticleSystem* MuzzleEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		class UParticleSystem* DefaultImpactEffect;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName MuzzleSocketName;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName TraceTargetName;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		UAnimMontage* EquipAnimotage;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		class USoundBase* EquipSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		class USoundBase* FireSound;
	UFUNCTION()
	virtual	void OnRep_HitScanTrace();


protected:
	class UAudioComponent* FireAC;
	float LastFire;
	EWeaponState::Type WeaponState;
	struct FTimerHandle TimeHandle_Fire;
	struct FTimerHandle TimerHandle_OnEquipFinished;
	float EquipStartedTime;
	float EquipDuration;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
		FHitScanTrace HitScanTrace;
};
