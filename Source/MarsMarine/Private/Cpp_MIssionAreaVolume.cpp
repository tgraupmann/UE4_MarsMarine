// Fill out your copyright notice in the Description page of Project Settings.


#include "Cpp_MIssionAreaVolume.h"
#include "Cpp_Marine.h"
#include "Cpp_MarsMarine_GameMode.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ACpp_MIssionAreaVolume::ACpp_MIssionAreaVolume()
{
	SecondsBeforePlayerDeath = 5.0f;

}

void ACpp_MIssionAreaVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (IsValid(OtherActor))
		{
			ACpp_Marine* Marine = Cast<ACpp_Marine>(OtherActor);
			if (Marine)
			{
				Marine->SetOutsideMissionArea(false);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Player Inside: %d"), Marine->GetPlayerIndex()));

				MakeTimers();
				GetWorldTimerManager().ClearTimer(GetTimer(Marine));
				GetTimer(Marine).Invalidate();
			}
		}
	}
}

void ACpp_MIssionAreaVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (IsValid(OtherActor))
		{
			ACpp_Marine* Marine = Cast<ACpp_Marine>(OtherActor);
			if (Marine)
			{
				Marine->SetOutsideMissionArea(true);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Player Outside: %d"), Marine->GetPlayerIndex()));

				MakeTimers();
				if (!GetTimer(Marine).IsValid())
				{
					FTimerDelegate TimerDel;
					TimerDel.BindUFunction(this, "KillPlayer", Marine);
					GetWorldTimerManager().SetTimer(GetTimer(Marine), TimerDel, SecondsBeforePlayerDeath, false);
				}
			}
		}
	}
}

void ACpp_MIssionAreaVolume::KillPlayer(ACpp_Marine* Marine)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (Marine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Kill Player: %d"), Marine->GetPlayerIndex()));
			UGameplayStatics::ApplyDamage(Marine, Marine->GetHealth(), nullptr, nullptr, nullptr);
		}
	}
}

void ACpp_MIssionAreaVolume::MakeTimers()
{
	AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (IsValid(BaseGameMode))
	{
		while (TimersHandleKillPlayer.Num() < BaseGameMode->GetNumPlayers())
		{
			TimersHandleKillPlayer.Add(FTimerHandle());
		}
	}
}

FTimerHandle& ACpp_MIssionAreaVolume::GetTimer(ACpp_Marine* Marine)
{
	MakeTimers();
	int PlayerIndex = Marine->GetPlayerIndex();
	return TimersHandleKillPlayer[PlayerIndex];
}
