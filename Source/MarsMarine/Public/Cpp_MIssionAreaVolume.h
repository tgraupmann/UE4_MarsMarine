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

	UFUNCTION()
	void KillPlayer();

	float SecondsBeforePlayerDeath;

	FTimerHandle TimerHandleKillPlayer;

};
