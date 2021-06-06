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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	UFUNCTION(BlueprintCallable)
	bool IsHealthFull() const;

	UFUNCTION(BlueprintCallable)
	float AddHealth(float HealthToAdd);

	UFUNCTION(BlueprintCallable)
	float DecreaseHealth(float DamageAmount);

	UFUNCTION(BlueprintCallable)
	void IncreaseKills();

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Properties")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Properties")
	int32 Kills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Properties")
	bool Dead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float WeaponRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float DamagePerShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float WeaponFireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool OutsideMissionArea;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	USoundBase* PlayerHurtSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Particles")
	UParticleSystem* ParticleCharacterImpact;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Particles")
	UParticleSystem* ParticleNonCharacterImpact;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Particles")
	UNiagaraSystem* ParticleTracerFire;

	UActorComponent* CompWeaponMount;
};
