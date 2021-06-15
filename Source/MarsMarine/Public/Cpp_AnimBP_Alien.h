// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Cpp_AnimBP_Alien.generated.h"

/**
 * 
 */
UCLASS()
class MARSMARINE_API UCpp_AnimBP_Alien : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	class ACpp_Alien* GetAlien() const;

	UFUNCTION(BlueprintCallable)
	float CalculateMovementSpeed() const;

	UFUNCTION(BlueprintCallable)
	void OnAnimNotify_Attack();


	UPROPERTY(BlueprintReadOnly)
	bool Dead;

	UPROPERTY(BlueprintReadOnly)
	float MovementSpeed;


	class ACpp_Alien* Alien;
	
};
