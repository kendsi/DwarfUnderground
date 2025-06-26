// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DwarfUnderground/Interface/RoleAssignable.h"
#include "DwarfUnderground/Interface/SelectableInterface.h"
#include "Animation/AnimMontage.h"
#include "UnitState.h"
#include "Unit.generated.h"

class UUnitStatComponent;
class AEquipmentBase;
class USquadComponent;
class AResourceBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitStateChanged, AUnit*, ChangedUnit,EUnitState, State);

UCLASS()
class DWARFUNDERGROUND_API AUnit : public ACharacter, public IRoleAssignable, public ISelectableInterface
{
	GENERATED_BODY()

public:
	AUnit();

	//유닛 상태 변경 이벤트
	FOnUnitStateChanged OnStateChanged;

	// 유닛 스탯
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit Stats")
	UUnitStatComponent* Stats;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Stats")
    float CurrentHealth;

    float LastAttackTime = 0.0f;

    int32 SquadIdx;

    FOnMontageEnded DeathMontageEndDelegate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* DefaultAttackMontage;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* DefaultBlockMontage;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* DefaultWorkMontage;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* DeathMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* WorkMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AttackMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* BlockMontage;

    UFUNCTION(Category = "Animation")
    void PlayWorkMontage();

    // 쿨타임 계산
    UFUNCTION(BlueprintCallable, Category = "Unit Actions")
    virtual bool IsAttackCooldown();

    // 공격 함수
    UFUNCTION(BlueprintCallable, Category = "Unit Actions")
    virtual void Attack(AUnit* Target);

    // 데미지를 받는 함수
    UFUNCTION(BlueprintCallable, Category = "Unit Actions")
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION(BlueprintCallable, Category = "Unit Actions")
    virtual void JumpToLocation(FVector Destination);

    // 부대 관련
    UPROPERTY(BlueprintReadWrite)
    USquadComponent* SquadRef = nullptr;

    // ISelectableInterface 구현
    virtual void OnSelected() override;
    virtual void OnDeselected() override;
    virtual FVector GetSelectPoint() const override ;

    // IRoleAssignable 인터페이스 구현
    virtual void AssignRole(FName NewRole);
	virtual FName GetRole() const;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    class AEquipmentBase* EquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    class AEquipmentBase* EquippedShield;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	class AEquipmentBase* EquippedTool;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
    class AEquipmentBase* EquippedArmor;

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    void EquipItem(AEquipmentBase* NewEquipment);

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    void UnequipItem(AEquipmentBase* ItemToUnequip);

    void StartSmoothRotation(const FRotator& TargetRot);

    void SetResourceType(FName Type);
    void SetResourceAmount(int32 Amount);
    FName GetResourceType();
    int32 GetResrouceAmount();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit Role")
    FName CurrentRole;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    FName ResourceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    int32 ResourceAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TArray<USoundBase*> HurtVoices;

    // 연속 재생 방지용 쿨타임(초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    float HurtVoiceCooldown = 4.f;

    FTimerHandle HurtVoiceTimer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 전용 BT 에셋(에디터에서 지정)
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTreeAsset;

	UFUNCTION(BlueprintPure, Category = "State")
    EUnitState GetUnitState() const;

    UFUNCTION(BlueprintPure, Category = "State")
    EUnitState GetPrevState() const;
	UFUNCTION(BlueprintCallable, Category = "State")
    void SetUnitState(EUnitState State);

	UFUNCTION(BlueprintCallable, Category = "State")
	void SetWorkSite(AActor* WorkSite);

	UFUNCTION(BlueprintCallable, Category = "State")
	AActor* GetWorkSite() const;

    void SetVisibility(bool);
    bool GetVisibility();
    bool IsDead();

private:
    // 목표 회전값
    FRotator DesiredRotation;

    // 회전 중 플래그
    bool bRotate;

    // 초당 회전 속도(도)
    UPROPERTY(EditAnywhere, Category = "Rotation")
    float RotationSpeed = 4.f;

	//유닛의 상태값, 이벤트 때문에 Getter, Setter 함수를 통해 접근
	UPROPERTY(VisibleAnywhere, Category = "State")
	EUnitState UnitState;
	
	//현재 할당된 일자리
	TWeakObjectPtr<AActor> CurrentWorkSite;

    UPROPERTY(VisibleAnywhere, Category = "State")
    EUnitState PrevState;

    bool bIsDead;
    bool bIsVisible;

    void Die();
    void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
