// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyServerPJCharacter.generated.h"

UCLASS(config=Game)
class AMyServerPJCharacter : public ACharacter
{
	GENERATED_BODY()
private:
	float shotSpeed = 0.0f;
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

protected:
	// 플레이어의 최대 체력. 체력의 최댓값입니다. 이 값은 스폰 시 시작되는 캐릭터의 체력 값입니다.
	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float MaxHealth;

	 // 플레이어의 현재 체력. 0이 되면 죽은 것으로 간주됩니다.
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, EditAnywhere)
		float CurrentHealth;

	// 현재 체력에 가해진 변경에 대한 RepNotify
	UFUNCTION()
		void OnRep_CurrentHealth();
	// 업데이트되는 체력에 반응. 서버에서는 수정 즉시 호출, 클라이언트에서는 RepNotify에 반응하여 호출
	void OnHealthUpdate();

	UPROPERTY(ReplicatedUsing = OnRep_bIsDie, BlueprintReadWrite, Category = "DieUnityChan")
		bool bIsDie = false;

	UFUNCTION()
		void OnRep_bIsDie();
public:
	AMyServerPJCharacter();
	virtual void BeginPlay() override;
	// 프로퍼티 리플리케이션 
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UnityChan")
		class USkeletalMesh* UnityChanMesh;
protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Projectile")
		TSubclassOf<class AThirdPersonMPProjectile> ProjectileClass;
		/** 발사 딜레이, 단위는 초. 테스트 발사체의 발사 속도를 제어하는 데 사용되지만, 서버 함수의 추가분이 SpawnProjectile을 입력에 직접 바인딩하지 않게 하는 역할도 합니다.*/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float FireRate;

	/** true인 경우 발사체를 발사하는 프로세스 도중입니다. */
	bool bIsFiringWeapon;

	/** 무기 발사 시작 함수*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartFire(float Val);

	/** 무기 발사 종료 함수. 호출되면 플레이어가 StartFire를 다시 사용할 수 있습니다.*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopFire();

	/** 발사체를 스폰하는 서버 함수*/
	UFUNCTION(Server, Reliable)
		void HandleFire();
	void HandleFire_Implementation();

	UFUNCTION()
	void DelayRespawn();

	/** 스폰 사이에 발사 속도 딜레이를 넣는 타이머 핸들*/
	FTimerHandle FiringTimer;
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	/** 최대 체력 게터*/
	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	/** 현재 체력 게터*/
	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	/** 현재 체력 세터. 값을 0과 MaxHealth 사이로 범위제한하고 OnHealthUpdate를 호출합니다. 서버에서만 호출되어야 합니다.*/
	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentHealth(float healthValue);

	/** 대미지를 받는 이벤트. APawn에서 오버라이드됩니다.*/
	UFUNCTION(BlueprintCallable, Category = "Health")
		float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY()
	APlayerController* ShootingPlayerController;
};

