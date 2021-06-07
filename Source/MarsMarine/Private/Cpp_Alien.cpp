// Fill out your copyright notice in the Description page of Project Settings.


#include "Cpp_Alien.h"
#include "Cpp_Marine.h"
#include "Cpp_MarsMarine_GameMode.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Kismet/GameplayStatics.h"
#include <Kismet/KismetMathLibrary.h>
#include <AIController.h>

// Sets default values
ACpp_Alien::ACpp_Alien()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACpp_Alien::BeginPlay()
{
	Super::BeginPlay();

	AttackRange = 150;
	DamagePerHit = 15.0;
	Health = 100;
	Dead = false;

	Montage = nullptr;

	DelegateMontageSuccess.BindUFunction(this, "OnMontageSuccess");

	FollowPlayer();
}

// Called every frame
void ACpp_Alien::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanAttack())
	{
		DeactivateAIMovement();
		USkeletalMeshComponent* MeshComp = GetMesh();
		if (IsValid(MeshComp) &&
			IsValid(Montage))
		{
			UAnimInstance* AnimComp = MeshComp->GetAnimInstance();
			if (IsValid(AnimComp))
			{
				AnimComp->Montage_SetEndDelegate(DelegateMontageSuccess, Montage);
				AnimComp->Montage_Play(Montage);
			}
		}
	}
}

void ACpp_Alien::OnMontageSuccess(UAnimMontage* MontageParam, bool bInterrupted)
{
	if (!bInterrupted && IsAlive())
	{
		ActivateAIMovement();
	}
}

// Called to bind functionality to input
void ACpp_Alien::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ACpp_Alien::IsAlive() const
{
	return !Dead;
}

bool ACpp_Alien::IsDead() const
{
	return Dead;
}

bool ACpp_Alien::CanAttack() const
{
	return (IsWithinAttackRange() && IsAlive() && !IsAttacking());
}

bool ACpp_Alien::IsAttacking() const
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (IsValid(MeshComp))
	{
		UAnimInstance* Anim = MeshComp->GetAnimInstance();
		if (IsValid(Anim) &&
			IsValid(Montage))
		{
			return Anim->Montage_IsPlaying(Montage);
		}
	}
	return false;
}

bool ACpp_Alien::IsWithinAttackRange() const
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (IsValid(Character))
	{
		ACpp_Marine* Marine = Cast<ACpp_Marine>(Character);
		if (IsValid(Marine))
		{
			if (Marine->IsAlive())
			{
				float Distance = GetDistanceTo(Marine);
				return (Distance < AttackRange);
			}
		}
	}
	return false;
}

void ACpp_Alien::DeactivateAIMovement()
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (IsValid(MoveComp))
	{
		MoveComp->StopMovementImmediately();
		MoveComp->Deactivate();
	}
}

void ACpp_Alien::ActivateAIMovement()
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (IsValid(MoveComp))
	{
		MoveComp->Activate();
	}
}

void ACpp_Alien::DamagePlayer()
{
	if (IsAlive())
	{
		ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		if (IsValid(Character))
		{
			UGameplayStatics::ApplyDamage(Character, DamagePerHit, nullptr, nullptr, nullptr);
		}
	}
}

float ACpp_Alien::DecreaseHealth(float Damage)
{
	Health -= Damage;
	Health = UKismetMathLibrary::Max(0, Health);
	if (IsValid(EnemyHurtSound))
	{
		UGameplayStatics::SpawnSound2D(GetWorld(), EnemyHurtSound);
	}
	return Health;
}

void ACpp_Alien::KillAI()
{
	Dead = true;
	DeactivateAIMovement();
	SetActorEnableCollision(false);
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (IsValid(Character))
	{
		ACpp_Marine* Marine = Cast<ACpp_Marine>(Character);
		if (IsValid(Marine))
		{
			Marine->IncreaseKills();
			AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(GetWorld());
			if (IsValid(BaseGameMode))
			{
				ACpp_MarsMarine_GameMode* GameMode = Cast<ACpp_MarsMarine_GameMode>(BaseGameMode);
				if (IsValid(GameMode))
				{
					GameMode->RegisterAlienDeath(this);
				}
			}
		}
	}
}

float ACpp_Alien::GetHealth() const
{
	return Health;
}

UFUNCTION(BlueprintCallable)
void ACpp_Alien::FollowPlayer()
{
	if (IsAlive())
	{
		AController* BaseController = GetController();
		if (IsValid(BaseController))
		{
			AAIController* AI = Cast<AAIController>(BaseController);
			if (IsValid(AI))
			{
				ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
				if (IsValid(Character))
				{
					AI->MoveToActor(Character);

					FTimerDelegate TimerDel;
					FTimerHandle TimerHandle;

					TimerDel.BindUFunction(this, FName("FollowPlayer"));
					GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, 2.0f, false);
				}
			}
		}
	}
}
