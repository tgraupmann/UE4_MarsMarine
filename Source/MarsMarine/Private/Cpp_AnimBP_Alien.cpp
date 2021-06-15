// Fill out your copyright notice in the Description page of Project Settings.


#include "Cpp_AnimBP_Alien.h"
#include "Cpp_Alien.h"
#include <Kismet/KismetMathLibrary.h>


void UCpp_AnimBP_Alien::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	APawn* Owner = TryGetPawnOwner();
	if (IsValid(Owner))
	{
		Alien = Cast<ACpp_Alien>(Owner);
	}
}

void UCpp_AnimBP_Alien::NativeUpdateAnimation(float DeltaSeconds)
{
	if (IsValid(Alien))
	{
		MovementSpeed = CalculateMovementSpeed();
		Dead = Alien->IsDead();
	}
}

ACpp_Alien* UCpp_AnimBP_Alien::GetAlien() const
{
	return Alien;
}

float UCpp_AnimBP_Alien::CalculateMovementSpeed() const
{
	if (IsValid(Alien))
	{
		FVector Velocity = Alien->GetVelocity();
		return UKismetMathLibrary::VSize(Velocity);
	}
	return 0;
}

void UCpp_AnimBP_Alien::OnAnimNotify_Attack()
{
	if (IsValid(Alien))
	{
		if (Alien->IsWithinAttackRange())
		{
			Alien->DamagePlayer();
		}
	}
}
