// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Cpp_Alien.generated.h"

UCLASS()
class MARSMARINE_API ACpp_Alien : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACpp_Alien();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	
	class ACpp_Marine* GetNearestPlayer() const;


	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintCallable)
	bool CanAttack() const;

	UFUNCTION(BlueprintCallable)
	bool IsAttacking() const;

	UFUNCTION(BlueprintCallable)
	bool IsWithinAttackRange() const;

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	void DeactivateAIMovement();

	UFUNCTION(BlueprintCallable)
	void ActivateAIMovement();

	UFUNCTION(BlueprintCallable)
	void DamagePlayer();

	UFUNCTION(BlueprintCallable)
	float DecreaseHealth(float Damage);

	UFUNCTION(BlueprintCallable)
	void KillAI();

	UFUNCTION(BlueprintCallable)
	void DestroyActor();

	UFUNCTION(BlueprintCallable)
	void FollowPlayer();

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	USoundBase* EnemyHurtSound;


	UPROPERTY(VisibleAnywhere, Category = "Enemy Properties")
	bool Dead;

	UPROPERTY(VisibleAnywhere, Category = "Enemy Properties")
	float Health;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Properties")
	float DamagePerHit;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Properties")
	float AttackRange;


	FOnMontageEnded DelegateMontageSuccess;
	
	UFUNCTION()
	void OnMontageSuccess(UAnimMontage* MontageParam, bool bInterrupted);
};
