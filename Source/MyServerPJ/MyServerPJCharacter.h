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
	// �÷��̾��� �ִ� ü��. ü���� �ִ��Դϴ�. �� ���� ���� �� ���۵Ǵ� ĳ������ ü�� ���Դϴ�.
	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float MaxHealth;

	 // �÷��̾��� ���� ü��. 0�� �Ǹ� ���� ������ ���ֵ˴ϴ�.
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, EditAnywhere)
		float CurrentHealth;

	// ���� ü�¿� ������ ���濡 ���� RepNotify
	UFUNCTION()
		void OnRep_CurrentHealth();
	// ������Ʈ�Ǵ� ü�¿� ����. ���������� ���� ��� ȣ��, Ŭ���̾�Ʈ������ RepNotify�� �����Ͽ� ȣ��
	void OnHealthUpdate();

	UPROPERTY(ReplicatedUsing = OnRep_bIsDie, BlueprintReadWrite, Category = "DieUnityChan")
		bool bIsDie = false;

	UFUNCTION()
		void OnRep_bIsDie();
public:
	AMyServerPJCharacter();
	virtual void BeginPlay() override;
	// ������Ƽ ���ø����̼� 
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
		/** �߻� ������, ������ ��. �׽�Ʈ �߻�ü�� �߻� �ӵ��� �����ϴ� �� ��������, ���� �Լ��� �߰����� SpawnProjectile�� �Է¿� ���� ���ε����� �ʰ� �ϴ� ���ҵ� �մϴ�.*/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float FireRate;

	/** true�� ��� �߻�ü�� �߻��ϴ� ���μ��� �����Դϴ�. */
	bool bIsFiringWeapon;

	/** ���� �߻� ���� �Լ�*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartFire(float Val);

	/** ���� �߻� ���� �Լ�. ȣ��Ǹ� �÷��̾ StartFire�� �ٽ� ����� �� �ֽ��ϴ�.*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopFire();

	/** �߻�ü�� �����ϴ� ���� �Լ�*/
	UFUNCTION(Server, Reliable)
		void HandleFire();
	void HandleFire_Implementation();

	UFUNCTION()
	void DelayRespawn();

	/** ���� ���̿� �߻� �ӵ� �����̸� �ִ� Ÿ�̸� �ڵ�*/
	FTimerHandle FiringTimer;
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	/** �ִ� ü�� ����*/
	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	/** ���� ü�� ����*/
	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	/** ���� ü�� ����. ���� 0�� MaxHealth ���̷� ���������ϰ� OnHealthUpdate�� ȣ���մϴ�. ���������� ȣ��Ǿ�� �մϴ�.*/
	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentHealth(float healthValue);

	/** ������� �޴� �̺�Ʈ. APawn���� �������̵�˴ϴ�.*/
	UFUNCTION(BlueprintCallable, Category = "Health")
		float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY()
	APlayerController* ShootingPlayerController;
};

