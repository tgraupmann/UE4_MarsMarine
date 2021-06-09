// Fill out your copyright notice in the Description page of Project Settings.


#include "Cpp_HealthPickup.h"
#include "Cpp_Marine.h"

// Sets default values
ACpp_HealthPickup::ACpp_HealthPickup()
{
	HealthPickupAmount = 35.0f;
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
