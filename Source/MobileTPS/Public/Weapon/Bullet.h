// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class MOBILETPS_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

public:
	UFUNCTION(BlueprintCallable)
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void Explode();
public:
	void MakeDamage(const FVector DamagePoint);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
protected:
	UPROPERTY(VisibleAnywhere, Category = "Damage")
		float DamageRadial;

	UPROPERTY(VisibleAnywhere, Category = "Damage")
		float DefaultForce;

	UPROPERTY(VisibleAnywhere, Category = "Damage")
		float BaseDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		class UParticleSystem* ExplosionFX;


	
};
