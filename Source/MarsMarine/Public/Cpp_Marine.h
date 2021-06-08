// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Cpp_Marine.generated.h"

class UNiagaraSystem;


UCLASS()
class MARSMARINE_API ACpp_Marine : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACpp_Marine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Input
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void ThumbstickMoveForward(float AxisValue);
	void ThumbstickMoveRight(float AxisValue);
	void ThumbstickAimUp(float AxisValue);
	void ThumbstickAimRight(float AxisValue);
	void AnyKeyPressed(FKey Key);
	void AnyKeyReleased(FKey Key);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
	void IncreaseKills();

protected:

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	
	UFUNCTION(BlueprintCallable, Category = "Mouse Input")
	FVector2D GetMouseVelocity() const;

	UFUNCTION(BlueprintCallable, Category = "Mouse Input")
	bool IsMouseAboveDeadzone() const;

	UFUNCTION(BlueprintCallable, Category = "Mouse Input")
	FRotator GetMouseAimDirection() const;

	UFUNCTION(BlueprintCallable, Category = "Mouse Input")
	void UpdateMouseAim();


	UFUNCTION(BlueprintCallable, Category = "Thumbstick Input")
	bool IsThumbstickAboveDeadzone() const;

	UFUNCTION(BlueprintCallable, Category = "Thumbstick Input")
	FRotator GetThumbstickAimDirection() const;

	UFUNCTION(BlueprintCallable, Category = "Thumbstick Input")
	void UpdateThumbstickAim();

	
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerAim();


	UFUNCTION(BlueprintCallable)
	bool IsHealthFull() const;

	UFUNCTION(BlueprintCallable)
	float AddHealth(float HealthToAdd);

	UFUNCTION(BlueprintCallable)
	float DecreaseHealth(float DamageAmount);

	UFUNCTION(BlueprintCallable)
	FVector GetWeaponTraceStartLocation() const;

	UFUNCTION(BlueprintCallable)
	FVector GetWeaponTraceEndLocation() const;

	UFUNCTION(BlueprintCallable)
	void SpawnTracerFire(FVector Loc) const;

	UFUNCTION(BlueprintCallable)
	void SpawnImpactHit(AActor* HitActor, FVector HitLocation) const;

	UFUNCTION(BlueprintCallable)
	void WeaponTrace();

	UFUNCTION(BlueprintCallable)
	void StartFiringWeapon();

	UFUNCTION(BlueprintCallable)
	void StopFiringWeapon();

	UFUNCTION(BlueprintCallable)
	void Respawn();

	UFUNCTION(BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintCallable)
	int32 GetKills() const;

	UFUNCTION(BlueprintCallable)
	bool IsOutsideMissionArea() const;

	UFUNCTION(BlueprintCallable)
	void SetOutsideMissionArea(bool Outside);

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	void SetCompMuzzleFlash(UParticleSystemComponent* Comp);


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	class TSubclassOf<UUserWidget> HUDClass;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Input")
	float MouseDeadzone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Input")
	float MouseSmoothingStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thumbstick Input")
	float ThumbstickDeadzone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thumbstick Input")
	bool GamepadActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thumbstick Input")
	float ThumbstickSmoothingStrength;


	UPROPERTY(EditDefaultsOnly, Category = "Player Properties")
	float Health;

	UPROPERTY(VisibleAnywhere, Category = "Player Properties")
	int32 Kills;

	UPROPERTY(VisibleAnywhere, Category = "Player Properties")
	bool Dead;

	UPROPERTY(EditDefaultsOnly, Category = "Player Properties")
	float WeaponRange;

	UPROPERTY(EditDefaultsOnly, Category = "Player Properties")
	float DamagePerShot;

	UPROPERTY(EditDefaultsOnly, Category = "Player Properties")
	float WeaponFireRate;

	UPROPERTY(VisibleAnywhere, Category = "Player Properties")
	bool OutsideMissionArea;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Properties")
	FVector WeaponTraceOffset;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	USoundBase* PlayerHurtSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	USoundBase* RifleFireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	USoundBase* RifleEndSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Particles")
	UParticleSystem* ParticleCharacterImpact;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Particles")
	UParticleSystem* ParticleNonCharacterImpact;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Particles")
	UNiagaraSystem* ParticleTracerFire;

	UParticleSystemComponent* CompMuzzleFlash;

	UAudioComponent* WeaponFireSound;

	FTimerHandle TimerHandleWeaponTrace;

	float AxisThumbstickAimUp;
	float AxisThumbstickAimRight;
};
