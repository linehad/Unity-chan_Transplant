// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyServerPJCharacter.h"
#include "SD_UnityChanAnimInstance.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include <Net/UnrealNetwork.h>
#include <Engine/Engine.h>
#include "Engine/SkeletalMesh.h"
#include "ThirdPersonMPProjectile.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// AMyServerPJCharacter

AMyServerPJCharacter::AMyServerPJCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(25.f, 45.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SocketOffset.Z = 50;
	CameraBoom->TargetArmLength = -50.f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	//플레이어 체력 초기화
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	//발사체 클래스 초기화
	ProjectileClass = AThirdPersonMPProjectile::StaticClass();
	//발사 속도 초기화
	FireRate = 0.01f;
	bIsFiringWeapon = false;
}
void AMyServerPJCharacter::BeginPlay()
{
	Super::BeginPlay();
	ShootingPlayerController = Cast<APlayerController>(this->GetController());
}
//////////////////////////////////////////////////////////////////////////
// Input

void AMyServerPJCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyServerPJCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyServerPJCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyServerPJCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyServerPJCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMyServerPJCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMyServerPJCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMyServerPJCharacter::OnResetVR);
	// 발사체 발사 처리
	PlayerInputComponent->BindAxis("FireBullet", this, &AMyServerPJCharacter::StartFire);
}

void AMyServerPJCharacter::OnResetVR()
{
	// If MyServerPJ is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in MyServerPJ.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMyServerPJCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AMyServerPJCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AMyServerPJCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyServerPJCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyServerPJCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyServerPJCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMyServerPJCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 현재 체력 리플리케이트
	DOREPLIFETIME(AMyServerPJCharacter, CurrentHealth);
	DOREPLIFETIME(AMyServerPJCharacter, bIsDie);
}

void AMyServerPJCharacter::OnHealthUpdate()
{
	// 클라이언트 전용
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("HP : %f"), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	
		if (CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You Die."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}

	// 서버 전용
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s is HP : %f"), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}
	
	// 플레이어가 죽었을때
	if (CurrentHealth <= 0 && !bIsDie)
	{
		ShootingPlayerController = Cast<APlayerController>(this->GetController());
		if (ShootingPlayerController == nullptr) {
			return;
		}
		FTimerHandle WaitHandle;
		GetWorld()->GetTimerManager().ClearTimer(WaitHandle);
		if (GetLocalRole() == ROLE_Authority)
		{
			GetWorld()->GetTimerManager().SetTimer(WaitHandle, this, &AMyServerPJCharacter::DelayRespawn, 1.0f, false, 5.0f);
			ShootingPlayerController->UnPossess();
		}
		bIsDie = true;
	}
}

void AMyServerPJCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void AMyServerPJCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float AMyServerPJCharacter::TakeDamage(float DamageTaken, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damageApplied = CurrentHealth - DamageTaken;
	if (GetLocalRole() == ROLE_Authority)
	{
		SetCurrentHealth(damageApplied);
	}
	return damageApplied;
}

void AMyServerPJCharacter::StartFire(float Val)
{
	shotSpeed += Val;
	if ((Controller != nullptr) && (shotSpeed >15.0f)) {
		if (!bIsFiringWeapon)
		{
			shotSpeed = 0;
			bIsFiringWeapon = true;
			UWorld* World = GetWorld();
			World->GetTimerManager().SetTimer(FiringTimer, this, &AMyServerPJCharacter::StopFire, FireRate, false);
			HandleFire();
		}
	}
}

void AMyServerPJCharacter::StopFire()
{
	bIsFiringWeapon = false;
}

void AMyServerPJCharacter::HandleFire_Implementation()
{
	FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 100.0f) + (GetActorUpVector() * 0.0f);
	FRotator spawnRotation = GetControlRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	AThirdPersonMPProjectile* spawnedProjectile = GetWorld()->SpawnActor<AThirdPersonMPProjectile>(spawnLocation, spawnRotation, spawnParameters);
}

void AMyServerPJCharacter::OnRep_bIsDie()
{
	bIsDie;
}

void AMyServerPJCharacter::DelayRespawn()
{
	bIsDie = false;
	CurrentHealth = 100;

	if (GetLocalRole() == ROLE_Authority)
	{
		if (ShootingPlayerController)
		{
			ShootingPlayerController->Possess(this);
		}
	}
}