// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Cpp_MarsMarine_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class MARSMARINE_API ACpp_MarsMarine_GameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

protected:

	UFUNCTION(BlueprintCallable)
	void SpawnWave();

	UFUNCTION(BlueprintCallable)
	void EndWave();

	UFUNCTION(BlueprintCallable)
	bool IsWaveComplete() const;

	UFUNCTION(BlueprintCallable)
	void RegisterAlienDeath(ACharacter* Alien);

	UFUNCTION(BlueprintCallable)
	ATargetPoint* GetRandomSpawnPoint() const;

	UFUNCTION(BlueprintCallable)
	void SpawnAlien(TSubclassOf<AActor> ActorToSpawn);

	UFUNCTION(BlueprintCallable)
	void IncreaseWaveDifficulty();

	UFUNCTION(BlueprintCallable)
	void StartNewWave();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
	TArray<ACharacter*> Aliens;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
	int32 AlienSpawnRemaining;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
	int32 WaveDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
	float DifficultyMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
	float SpawnFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
	int32 CurrentWave;
};
