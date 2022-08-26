// Copyright Epic Games, Inc. All Rights Reserved.

#include "MobileTPSCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Weapon/SWeapon.h"
#include "Net/UnrealNetwork.h"
#include "MobileTPS.h"
#include "Component/HealthComponent.h"
#include "GameBase/MobileTPSPlayerController.h"

//////////////////////////////////////////////////////////////////////////
// AMobileTPSCharacter

AMobileTPSCharacter::AMobileTPSCharacter()
{
	// Set size for collision capsule
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = true;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	SprintSpeed = 1000.0f;
	WalkSpeed = 600.0f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bUseControllerRotationYaw = true;
	ZoomedFOV = 65;
	ZoomInterpSpeed = 20.0f;
	CurWeapon = nullptr;
	OwnWeapon.Add(nullptr);
	WeaponIndex = 0;
	IsDied = false;
	bIsAI = false;
	ClientHealth = HealthComp->GetDefaultHealth();

	EquipSockeName = "WeaponPoint";
	BackSockeName = "WeaponBackPoint";

}

void AMobileTPSCharacter::MoveForward(float Value) {
	AddMovementInput(GetActorForwardVector() * Value);
}

void AMobileTPSCharacter::MoveRight(float Value) {
	AddMovementInput(GetActorRightVector() * Value);
}

void AMobileTPSCharacter::BeginCrouch() {
	Crouch();
}

void AMobileTPSCharacter::EndCrouch() {
	UnCrouch();
}

void AMobileTPSCharacter::BeginSprint() {
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AMobileTPSCharacter::EndSprint() {
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}


void AMobileTPSCharacter::StartFire(){
	if (CurWeapon)	CurWeapon->StartFire();
}


void AMobileTPSCharacter::EndFire(){
	if (CurWeapon) CurWeapon->StopFire();
}


bool AMobileTPSCharacter::IsCurWeapon(){
	if (CurWeapon == nullptr) return false;
	return true;
}

void AMobileTPSCharacter::BeginZoom() {
	bWantsToZoom = true;
	//CameraComp->SetFieldOfView(ZoomedFOV);
}

void AMobileTPSCharacter::EndZoom() {
	bWantsToZoom = false;
	/*CameraComp->SetFieldOfView(DefaultFOV);*/
}


void AMobileTPSCharacter::NextWeapon(){
	if (GetLocalRole() < ROLE_Authority) {
		ServerNextWeapon();
	}
	if (OwnWeapon.Num() == 0) return;
	WeaponIndex = (WeaponIndex + 1) % OwnWeapon.Num();
	UE_LOG(LogTemp,Warning,TEXT("%d"), OwnWeapon.Num());
	ASWeapon* NextWeapon = OwnWeapon[WeaponIndex];
	ChangeWeapon(NextWeapon);
}

bool AMobileTPSCharacter::IsEquipWeapon(){
	return (CurWeapon != nullptr);
}


void AMobileTPSCharacter::OnHealthChanged(class UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser){
	if (HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("This is Server"));
		ClientHealth = Health;
		OnRep_HealthChange();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("This is Client"));
	}
	if (Health <= 0.0f && !IsDied) {
		GetCharacterMovement()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		IsDied = true;
		for (ASWeapon* Weapon : OwnWeapon) {
			if (Weapon) Weapon->Destroy();
		}
		OwnWeapon.Empty();
		FTimerHandle FControlTime;
		GetWorldTimerManager().SetTimer(FControlTime, this, &AMobileTPSCharacter::DetachFromControllerPendingDestroy, 2.0, false);
		SetLifeSpan(10.0);
	}
}


void AMobileTPSCharacter::OnRep_HealthChange(){
	AController* APC = this->GetController();
	AMobileTPSPlayerController* PC = Cast<AMobileTPSPlayerController>(APC);
	if (PC && PC->IsLocalController()) {
		PC->ChangeHUDHP(HealthComp->GetHealth() / HealthComp->GetDefaultHealth());
	}
}

void AMobileTPSCharacter::ChangeWeapon(class ASWeapon* NextWeapon){
	if (CurWeapon != nullptr) {
		CurWeapon->OnUnEquip();
	}
	if (NextWeapon) {
		NextWeapon->OnEquip();
	}
	CurWeapon = NextWeapon;
	UE_LOG(LogTemp, Warning, TEXT("IsWeapon %d"), CurWeapon != nullptr);
}


FName AMobileTPSCharacter::GetBackSockeName(){
	return BackSockeName;
}


FName AMobileTPSCharacter::GetEquipSockeName(){
	return EquipSockeName;
}


void AMobileTPSCharacter::SetCurWeapon(class ASWeapon* NowWeapon){
	CurWeapon = NowWeapon;
}


void AMobileTPSCharacter::ServerNextWeapon_Implementation(){
	NextWeapon();
}


bool AMobileTPSCharacter::ServerNextWeapon_Validate(){
	return true;
}

bool AMobileTPSCharacter::IsRunning() const{
	if (!GetCharacterMovement())
	{
		return false;
	}

	return (!GetVelocity().IsZero() && (GetVelocity().GetSafeNormal2D() | GetActorForwardVector()) > -0.1);
}


FRotator AMobileTPSCharacter::GetAimOffsets() const{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS); // 父空间转局部空间坐标
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

void AMobileTPSCharacter::BeginPlay(){
	Super::BeginPlay();
	if (bIsAI) {
		if (GetLocalRole() == ROLE_Authority) {
			UseAIBehaviorTree();
		}
	}
	
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	DefaultFOV = CameraComp->FieldOfView;

	if (HasAuthority()) {
		for (int i = 0; i < PrimaryWeaponClass.Num(); i++) {
			FVector WeaponLocation = GetMesh()->GetSocketLocation(BackSockeName);
			FRotator WeaponROtation = GetMesh()->GetSocketRotation(BackSockeName);

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ActorSpawnParams.Instigator = this;
			ActorSpawnParams.Owner = this;

			ASWeapon* Weapon = GetWorld()->SpawnActor<ASWeapon>(PrimaryWeaponClass[i], WeaponLocation, WeaponROtation, ActorSpawnParams);
			if (Weapon) {
				FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
				Weapon->AttachToComponent(GetMesh(), Rules, BackSockeName);
				OwnWeapon.Add(Weapon);
			}
		}
	}
	HealthComp->OnHealthChange.AddDynamic(this,&AMobileTPSCharacter::OnHealthChanged);
}

void AMobileTPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;


	float CurrentFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	CameraComp->SetFieldOfView(CurrentFOV);
}

void AMobileTPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMobileTPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMobileTPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AMobileTPSCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AMobileTPSCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMobileTPSCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMobileTPSCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMobileTPSCharacter::BeginSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMobileTPSCharacter::EndSprint);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("LeftMouse", IE_Pressed, this, &AMobileTPSCharacter::StartFire);
	PlayerInputComponent->BindAction("LeftMouse", IE_Released, this, &AMobileTPSCharacter::EndFire);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AMobileTPSCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AMobileTPSCharacter::EndZoom);

	PlayerInputComponent->BindAction("NextWeapon",IE_Pressed,this,&AMobileTPSCharacter::NextWeapon);
}

void AMobileTPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMobileTPSCharacter,CurWeapon);
	DOREPLIFETIME(AMobileTPSCharacter, OwnWeapon);
	DOREPLIFETIME(AMobileTPSCharacter, IsDied);
	DOREPLIFETIME_CONDITION(AMobileTPSCharacter, ClientHealth, COND_OwnerOnly);
}
