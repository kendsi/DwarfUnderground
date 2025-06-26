// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit.h"
#include "AIController.h"
#include "DwarfUnderground/Component/UnitStatComponent.h"
#include "DwarfUnderground/Resource/ResourceBase.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "DwarfUnderground/Equipment/Armor.h"
#include "DwarfUnderground/Equipment/Weapon.h"
#include "DwarfUnderground/Equipment/Pickaxe.h"
#include "DwarfUnderground/Equipment/Shield.h"
#include "Squad/SquadComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AUnit::AUnit()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//기존 존재하던 능력치 초기화 코드는 UUnitStatComponent로 옮겨감
	Stats = CreateDefaultSubobject<UUnitStatComponent>(TEXT("UnitStat"));
	CurrentHealth = 100.0f;
	// AttackPower = 10.0f;
	// AttackRange = 200.0f;
	// AttackCooldown = 2.0f;
	// Accuracy = 80.0f;
	// Evasion = 20.0f;
	// ArmorRating = 0.0f;
	// Speed = GetCharacterMovement()->MaxWalkSpeed;
    CurrentRole = NAME_None;

	bRotate = false;
	bIsDead = false;
	bIsVisible = true;

	SquadIdx = -1;

	UnitState = EUnitState::Idle;
	ResourceType = NAME_None;
	ResourceAmount = 0;
}

bool AUnit::IsAttackCooldown() {
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	float ModifiedAttackCooldown = Stats->GetFinalStat(EUnitStatType::AttackCooldown);
	if (CurrentTime - LastAttackTime < ModifiedAttackCooldown) {
		return true;
	}
	return false;
}

void AUnit::Attack(AUnit* Target)
{
    if (!Target)
        return;

	LastAttackTime = GetWorld()->GetTimeSeconds();

	// 공격 애니메이션 재생
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}

	// 명중 판정

	float MyAccuracy = Stats->GetFinalStat(EUnitStatType::Accuracy);
	float TargetEvasion = Target->Stats->GetFinalStat(EUnitStatType::Evasion);
	float HitChance = MyAccuracy - TargetEvasion;
	HitChance = FMath::Clamp(HitChance, 0.f, 100.f);

	if (FMath::FRand() * 100.0f > HitChance)
	{
		FDamageEvent DamageEvent;
		Target->TakeDamage(0.f, DamageEvent, nullptr, this);
		return;
	}

    // 실제 데미지 처리
    FDamageEvent DamageEvent;
	float MyAttackPower = Stats->GetFinalStat(EUnitStatType::AttackPower);
    Target->TakeDamage(MyAttackPower, DamageEvent, nullptr, this);
}

float AUnit::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount == 0) {
		PlayAnimMontage(BlockMontage);
		return 0.f;
	}

	// 1) 비례 감소 계산
	const float K = 100.f;  // 임의 상수 (밸런스에 맞춰 조정)
	const float ModifiedArmor = Stats->GetFinalStat(EUnitStatType::ArmorRating);
	float DefenseRatio = K / (K + ModifiedArmor);
	float FinalDamage = static_cast<float>(FMath::RoundToInt(DamageAmount * DefenseRatio));

	// 2) 최소 1 대미지 보장
	FinalDamage = FMath::Max(FinalDamage, 1.f);

	// 3) 체력 차감
	CurrentHealth -= FinalDamage;
	if (CurrentHealth <= 0.f)
	{
		//SquadRef->SetNewLeader(this);
		Die();
	}

	// 4) 피격음 재생
	if (!GetWorld()->GetTimerManager().IsTimerActive(HurtVoiceTimer) && HurtVoices.Num() > 0)
	{
		// 랜덤으로 하나 선택
		const int32 Idx = FMath::RandRange(0, HurtVoices.Num() - 1);
		UGameplayStatics::PlaySoundAtLocation(this, HurtVoices[Idx], GetActorLocation());

		// 쿨타임 시작
		GetWorld()->GetTimerManager().SetTimer(HurtVoiceTimer, HurtVoiceCooldown, false);
	}

	return FinalDamage;
}

void AUnit::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	// 충돌·AI 비활성화(선택)
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DetachFromControllerPendingDestroy();

	if (DeathMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		UAnimInstance* Anim = GetMesh()->GetAnimInstance();

		DeathMontageEndDelegate.Unbind();
		DeathMontageEndDelegate.BindUObject(this, &AUnit::OnDeathMontageEnded);

		const float Duration = Anim->Montage_Play(DeathMontage);
		if (Duration > 0.f)
		{
			Anim->Montage_SetEndDelegate(DeathMontageEndDelegate, DeathMontage);
			return;
		}
	}

	Destroy();
}

void AUnit::OnDeathMontageEnded(UAnimMontage* /*Montage*/, bool /*bInterrupted*/)
{
	Destroy();
}

void AUnit::JumpToLocation(FVector Destination)
{
    ACharacter* Character = Cast<ACharacter>(this);
    if (!Character)
    {
        return;
    }

    FVector CurrentLocation = GetActorLocation();
    FVector JumpDirection = Destination - CurrentLocation;

    float DistanceXY = FVector(JumpDirection.X, JumpDirection.Y, 0).Size();
    float DistanceZ = JumpDirection.Z;

    const float Gravity = 980.0f;
    float JumpVelocityZ = 1.1 * FMath::Sqrt(2 * Gravity * FMath::Max(200.0f, DistanceZ + 100.0f));

    FVector LaunchVelocity = FVector(JumpDirection.X, JumpDirection.Y, 0).GetSafeNormal() * DistanceXY * 1.2f;
    LaunchVelocity.Z = JumpVelocityZ;

    Character->LaunchCharacter(LaunchVelocity, true, true);
}

void AUnit::AssignRole(FName NewRole)
{
    CurrentRole = NewRole;
}

FName AUnit::GetRole() const
{
    return CurrentRole;
}

void AUnit::EquipItem(AEquipmentBase* NewEquipment)
{
	if (!NewEquipment)
	{
		return;
	}

	// 무기인지 확인
	if (AShield* Shield = Cast<AShield>(NewEquipment))
	{
		// 기존 방패가 있다면 해제
		if (EquippedShield)
		{
			UnequipItem(EquippedShield);
		}

		// 새 방패 장착
		EquippedShield = Shield;
		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
		EquippedShield->AttachToComponent(GetMesh(), AttachRules, TEXT("LeftWeaponSocket"));
		EquippedShield->OnEquipped(this);

		// 방패를 장착하면, 그 방패가 가진 BlockMontage를 캐릭터 쪽 BlockMontage로 세팅
		if (Shield->BlockMontage)
		{
			BlockMontage = Shield->BlockMontage;
		}
	}
	else if (APickaxe* Pickaxe = Cast<APickaxe>(NewEquipment))
	{
		if (EquippedTool) {
			UnequipItem(EquippedTool);
		}

		EquippedTool = Pickaxe;
		EquippedTool->OnEquipped(this);

		if (Pickaxe->WorkMontage)
		{
			WorkMontage = Pickaxe->WorkMontage;
		}
	}
	else if (AWeapon* Weapon = Cast<AWeapon>(NewEquipment))
	{
		// 기존 무기가 있다면 해제
		if (EquippedWeapon)
		{
			UnequipItem(EquippedWeapon);
		}

		// 새 무기 장착

		EquippedWeapon = Weapon;
		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
		EquippedWeapon->AttachToComponent(GetMesh(), AttachRules, TEXT("RightWeaponSocket"));
		EquippedWeapon->OnEquipped(this);

		// 무기를 장착하면, 그 무기가 가진 AttackMontage를 캐릭터 쪽 AttackMontage로 세팅
		if (Weapon->AttackMontage)
		{
			AttackMontage = Weapon->AttackMontage;
		}
	}
	else if (AArmor* Armor = Cast<AArmor>(NewEquipment))
	{
		// 기존 방어구가 있다면 해제
		if (EquippedArmor)
		{
			UnequipItem(EquippedArmor);
		}

		// 새 방어구 장착
		EquippedArmor = Armor;
		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
		EquippedArmor->AttachToComponent(GetMesh(), AttachRules, TEXT("ArmorSocket"));
		EquippedArmor->OnEquipped(this);
	}
}

void AUnit::UnequipItem(AEquipmentBase* ItemToUnequip)
{
	if (!ItemToUnequip)
		return;

	// 기본 해제 로직
	ItemToUnequip->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemToUnequip->OnUnequipped(this);

	// 방패인지 확인
	if (Cast<AShield>(ItemToUnequip) && EquippedShield == ItemToUnequip)
	{
		EquippedShield = nullptr;
		BlockMontage = DefaultBlockMontage;
	}
	else if (Cast<APickaxe>(ItemToUnequip) && EquippedTool == ItemToUnequip)
	{
		EquippedTool = nullptr;
		WorkMontage = DefaultWorkMontage;
	}
	// 무기인지 확인
	else if (Cast<AWeapon>(ItemToUnequip) && EquippedWeapon == ItemToUnequip)
	{
		EquippedWeapon = nullptr;
		AttackMontage = DefaultAttackMontage;
	}
	// 방어구인지 확인
	else if (Cast<AArmor>(ItemToUnequip) && EquippedArmor == ItemToUnequip)
	{
		EquippedArmor = nullptr;
	}
}

EUnitState AUnit::GetUnitState() const{
	return UnitState;
}

EUnitState AUnit::GetPrevState() const {
	return PrevState;
}

void AUnit::SetUnitState(EUnitState State) {
	OnStateChanged.Broadcast(this,State);
	PrevState = UnitState;
	UnitState = State;
}

void AUnit::SetWorkSite(AActor* WorkSite)
{
	//장소 할당이 존재하면 추가, 없으면 null
	CurrentWorkSite = WorkSite;
}

AActor* AUnit::GetWorkSite() const
{
	return CurrentWorkSite.Get();
}

// Called when the game starts or when spawned
void AUnit::BeginPlay()
{
	Super::BeginPlay();

	AttackMontage = DefaultAttackMontage;
	BlockMontage = DefaultBlockMontage;
	WorkMontage = DefaultWorkMontage;
}

// Called every frame
void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRotate)
	{
		FRotator Current = GetActorRotation();
		// RInterpTo(Current, Target, DeltaTime, Speed)
		FRotator NewRot = FMath::RInterpTo(Current, DesiredRotation, DeltaTime, RotationSpeed);
		SetActorRotation(NewRot);

		// 충분히 가까워지면 회전 종료
		if (NewRot.Equals(DesiredRotation, 1.0f))
		{
			bRotate = false;
		}
	}
}

void AUnit::PlayWorkMontage() {
	PlayAnimMontage(WorkMontage);
}

void AUnit::OnSelected() {
	if (UDecalComponent* Decal = FindComponentByClass<UDecalComponent>()) {
		Decal->SetVisibility(true);
	}
}
void AUnit::OnDeselected() {
	if (UDecalComponent* Decal = FindComponentByClass<UDecalComponent>()) {
		Decal->SetVisibility(false);
	}
}
FVector AUnit::GetSelectPoint() const { return GetActorLocation(); }

void AUnit::StartSmoothRotation(const FRotator& TargetRot)
{
	DesiredRotation = TargetRot;
	bRotate = true;
}

void AUnit::SetResourceType(FName Type) {
	ResourceType = Type;
}

void AUnit::SetResourceAmount(int32 Amount) {
	ResourceAmount = Amount;
}

FName AUnit::GetResourceType() {
	return ResourceType;
}

int32 AUnit::GetResrouceAmount() {
	return ResourceAmount;
}

void AUnit::SetVisibility(bool visibility) {
	bIsVisible = visibility;
}

bool AUnit::GetVisibility() {
	return bIsVisible;
}

bool AUnit::IsDead() {
	return bIsDead;
}