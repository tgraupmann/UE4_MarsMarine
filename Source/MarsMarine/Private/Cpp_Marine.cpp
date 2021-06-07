// Fill out your copyright notice in the Description page of Project Settings.


#include "Cpp_Marine.h"
#include "Kismet/GameplayStatics.h"
#include <Kismet/KismetMathLibrary.h>
#include <Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraComponent.h>
#include <Components/AudioComponent.h>

// Sets default values
ACpp_Marine::ACpp_Marine()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MouseDeadzone = 20.0f;
	MouseSmoothingStrength = 7.0f;
	ThumbstickDeadzone = 0.5;
	GamepadActive = false;
	ThumbstickSmoothingStrength = 7.0f;
	Health = 100.0f;
	Kills = 0;
	Dead = false;
	WeaponRange = 5000.0f;
	DamagePerShot = 25.0f;
	WeaponFireRate = 0.08f;
	OutsideMissionArea = false;

	WeaponTraceOffset = FVector(0, 0, 50);

	PlayerHurtSound = nullptr;
	RifleFireSound = nullptr;
	RifleEndSound = nullptr;

	CompMuzzleFlash = nullptr;
	WeaponFireSound = nullptr;
}

// Called when the game starts or when spawned
void ACpp_Marine::BeginPlay()
{
	Super::BeginPlay();

	UActorComponent* CompSearch = GetComponentByClass(UParticleSystemComponent::StaticClass()); // Gets: P_AssaultRifle_MF
	if (IsValid(CompSearch))
	{
		CompMuzzleFlash = Cast<UParticleSystemComponent>(CompSearch);
	}
}

// Called every frame
void ACpp_Marine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACpp_Marine::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ACpp_Marine::IsHealthFull() const
{
	return (Health >= 100.0f);
}

float ACpp_Marine::AddHealth(float HealthToAdd)
{
	Health += HealthToAdd;
	Health = UKismetMathLibrary::Max(0, Health);
	return Health;
}

UFUNCTION(BlueprintCallable)
float ACpp_Marine::DecreaseHealth(float DamageAmount)
{
	Health -= DamageAmount;
	Health = UKismetMathLibrary::Max(0, Health);

	if (IsValid(PlayerHurtSound))
	{
		UGameplayStatics::SpawnSound2D(GetWorld(), PlayerHurtSound);
	}

	return Health;
}

void ACpp_Marine::IncreaseKills()
{
	++Kills;
}

bool ACpp_Marine::IsAlive() const
{
	return !Dead;
}

FVector ACpp_Marine::GetWeaponTraceStartLocation() const
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	return MeshComp->GetSocketLocation("MuzzleFlash") + WeaponTraceOffset;
}

FVector ACpp_Marine::GetWeaponTraceEndLocation() const
{
	FVector Loc = GetWeaponTraceStartLocation();
	FVector Forward = UKismetMathLibrary::GetForwardVector(GetControlRotation());
	return Loc + (Forward * WeaponRange);
}

void ACpp_Marine::SpawnTracerFire(FVector Loc) const
{
	if (IsValid(CompMuzzleFlash))
	{
		USceneComponent* SceneComp = Cast<USceneComponent>(CompMuzzleFlash);
		if (IsValid(SceneComp))
		{
			UNiagaraComponent* Effect = UNiagaraFunctionLibrary::SpawnSystemAttached(ParticleTracerFire, SceneComp,
				FName("None"),
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::Type::KeepRelativeOffset,
				true,
				true
			);
			if (IsValid(Effect))
			{
				Effect->SetVectorParameter(FName("BeamEnd"), Loc);
			}
		}
	}
}

void ACpp_Marine::SpawnImpactHit(AActor* HitActor, FVector HitLocation) const
{
	if (IsValid(HitActor))
	{
		SpawnTracerFire(HitLocation);
		ACharacter* Character = Cast<ACharacter>(HitActor);
		if (IsValid(Character))
		{
			if (IsValid(ParticleCharacterImpact))
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleCharacterImpact, HitLocation);
			}
		}
		else
		{
			if (IsValid(ParticleNonCharacterImpact))
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleNonCharacterImpact, HitLocation);
			}
		}
	}
}

void ACpp_Marine::WeaponTrace()
{
	/*
	FVector Start = GetWeaponTraceStartLocation();
	FVector End = GetWeaponTraceEndLocation();
	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility, Params))
	*/


	FVector Start = GetWeaponTraceStartLocation();
	FVector End = GetWeaponTraceEndLocation();
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
		false, ActorsToIgnore,
		EDrawDebugTrace::Type::None,
		//EDrawDebugTrace::Type::ForDuration,
		OutHit, true
		//, FLinearColor::Red, FLinearColor::Green, 1.0f
	))
	{
		if (OutHit.bBlockingHit)
		{
			if (OutHit.Actor.IsValid())
			{
				SpawnImpactHit(OutHit.Actor.Get(), OutHit.Location);
				UGameplayStatics::ApplyDamage(OutHit.Actor.Get(), DamagePerShot, nullptr, nullptr, nullptr);
			}
		}
	}
}

void ACpp_Marine::StartFiringWeapon()
{
	if (IsAlive())
	{
		if (IsValid(CompMuzzleFlash))
		{
			CompMuzzleFlash->Activate();
		}

		if (!IsValid(WeaponFireSound))
		{
			WeaponFireSound = UGameplayStatics::SpawnSound2D(GetWorld(), RifleFireSound);

			WeaponTrace();
			
			if (!TimerHandleWeaponTrace.IsValid())
			{
				FTimerDelegate TimerDel;
				TimerDel.BindUFunction(this, "WeaponTrace");
				GetWorldTimerManager().SetTimer(TimerHandleWeaponTrace, TimerDel, WeaponFireRate, true);
			}
		}
	}
}

void ACpp_Marine::StopFiringWeapon()
{
	if (IsValid(CompMuzzleFlash))
	{
		CompMuzzleFlash->Deactivate();
	}

	GetWorldTimerManager().ClearTimer(TimerHandleWeaponTrace);
	TimerHandleWeaponTrace.Invalidate();

	if (IsValid(WeaponFireSound))
	{
		WeaponFireSound->Stop();
		WeaponFireSound = nullptr;

		UGameplayStatics::SpawnSound2D(GetWorld(), RifleEndSound);
	}
}

float ACpp_Marine::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float Result = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (IsAlive())
	{
		if (DecreaseHealth(DamageAmount) <= 0.0f)
		{
			Dead = true;
			StopFiringWeapon();

			FTimerDelegate TimerDel;
			FTimerHandle TimerHandle;

			TimerDel.BindUFunction(this, FName("Respawn"));
			GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, 5.0f, false);
		}
	}

	return Result;
}

void ACpp_Marine::Respawn()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("Mars"));
}

bool ACpp_Marine::IsDead() const
{
	return Dead;
}

int32 ACpp_Marine::GetKills() const
{
	return Kills;
}

bool ACpp_Marine::IsOutsideMissionArea() const
{
	return OutsideMissionArea;
}

void ACpp_Marine::SetOutsideMissionArea(bool Outside)
{
	OutsideMissionArea = Outside;
}

float ACpp_Marine::GetHealth() const
{
	return Health;
}
