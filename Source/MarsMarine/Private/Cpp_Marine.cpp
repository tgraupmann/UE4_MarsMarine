// Fill out your copyright notice in the Description page of Project Settings.


#include "Cpp_Marine.h"
#include "Kismet/GameplayStatics.h"
#include <Kismet/KismetMathLibrary.h>
#include <Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraComponent.h>

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
}

// Called when the game starts or when spawned
void ACpp_Marine::BeginPlay()
{
	Super::BeginPlay();

	CompWeaponMount = GetComponentByClass(UParticleSystemComponent::StaticClass()); // Gets: P_AssaultRifle_MF
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
		UGameplayStatics::PlaySound2D(GetWorld(), PlayerHurtSound);
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
	return MeshComp->GetSocketLocation("MuzzleFlash");
}

FVector ACpp_Marine::GetWeaponTraceEndLocation() const
{
	FVector Loc = GetWeaponTraceStartLocation();
	FVector Forward = UKismetMathLibrary::GetForwardVector(GetControlRotation());
	return Loc + (Forward * WeaponRange);
}

void ACpp_Marine::SpawnTracerFire(FVector Loc) const
{
	if (IsValid(CompWeaponMount))
	{
		USceneComponent* SceneComp = Cast<USceneComponent>(CompWeaponMount);
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
			if (ParticleCharacterImpact != nullptr)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleCharacterImpact, HitLocation);
			}
		}
		else
		{
			if (ParticleNonCharacterImpact != nullptr)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleNonCharacterImpact, HitLocation);
			}
		}
	}
}

void ACpp_Marine::WeaponTrace()
{
	FVector Start = GetWeaponTraceStartLocation();
	FVector End = GetWeaponTraceEndLocation();
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End,
		ETraceTypeQuery::TraceTypeQuery1, //visibility
		false, ActorsToIgnore,
		EDrawDebugTrace::Type::None,
		//EDrawDebugTrace::Type::ForDuration,
		OutHit, true,
		FLinearColor::Red, FLinearColor::Green, 1.0f))
	{
		if (OutHit.Actor.IsValid())
		{
			SpawnImpactHit(OutHit.Actor.Get(), OutHit.Location);
			UGameplayStatics::ApplyDamage(OutHit.Actor.Get(), DamagePerShot, nullptr, nullptr, nullptr);
		}
	}
}
