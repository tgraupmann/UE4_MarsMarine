// Fill out your copyright notice in the Description page of Project Settings.


#include "Cpp_Marine.h"
#include "Cpp_MarsMarine_GameMode.h"
#include "Cpp_PlayerHUD.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetInputLibrary.h>
#include <Kismet/KismetMathLibrary.h>
#include <Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraComponent.h>
#include <Components/AudioComponent.h>
#include <Blueprint/UserWidget.h>
#include <Net/UnrealNetwork.h>

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

	AxisThumbstickAimUp = 0;
	AxisThumbstickAimRight = 0;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void ACpp_Marine::SetCompMuzzleFlash(UParticleSystemComponent* Comp)
{
	CompMuzzleFlash = Comp;
}

// Called when the game starts or when spawned
void ACpp_Marine::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		if (IsValid(HUDClass))
		{
			UUserWidget* Widget = CreateWidget(GetWorld(), HUDClass);
			if (IsValid(Widget))
			{
				UCpp_PlayerHUD* PlayerHUD = Cast<UCpp_PlayerHUD>(Widget);
				if (IsValid(PlayerHUD))
				{
					PlayerHUD->Marine = this;
				}
				Widget->AddToViewport();
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Created Player HUD: %d"), GetPlayerIndex()));
			}
		}
	}
}

// Called every frame
void ACpp_Marine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePlayerAim();

	/*
	if (IsLocallyControlled())
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Server player: %d"), GetPlayerIndex()));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Client player: %d"), GetPlayerIndex()));
		}
	}
	*/
}

// Called to bind functionality to input
void ACpp_Marine::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACpp_Marine::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACpp_Marine::MoveRight);

	PlayerInputComponent->BindAxis("Thumbstick MoveForward", this, &ACpp_Marine::ThumbstickMoveForward);
	PlayerInputComponent->BindAxis("Thumbstick MoveRight", this, &ACpp_Marine::ThumbstickMoveRight);

	PlayerInputComponent->BindAxis("Thumbstick AimUp", this, &ACpp_Marine::ThumbstickAimUp);
	PlayerInputComponent->BindAxis("Thumbstick AimRight", this, &ACpp_Marine::ThumbstickAimRight);

	PlayerInputComponent->BindAction("AnyKey", IE_Pressed, this, &ACpp_Marine::AnyKeyPressed);
	PlayerInputComponent->BindAction("AnyKey", IE_Released, this, &ACpp_Marine::AnyKeyReleased);

	PlayerInputComponent->BindAction("Fire Weapon", IE_Pressed, this, &ACpp_Marine::StartFiringWeapon);
	PlayerInputComponent->BindAction("Fire Weapon", IE_Released, this, &ACpp_Marine::StopFiringWeapon);
}

void ACpp_Marine::MoveForward(float AxisValue)
{
	if (IsAlive() && !GamepadActive)
	{
		AddMovementInput(FVector(1, 0, 0) * AxisValue);
	}
}

void ACpp_Marine::MoveRight(float AxisValue)
{
	if (IsAlive() && !GamepadActive)
	{
		AddMovementInput(FVector(0, 1, 0) * AxisValue);
	}
}

void ACpp_Marine::ThumbstickMoveForward(float AxisValue)
{
	if (IsAlive() && GamepadActive)
	{
		AddMovementInput(FVector(1, 0, 0) * AxisValue);
	}
}

void ACpp_Marine::ThumbstickMoveRight(float AxisValue)
{
	if (IsAlive() && GamepadActive)
	{
		AddMovementInput(FVector(0, 1, 0) * AxisValue);
	}
}

void ACpp_Marine::ThumbstickAimUp(float AxisValue)
{
	AxisThumbstickAimUp = AxisValue;
}

void ACpp_Marine::ThumbstickAimRight(float AxisValue)
{
	AxisThumbstickAimRight = AxisValue;
}

void ACpp_Marine::AnyKeyPressed(FKey Key)
{
	if (UKismetInputLibrary::Key_IsKeyboardKey(Key))
	{
		GamepadActive = false;
	}

	else if (UKismetInputLibrary::Key_IsMouseButton(Key))
	{
		GamepadActive = false;
	}

	else if (UKismetInputLibrary::Key_IsGamepadKey(Key))
	{
		GamepadActive = true;
	}
}

void ACpp_Marine::AnyKeyReleased(FKey Key)
{
	if (UKismetInputLibrary::Key_IsKeyboardKey(Key))
	{
		GamepadActive = false;
	}
	
	else if (UKismetInputLibrary::Key_IsMouseButton(Key))
	{
		GamepadActive = false;
	}

	else if (UKismetInputLibrary::Key_IsGamepadKey(Key))
	{
		GamepadActive = true;
	}
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

void ACpp_Marine::ServerClientWeaponTrace(bool IsServer)
{
	FVector Start = GetWeaponTraceStartLocation();
	FVector End = GetWeaponTraceEndLocation();
	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility, Params))
	{
		if (OutHit.bBlockingHit)
		{
			if (OutHit.Actor.IsValid())
			{
				SpawnImpactHit(OutHit.Actor.Get(), OutHit.Location);
				if (IsServer)
				{
					UGameplayStatics::ApplyDamage(OutHit.Actor.Get(), DamagePerShot, nullptr, nullptr, nullptr);
				}
			}
		}
	}
}

void ACpp_Marine::ServerWeaponTrace_Implementation()
{
	ServerClientWeaponTrace(true);
}

bool ACpp_Marine::ServerWeaponTrace_Validate()
{
	return true;
}

void ACpp_Marine::WeaponTrace()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		ServerClientWeaponTrace(false);
	}

	ServerWeaponTrace();
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

	if (GetLocalRole() == ROLE_Authority)
	{
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
	}

	return Result;
}

void ACpp_Marine::Respawn()
{
	//UGameplayStatics::OpenLevel(GetWorld(), FName("Mars"));
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

FVector2D ACpp_Marine::GetMouseVelocity() const
{
	if (!IsLocallyControlled())
	{
		return FVector2D::ZeroVector;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (IsValid(PlayerController))
	{
		float DeltaX, DeltaY;
		PlayerController->GetInputMouseDelta(DeltaX, DeltaY);

		float DeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		return (FVector2D(DeltaX, DeltaY) / DeltaSeconds);
	}

	return FVector2D::ZeroVector;
}

bool ACpp_Marine::IsMouseAboveDeadzone() const
{
	FVector2D Vec = GetMouseVelocity();
	return (UKismetMathLibrary::VSize2D(Vec) > MouseDeadzone);
}

FRotator ACpp_Marine::GetMouseAimDirection() const
{
	if (!IsLocallyControlled())
	{
		return FRotator::ZeroRotator;
	}

	FRotator ControlRotation = Controller->GetControlRotation();
		
	FVector2D Vec = GetMouseVelocity();

	FRotator Target = FRotator::ZeroRotator;
	Target.Yaw = UKismetMathLibrary::DegAtan2(Vec.X, Vec.Y);

	return UKismetMathLibrary::RInterpTo(ControlRotation, Target,
		UGameplayStatics::GetWorldDeltaSeconds(GetWorld()),
		MouseSmoothingStrength);
}

void ACpp_Marine::UpdateMouseAim()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (IsMouseAboveDeadzone())
	{
		FRotator NewRotation = GetMouseAimDirection();
		Controller->SetControlRotation(NewRotation);
	}
}

bool ACpp_Marine::IsThumbstickAboveDeadzone() const
{
	FVector2D Vec(AxisThumbstickAimUp, AxisThumbstickAimRight);
	return (UKismetMathLibrary::VSize2D(Vec) > ThumbstickDeadzone);
}

FRotator ACpp_Marine::GetThumbstickAimDirection() const
{
	if (!IsLocallyControlled())
	{
		return FRotator::ZeroRotator;
	}

	FRotator ControlRotation = Controller->GetControlRotation();

	FRotator Target = FRotator::ZeroRotator;
	Target.Yaw = UKismetMathLibrary::DegAtan2(AxisThumbstickAimRight, AxisThumbstickAimUp);

	return UKismetMathLibrary::RInterpTo(ControlRotation, Target,
		UGameplayStatics::GetWorldDeltaSeconds(GetWorld()),
		MouseSmoothingStrength);
}

void ACpp_Marine::UpdateThumbstickAim()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (IsThumbstickAboveDeadzone())
	{
		FRotator NewRotation = GetThumbstickAimDirection();
		Controller->SetControlRotation(NewRotation);
	}
}

void ACpp_Marine::UpdatePlayerAim()
{
	if (IsAlive())
	{
		if (GamepadActive)
		{
			UpdateThumbstickAim();
		}
		else
		{
			UpdateMouseAim();
		}
	}
}

void ACpp_Marine::ClientOnRep_Dead()
{

}

void ACpp_Marine::ClientOnRep_Health()
{

}

void ACpp_Marine::ClientOnRep_Kills()
{

}

void ACpp_Marine::ClientOnRep_OutsideMissionArea()
{

}

void ACpp_Marine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACpp_Marine, Dead);
	DOREPLIFETIME(ACpp_Marine, Health);
	DOREPLIFETIME(ACpp_Marine, Kills);
	DOREPLIFETIME(ACpp_Marine, OutsideMissionArea);
}

ACpp_Marine* ACpp_Marine::GetMarine(UWorld* World, int32 PlayerIndex)
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(World, PlayerIndex);
	if (IsValid(Character))
	{
		return Cast<ACpp_Marine>(Character);
	}
	return nullptr;
}

int32 ACpp_Marine::GetPlayerIndex() const
{
	AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (IsValid(BaseGameMode))
	{
		for (int PlayerIndex = 0; PlayerIndex < BaseGameMode->GetNumPlayers(); ++PlayerIndex)
		{
			ACpp_Marine* Marine = ACpp_Marine::GetMarine(GetWorld(), PlayerIndex);
			if (IsValid(Marine))
			{
				if (Marine == this)
				{
					return PlayerIndex;
				}
			}
		}
	}
	return 0;
}
