// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MobileTPSCharacter.generated.h"

UCLASS(config=Game)
class AMobileTPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMobileTPSCharacter();

public:

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	//将角色移动速度设置为冲刺值。
	void BeginSprint();

	//将角色移动速度设置回默认速度值。
	void EndSprint();


	UFUNCTION(BlueprintCallable,Category = Fire)
	void StartFire();
	UFUNCTION(BlueprintCallable, Category = Fire)
	void EndFire();
	UFUNCTION(BlueprintCallable)
	bool IsCurWeapon();

	void BeginZoom();

	void EndZoom();

	UFUNCTION(BlueprintCallable, Category = Fire)
	void NextWeapon();

	void ChangeWeapon(class ASWeapon* NextWeapon);

	FName GetBackSockeName();

	FName GetEquipSockeName();

	void SetCurWeapon(class ASWeapon* NowWeapon);

	class UCameraComponent* GetCamera() {return CameraComp;}
public:
	//UFUNCTION(Server,Reliable,WithValidation)
	//	void ServerStartFire();
	//UFUNCTION(Server, Reliable, WithValidation)
	//	void ServerStopFire();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerNextWeapon();
public:
	UFUNCTION(BlueprintCallable, Category = Pawn)
		bool IsRunning() const;
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		FRotator GetAimOffsets() const;
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		bool IsEquipWeapon();
	UFUNCTION()
		void OnHealthChanged(class UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
		void UseAIBehaviorTree();
	UFUNCTION()
		void OnRep_HealthChange();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Speed")
		float SprintSpeed;

	UPROPERTY(EditAnywhere, Category = "Speed")
		float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
		class UAnimSequence* FireAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
		class UHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
		float ZoomInterpSpeed;

	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = "Weapeon")
		TArray<TSubclassOf<class ASWeapon>> PrimaryWeaponClass;

	UPROPERTY(EditAnywhere, Category = "Weapon")
		FName EquipSockeName;

	UPROPERTY(EditAnywhere, Category = "Weapon")
		FName BackSockeName;
	UPROPERTY(Replicated,BlueprintReadOnly, Category = Player)
		bool IsDied;
	UPROPERTY(EditAnywhere, Category = AI)
		bool bIsAI;

	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UPROPERTY(Replicated)
	TArray<class ASWeapon*> OwnWeapon;

	UPROPERTY(Replicated)
	ASWeapon* CurWeapon;

	int WeaponIndex;

	bool bWantsToZoom;
	UPROPERTY(ReplicatedUsing = OnRep_HealthChange)
	int ClientHealth;

};

