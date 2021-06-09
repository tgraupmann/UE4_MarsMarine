// Fill out your copyright notice in the Description page of Project Settings.


#include "Cpp_HealthPickup.h"
#include "Cpp_Marine.h"

// Sets default values
ACpp_HealthPickup::ACpp_HealthPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthPickupAmount = 35.0f;
}

// Called when the game starts or when spawned
void ACpp_HealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACpp_HealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACpp_HealthPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (IsValid(OtherActor))
	{
		ACpp_Marine* Marine = Cast<ACpp_Marine>(OtherActor);
		if (Marine)
		{
			if (!Marine->IsHealthFull())
			{
				Marine->AddHealth(HealthPickupAmount);
				Destroy();
			}
		}
	}
}
