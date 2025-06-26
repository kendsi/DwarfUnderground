#include "DwarfUndergroundPlayerController.h"
#include "./BuildManagement/DwarfUndergroundBaseTile.h"
#include "./BuildManagement/DwarfUndergroundBuildManager.h"
#include "./BuildManagement/SoilManager.h"
#include "./BuildManagement/Buildings/BuildingBase.h"
#include "./Resource/GoldOre.h"
#include "./Resource/IronOre.h"
#include "./Resource/ResourceBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Components/Button.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AI/Controller/DwarfAIController.h"
#include "Unit/EnemyUnit.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "DwarfUndergroundCharacter.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"
#include "Input/BuildInteractionComponent.h"
#include "Input/UnitInteractionComponent.h"
#include "Unit/Squad/SquadProxy.h"
#include "Unit/Squad/SquadComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Item/InventorySubSystem.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ADwarfUndergroundPlayerController::ADwarfUndergroundPlayerController()
{
	SquadList.SetNum(10);
}

void ADwarfUndergroundPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// 첫 번째 BuildManager 검색
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADwarfUndergroundBuildManager::StaticClass(), FoundActors);

	// BuildManagerRef 초기화
	if (FoundActors.Num() > 0)
	{
		BuildManagerRef = Cast<ADwarfUndergroundBuildManager>(FoundActors[0]);
		if (BuildManagerRef)
		{
			UE_LOG(LogTemp, Display, TEXT("BuildManager found and referenced successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to cast found actor to ADwarfUndergroundBuildManager"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No BuildManager found in the level"));
	}

	AEnemyUnit::OnAllEnemiesDead.AddUObject(this, &ADwarfUndergroundPlayerController::HandleAllDead);

	WoodCount = 30;
	GoldIngotCount = 30;
	GoldOreCount = 30;
	StoneCount = 30;
	IronIngotCount = 30;
	IronOreCount = 30;

	bShowMouseCursor = true;

	UGameViewportClient* VC = GetWorld()->GetGameViewport();
	if (VC)
	{
		VC->SetMouseLockMode(EMouseLockMode::LockAlways);
	}
}

void ADwarfUndergroundPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	check(EIC);

	UnitComp = NewObject<UUnitInteractionComponent>(this, TEXT("UnitInteraction"));
	UnitComp->RegisterComponent();
	UnitComp->Bind(EIC, IA_LClick_Unit, IA_RClick_Unit, IA_SquadSelect, IA_DwarfInfo, IA_Inventory, IA_ModeChange, IA_ESC);

	BuildComp = NewObject<UBuildInteractionComponent>(this, TEXT("BuildInteraction"));
	BuildComp->RegisterComponent();
	BuildComp->Bind(EIC, IA_LClick_Build, IA_RClick_Build, IA_BuildingRotation, IA_ModeChange, IA_BuildDestroyModeChange, IA_ESC);

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(MC_Unit, 0);
	}

	SetBuildSelectionMode(2);
}

void ADwarfUndergroundPlayerController::SetBuildSelectionMode(int32 Mode)
/* 0=Build, 1=Destroy, 2=Normal(Unit) */
{
	auto* Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	switch (Mode)
	{
	case 0: // Build
		bEnableMouseOverEvents = true;
		Sub->RemoveMappingContext(MC_Unit);
		Sub->AddMappingContext(MC_Build, 0);
		if (BuildManagerRef)
		{
			BuildManagerRef->SetAllTilesOpacity(0.5f);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					5.0f,
					FColor::Blue,
					TEXT("Build Mode")
				);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BuildManagerRef is null in Build Mode"));
		}
		BuildComp->SetMode(EBuildMode::Build);
		break;

	case 1: // Destroy
		bEnableMouseOverEvents = true;
		Sub->RemoveMappingContext(MC_Unit);
		Sub->AddMappingContext(MC_Build, 0);
		if (BuildManagerRef)
		{
			BuildManagerRef->SetAllTilesOpacity(0.5f);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					5.0f,
					FColor::Red,
					TEXT("Destroy Mode")
				);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BuildManagerRef is null in Destroy Mode"));
		}

		BuildComp->SetMode(EBuildMode::Destroy);
		break;

	default: // Normal (Unit)
		bEnableMouseOverEvents = false;
		Sub->RemoveMappingContext(MC_Build);
		Sub->AddMappingContext(MC_Unit, 0);
		if (BuildManagerRef) BuildManagerRef->SetAllTilesOpacity(0.f);
		break;
	}

	BuildSelectionMode = Mode;
}

void ADwarfUndergroundPlayerController::SetBuildingType(TSubclassOf<AActor> TypeSelect)

{
	if (!TypeSelect->ImplementsInterface(UBuildable::StaticClass()))
	{
		//Buildable이 아닌 타입이 Set되면 경고 메시지 출력
		UE_LOG(LogTemp, Warning, TEXT("PlayerController::SetBuildingType: 입력된 타입이 Buildable이 아님"));		
	}
	BuildingType = TypeSelect;
}

TSubclassOf<AActor> ADwarfUndergroundPlayerController::GetBuildingType()
{
	return BuildingType;
}

ADwarfUndergroundBuildManager* ADwarfUndergroundPlayerController::GetBuildManagerRef() {
	return BuildManagerRef;
}

int32 ADwarfUndergroundPlayerController::GetBuildSelectionMode()
{
	return BuildSelectionMode;
}

void ADwarfUndergroundPlayerController::IncreaseResource(FName ResourceType, int32 Amount)
{
	if (ResourceType == "Wood")
	{
		WoodCount += Amount;
	}
	else if (ResourceType == "Stone")
	{
		StoneCount += Amount;
	}
	else if (ResourceType == "GoldOre")
	{
		GoldOreCount += Amount;
	}
	else if (ResourceType == "GoldIngot")
	{
		GoldIngotCount += Amount;
	}
	else if (ResourceType == "IronOre")
	{
		IronOreCount += Amount;
	}
	else if (ResourceType == "IronIngot")
	{
		IronIngotCount += Amount;
	}
	GetGameInstance()->GetSubsystem<UInventorySubSystem>()->AddItem(ResourceType, Amount);
}

USquadComponent* ADwarfUndergroundPlayerController::CreateEmptySquad()
{
	int32 FreeIdx = INDEX_NONE;
	static const TArray<int32> Order = { 1,2,3,4,5,6,7,8,9,0 };
	for (int32 Num : Order)
	{
		if (SquadList.IsValidIndex(Num) && SquadList[Num] == nullptr)
		{
			FreeIdx = Num;
			break;
		}
	}
	if (FreeIdx == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Squad] Cannot create more squads"));
		return nullptr;
	}

	// Proxy Actor 생성
	ASquadProxy* Proxy = GetWorld()->SpawnActor<ASquadProxy>();
	if (!Proxy) return nullptr;

	// Proxy에 SquadComponent 부착
	USquadComponent* Squad = NewObject<USquadComponent>(Proxy, TEXT("SquadComponent"));
	Squad->RegisterComponent();
	Squad->Proxy = Proxy;
	Proxy->OwnerSquad = Squad;

	Squad->SetSquadNumber(FreeIdx);
	SquadList[FreeIdx] = Squad;

	UE_LOG(LogTemp, Log, TEXT("[Squad] Empty squad #%d created"), Squad->GetSquadNumber());
	return Squad;
}

void ADwarfUndergroundPlayerController::RemoveSquad(int32 SquadNum)
{
	USquadComponent* Squad = SquadList[SquadNum];

	if (!Squad) return;

	// 모든 유닛 Detach
	TArray<AUnit*> UnitsCopy = Squad->Units;
	for (AUnit* U : UnitsCopy)
		Squad->RemoveUnit(U);

	// Proxy 제거
	if (Squad->Proxy) { Squad->Proxy->Destroy(); }

	// 컴포넌트 해제
	Squad->DestroyComponent();
	SquadList[SquadNum] = nullptr;

	UE_LOG(LogTemp, Log, TEXT("[Squad] Squad #%d removed"), SquadNum);
}

void ADwarfUndergroundPlayerController::AddUnitToSquad(int32 SquadNum, AUnit* Unit)
{
	USquadComponent* Squad = SquadList[SquadNum];

	if (!Squad || !Unit) return;

	// 다른 부대에 속해 있다면 우선 탈퇴
	if (Unit->SquadRef && Unit->SquadRef != Squad)
		Unit->SquadRef->RemoveUnit(Unit);

	Squad->AddUnit(Unit);
	Unit->SquadIdx = SquadNum;
}

void ADwarfUndergroundPlayerController::RemoveUnitFromSquad(int32 SquadNum, AUnit* Unit)
{
	USquadComponent* Squad = SquadList[SquadNum];

	if (!Squad || !Unit) return;
	if (Unit->SquadRef == Squad)
	{
		Squad->RemoveUnit(Unit);
		Unit->SquadIdx = -1;
	}
}

TArray<USquadComponent*> ADwarfUndergroundPlayerController::GetSquadList() {
	return SquadList;
}

void ADwarfUndergroundPlayerController::HandleAllDead()
{
	UE_LOG(LogTemp, Display, TEXT("ALL DEAD"));
	UGameplayStatics::OpenLevel(this, "EndingLevel");
}

void ADwarfUndergroundPlayerController::SelectBuilding(AActor* Building)
{
	ABuildingBase* BuildingBase = Cast<ABuildingBase>(Building);
	if (!BuildingBase)
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectBuilding: 전달된 액터가 BuildingBase가 아닙니다."));
		return;
	}

	// 이전에 선택된 건물이 있다면 선택 해제
	if (CurrentSelectedBuilding && CurrentSelectedBuilding != BuildingBase)
	{
		CurrentSelectedBuilding->SetSelected(false);
		UE_LOG(LogTemp, Log, TEXT("이전 선택 건물 해제: %s"), *CurrentSelectedBuilding->GetBuildingName());
	}

	// 새 건물 선택
	CurrentSelectedBuilding = BuildingBase;
	CurrentSelectedBuilding->SetSelected(true);

	UE_LOG(LogTemp, Log, TEXT("건물 선택됨: %s"), *CurrentSelectedBuilding->GetBuildingName());
}

void ADwarfUndergroundPlayerController::DeselectCurrentBuilding()
{
	if (CurrentSelectedBuilding)
	{
		CurrentSelectedBuilding->SetSelected(false);
		UE_LOG(LogTemp, Log, TEXT("건물 선택 해제: %s"), *CurrentSelectedBuilding->GetBuildingName());
		CurrentSelectedBuilding = nullptr;
	}
}