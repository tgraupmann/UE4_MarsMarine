// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cpp_MIssionAreaVolume.generated.h"

UCLASS()
class MARSMARINE_API ACpp_MIssionAreaVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACpp_MIssionAreaVolume();

public:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:

	class ACpp_Marine* GetMarine(int32 PlayerIndex) const;
	
	int32 GetPlayerIndex(class ACpp_Marine* Marine) const;

	void MakeTimers();

	FTimerHandle& GetTimer(class ACpp_Marine* Marine);

	UFUNCTION()
	void KillPlayer(class ACpp_Marine* Marine);

	float SecondsBeforePlayerDeath;

	TArray<FTimerHandle> TimersHandleKillPlayer;
};
