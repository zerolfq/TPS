// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/HealthComponent.h"
#include "MobileTPS/MobileTPSGameMode.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	DefaultHealth = 100.0f;
	bIsDead = false;
	TeamNum = 255;
	SetIsReplicatedByDefault(true);
	// ...
}


float UHealthComponent::GetHealth() const{
	return Health;
}

float UHealthComponent::GetDefaultHealth() const{
	return DefaultHealth;
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...与服务器相关联
	if (GetOwnerRole() == ROLE_Authority) {
		AActor* MyOwner = GetOwner();
		if (MyOwner) {
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
		}
	}
	Health = DefaultHealth;
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, Health);
	
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser){
	if (Damage <= 0.0f || bIsDead) return;
	if (DamagedActor != InstigatedBy->GetPawn() && IsFriendly(DamagedActor, InstigatedBy->GetPawn())) return;
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health Changed:%f %d"), Health,TeamNum);

	OnHealthChange.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
	bIsDead = Health <= 0.0f;
	if (bIsDead) {
		AMobileTPSGameMode* GM = Cast<AMobileTPSGameMode>(GetWorld()->GetAuthGameMode());
		if (GM) {
			GM->OnActorKilled.Broadcast(GetOwner(),DamageCauser, InstigatedBy);
		}
	}
}

bool UHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB){
	if (ActorB == nullptr || ActorA == nullptr) return false;
	UHealthComponent* HealthCompA = Cast<UHealthComponent>(ActorA->GetComponentByClass(UHealthComponent::StaticClass()));
	UHealthComponent* HealthCompB = Cast<UHealthComponent>(ActorB->GetComponentByClass(UHealthComponent::StaticClass()));
	if (HealthCompA == nullptr || HealthCompB == nullptr) return true;
	if (HealthCompA->TeamNum == HealthCompB->TeamNum) return true;
	else return false;	
	
}

