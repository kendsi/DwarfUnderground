#include "BuildingBase.h"

#include "GroomVisualizationData.h"
#include "DwarfUnderground/DwarfUndergroundPlayerController.h"
#include "DwarfUnderground/Subsystem/BuildingManagerSubsystem.h"
#include "DwarfUnderground/UI/LogUI.h"

// Sets default values
ABuildingBase::ABuildingBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//해당 값은 하위 오브젝트에서 초기화되어야함.
	Width = 1;
	Height = 1;

	//이하 코드는 기존 DwarfUndergroundBuild 클래스에서 가져옴
	// - BuildingBase의 RootComponent로 StaticMeshComponent 지정 및 기타 초기화
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
	if (BuildingMesh != nullptr)
	{
		RootComponent = BuildingMesh;
		BuildingMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
		BuildingMesh->SetCanEverAffectNavigation(true);

		BuildingMesh->OnBeginCursorOver.AddDynamic(this, &ABuildingBase::OnDestroyBeginCursorOver);
		BuildingMesh->OnEndCursorOver.AddDynamic(this, &ABuildingBase::OnDestroyEndCursorOver);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: 액터에 StaticMeshComponent가 없습니다."), *GetName());
	}

	//삭제모드 마우스 오버 시 
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DestroyPreviewMatRef(
		TEXT("/Game/Building/Materials/M_DestroyPreview.M_DestroyPreview")
	);
	if (DestroyPreviewMatRef.Succeeded())
	{
		DestroyPreviewMat = DestroyPreviewMatRef.Object;
	}

	AttachConstructionWidgetComponent();
	ProgressTimer = 0.0f;
	bIsSelected = false;
}

// Called when the game starts or when spawned
void ABuildingBase::BeginPlay()
{
	Super::BeginPlay();
	FindAndGetConstructionProgressBarComponent();

	// 원본 머티리얼 저장 및 CustomDepth 설정
	if (BuildingMesh)
	{
		OriginalMaterial = BuildingMesh->GetMaterial(0);

		// Custom Depth 렌더링 설정 (기본적으로 비활성화)
		BuildingMesh->SetRenderCustomDepth(false);
		BuildingMesh->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_Default);
		BuildingMesh->SetCustomDepthStencilValue(OutlineStencilValue);
	}

	//빌딩 추가됨을 알림
	if (UGameInstance* GI = GetGameInstance())
	{
		BuildingManager = GI->GetSubsystem<UBuildingManagerSubsystem>();
		BuildingManager->RegisterBuilding(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BuildingBase: GameInstanceSubsystem을 찾을 수 없습니다."));
	}
}

void ABuildingBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//빌딩 삭제됨을 알림
	if (BuildingManager)
	{
		BuildingManager->UnregisterBuilding(this);
	}
}

// Called every frame
void ABuildingBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//건설 로딩바 테스트용
	if (ProgressTimer >= 1.0f) return;
	if (ProgressTimer == 0.0f) ConstructionProgressBarWidget->BeginProgress();
	ProgressTimer += DeltaTime;
	ConstructionProgressBarWidget->UpdateProgress(ProgressTimer);
	if (ProgressTimer >= 1.0f) ConstructionProgressBarWidget->EndProgress();
}

void ABuildingBase::OnDestroyBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	ADwarfUndergroundPlayerController* MyController = Cast<ADwarfUndergroundPlayerController>(PC);
	if (MyController->GetBuildSelectionMode() == 1)
	{
		if (DestroyPreviewMat && BuildingMesh)
		{
			BuildingMesh->SetMaterial(0, DestroyPreviewMat);
		}
	}
}

void ABuildingBase::OnDestroyEndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	// Destroy Mode가 아닐 때만 원래 머티리얼로 복원
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	ADwarfUndergroundPlayerController* MyController = Cast<ADwarfUndergroundPlayerController>(PC);

	if (MyController && MyController->GetBuildSelectionMode() != 1)
	{
		// 선택 상태가 아닐 때만 원래 머티리얼로 복원
		if (!bIsSelected && BuildingMesh)
		{
			BuildingMesh->SetMaterial(0, OriginalMaterial);
		}
	}
}

void ABuildingBase::AttachConstructionWidgetComponent()
{
	ConstructionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	ConstructionWidgetComponent->SetupAttachment(GetRootComponent());
	ConstructionWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));

	static ConstructorHelpers::FClassFinder<UUserWidget> ProgressBarRef = TEXT("/Game/UI/InGameUI/BP_BuildProgressBar");
	if (ProgressBarRef.Class)
	{
		ConstructionWidgetComponent->SetWidgetClass(ProgressBarRef.Class);
		ConstructionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		ConstructionWidgetComponent->SetDrawSize(FVector2D(90.0f, 5.0f));
		ConstructionWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ABuildingBase::FindAndGetConstructionProgressBarComponent()
{
	UUserWidget* uw = ConstructionWidgetComponent->GetUserWidgetObject();
	if (uw)
	{
		ConstructionProgressBarWidget = Cast<UBuildProgressBarWidget>(uw);
	}
	if (ConstructionProgressBarWidget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("건설 로딩바 찾지 못함"));
	}
}

FVector ABuildingBase::GetEntrancePosition()
{
	if (Entrance.IsValid())
	{
		return Entrance.Get()->GetComponentLocation();
	}
	else
	{
		TArray<UActorComponent*> Components = GetComponentsByTag(USceneComponent::StaticClass(), FName("Entrance"));
		if (Components.Num() > 0)
		{
			if (auto* Ent = Cast<USceneComponent>(Components[0]))
			{
				Entrance = Ent;
				return Entrance->GetComponentLocation();
			}
		ULogUI::Log(TEXT("찾긴함"));
		}
		return GetOwner()->GetActorLocation();
	}
}

void ABuildingBase::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;

	if (!BuildingMesh)
		return;

	if (bIsSelected)
	{
		// 선택 시 Custom Depth 렌더링 활성화 (후처리 외곽선용)
		BuildingMesh->SetRenderCustomDepth(true);
		UE_LOG(LogTemp, Log, TEXT("%s 건물이 선택되었습니다. CustomDepth 활성화"), *GetBuildingName());
	}
	else
	{
		// 선택 해제 시 Custom Depth 렌더링 비활성화
		BuildingMesh->SetRenderCustomDepth(false);
		UE_LOG(LogTemp, Log, TEXT("%s 건물 선택이 해제되었습니다. CustomDepth 비활성화"), *GetBuildingName());
	}
}

FString ABuildingBase::GetBuildingName()
{
	return BuildingName;
}

int32 ABuildingBase::GetWidth() const
{
	return Width;
}

int32 ABuildingBase::GetHeight() const
{
	return Height;
}

void ABuildingBase::AddOccupiedTile(ADwarfUndergroundBaseTile* Tile)
{
	OccupiedTiles.Add(Tile);
}

void ABuildingBase::RemoveOccupiedTile(ADwarfUndergroundBaseTile* Tile)
{
	OccupiedTiles.Remove(Tile);
}

TArray<ADwarfUndergroundBaseTile*> ABuildingBase::GetOccupiedTiles()
{
	return OccupiedTiles;
}

AActor* ABuildingBase::GetActor()
{
	return this;
}

void ABuildingBase::SetWidth(const int32 NewWidth)
{
	Width = NewWidth;
}

void ABuildingBase::SetHeight(const int32 NewHeight)
{
	Height = NewHeight;
}