// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cpp_HealthPickup.generated.h"

UCLASS()
class MARSMARINE_API ACpp_HealthPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACpp_HealthPickup();

public:	

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	float HealthPickupAmount;
};
