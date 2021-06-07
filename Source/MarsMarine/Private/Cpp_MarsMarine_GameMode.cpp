// Fill out your copyright notice in the Description page of Project Settings.


#include "Cpp_MarsMarine_GameMode.h"
#include <Engine/TargetPoint.h>
#include <GameFramework/Character.h>
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>

void ACpp_MarsMarine_GameMode::BeginPlay()
{
	AlienSpawnRemaining = 5;
	WaveDifficulty = 5;
	DifficultyMultiplier = 1.5;
	SpawnFrequency = 0.25f;
	CurrentWave = 0;

	Super::BeginPlay();

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	TimerDel.BindUFunction(this, FName("StartNewWave"));
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, 2.0f, false);
}

void ACpp_MarsMarine_GameMode::SpawnWave()
{
}

void ACpp_MarsMarine_GameMode::EndWave()
{
	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	TimerDel.BindUFunction(this, FName("StartNewWave"));
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, 2.0f, false);
}

bool ACpp_MarsMarine_GameMode::IsWaveComplete() const
{
	return (Aliens.Num() == 0 && AlienSpawnRemaining == 0);
}

void ACpp_MarsMarine_GameMode::RegisterAlienDeath(ACharacter* Alien)
{
	Aliens.Remove(Alien);
	if (IsWaveComplete())
	{
		EndWave();
	}
}

ATargetPoint* ACpp_MarsMarine_GameMode::GetRandomSpawnPoint() const
{
	TArray<AActor*> OutActors;
	TSubclassOf<ATargetPoint> TargetPointClass;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), OutActors);
	if (OutActors.Num() == 0)
	{
		return nullptr;
	}
	int32 Index = UKismetMathLibrary::RandomIntegerInRange(0, OutActors.Num() - 1);
	return Cast<ATargetPoint>(OutActors[Index]);
}

UFUNCTION(BlueprintCallable)
void ACpp_MarsMarine_GameMode::SpawnAlien(TSubclassOf<AActor> ActorToSpawn)
{
	ATargetPoint* SpawnPoint = GetRandomSpawnPoint();
	if (IsValid(SpawnPoint))
	{
		const FTransform& Transform = SpawnPoint->GetActorTransform();

		FActorSpawnParameters SpawnParameters;
		AActor* ActorSpawn = GetWorld()->SpawnActor<AActor>(ActorToSpawn, Transform, SpawnParameters);
		ACharacter* CharacterSpawn = Cast<ACharacter>(ActorSpawn);
		if (IsValid(CharacterSpawn))
		{
			Aliens.Add(CharacterSpawn);
			AlienSpawnRemaining--;
			if (AlienSpawnRemaining <= 0)
			{
				UKismetSystemLibrary::K2_ClearTimer(this, "Spawn Wave");
			}
		}
	}
}

void ACpp_MarsMarine_GameMode::IncreaseWaveDifficulty()
{
	WaveDifficulty = UKismetMathLibrary::Round(WaveDifficulty * DifficultyMultiplier);

	AlienSpawnRemaining = WaveDifficulty;
}

void ACpp_MarsMarine_GameMode::StartNewWave()
{
	++CurrentWave;

	IncreaseWaveDifficulty();

	UKismetSystemLibrary::K2_SetTimer(this, "Spawn Wave", SpawnFrequency, true);
}

int32 ACpp_MarsMarine_GameMode::GetRemainingEnemies() const
{
	return Aliens.Num();
}

int32 ACpp_MarsMarine_GameMode::GetCurrentWave() const
{
	return CurrentWave;
}
