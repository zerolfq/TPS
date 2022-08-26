// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Bullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
ABullet::ABullet(){
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");	// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	DamageRadial = 300.0f;
	BaseDamage = 40.0f;
	DefaultForce = 300.0f;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void ABullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){
	if (OtherActor == GetOwner()) {
		return;
	}
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics()) {
		float RandomIntensity = FMath::RandRange(200.0f, 500.0f);
		UE_LOG(LogTemp,Warning,TEXT("on Hit "));
		OtherComp->AddImpulseAtLocation(GetVelocity() * RandomIntensity, GetActorLocation());

	}
	if (GetLocalRole() == ROLE_Authority) {
		MakeNoise(1.0f, GetInstigator());
		Explode();
	}
}

void ABullet::Explode(){
	MakeDamage(GetActorLocation());
	Destroy();
}

void ABullet::MakeDamage(const FVector DamagePoint){
	TArray<AActor*> ignoredActors;
	UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, DamagePoint, DamageRadial, UDamageType::StaticClass(), ignoredActors, this, GetInstigator()->GetController());

	FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(ApplyRadialDamage), false, this);
	TArray<FOverlapResult> Overlaps;
	UWorld* World = GetWorld();
	World->OverlapMultiByObjectType(Overlaps, DamagePoint, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(DamageRadial), SphereParams);

	for (FOverlapResult Overlap : Overlaps) {
		AActor* const OverlapActor = Overlap.GetActor();

		if (OverlapActor && OverlapActor != this && Overlap.Component.IsValid()) {
			UPrimitiveComponent* OtherComp = Overlap.GetComponent();
			if (OtherComp->IsSimulatingPhysics()) {
				float dist = (GetActorLocation() - OtherComp->GetComponentLocation()).Size();
				float RandomIntensity = FMath::Clamp(dist / DamageRadial * DefaultForce,0.0f,DefaultForce);
				OtherComp->AddImpulseAtLocation(GetVelocity() * RandomIntensity, GetActorLocation());
			}
		}
	}
}

// Called when the game starts or when spawned
void ABullet::BeginPlay(){
	Super::BeginPlay();

	CollisionComp->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABullet::Explode, 3.0f, false);
}

void ABullet::Destroyed(){
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, GetActorLocation(), FRotator::ZeroRotator, FVector(5.0f));
}

