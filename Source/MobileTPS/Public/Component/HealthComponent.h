// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
UCLASS( ClassGroup=(TPS), meta=(BlueprintSpawnableComponent) )
class MOBILETPS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

public:
	float GetHealth() const;

	float GetDefaultHealth() const;
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
		void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:
	// Called when the game starts

	UPROPERTY(Replicated, BlueprintReadOnly,Category = HealthComponent)
		float Health;
	UPROPERTY(Replicated, EditAnywhere,BlueprintReadWrite, Category = HealthComponent)
		float DefaultHealth;
	bool bIsDead;
public:
	// Called every frame

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnHealthChangedSignature OnHealthChange;
}; 