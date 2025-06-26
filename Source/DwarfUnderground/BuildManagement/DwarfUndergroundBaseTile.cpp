#include "DwarfUndergroundBaseTile.h"
#include "DwarfUndergroundBuildManager.h"
#include "../DwarfUndergroundPlayerController.h"
#include "Buildings/BuildingBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "DwarfUnderground/Item/InventorySubSystem.h"
#include "DwarfUnderground/UI/LogUI.h"
#include "Engine/World.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

ADwarfUndergroundBaseTile::ADwarfUndergroundBaseTile()
{
    PrimaryActorTick.bCanEverTick = true;

    HexTile = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HexTile"));
    RootComponent = HexTile;
    HexTile->SetGenerateOverlapEvents(true);
    HexTile->SetCollisionResponseToAllChannels(ECR_Ignore);
    HexTile->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    HexTile->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    HexTile->OnBeginCursorOver.AddDynamic(this, &ADwarfUndergroundBaseTile::OnTileBeginCursorOver);
    HexTile->OnEndCursorOver.AddDynamic(this, &ADwarfUndergroundBaseTile::OnTileEndCursorOver);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> BuildBaseMesh(TEXT("/Game/Building/Meshes/SM_BaseHexTile.SM_BaseHexTile"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BuildBaseMat(TEXT("/Game/Building/Materials/M_BaseHexTile.M_BaseHexTile"));

    if (BuildBaseMesh.Succeeded())
    {
        HexTile->SetStaticMesh(BuildBaseMesh.Object);
    }

    if (BuildBaseMat.Succeeded())
    {
        HexTile->SetMaterial(0, BuildBaseMat.Object);
    }

    // Preview Building Material
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> PreviewBuildingMatRef(
        TEXT("/Game/Building/Materials/M_BuildPreview.M_BuildPreview")
    );
    if (PreviewBuildingMatRef.Succeeded())
    {
        PreviewBuildingMat = PreviewBuildingMatRef.Object;
    }
}

void ADwarfUndergroundBaseTile::BeginPlay()
{
    Super::BeginPlay();

    UMaterialInterface* TileMat = HexTile->GetMaterial(0);
    if (TileMat)
    {
        DynamicMaterial_BaseTile = UMaterialInstanceDynamic::Create(TileMat, this);
        if (DynamicMaterial_BaseTile)
        {
            HexTile->SetMaterial(0, DynamicMaterial_BaseTile);
            DynamicMaterial_BaseTile->SetVectorParameterValue(TEXT("ColorParam"), NormalColor);
        }
    }
}

void ADwarfUndergroundBaseTile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}



bool ADwarfUndergroundBaseTile::BuildOnTile(TSubclassOf<AActor> InType)
{
    if (!GetWorld()) return false;
    if (!InType)
    {
        UE_LOG(LogTemp, Error, TEXT("BaseTile: BuildOnTile 실패, 생성하고자 하는 Type이 존재하지 않습니다."));
        return false;
    }

    //빌드하고자 하는 값이 가능한지 확인
    if (auto* Inven = GetGameInstance()->GetSubsystem<UInventorySubSystem>())
    {
        if (!Inven->CheckEnoughPriceToBuild(InType))
        {
            ULogUI::Log(TEXT("건설에 필요한 자원이 부족합니다."));
            return false;
        }
    }

    ADwarfUndergroundBuildManager* BuildManager = nullptr;
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADwarfUndergroundBuildManager::StaticClass(), FoundActors);

    if (FoundActors.Num() <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildOnTile: BuildManager를 찾을 수 없음"));
        return false;
    }

    BuildManager = Cast<ADwarfUndergroundBuildManager>(FoundActors[0]);
    if (!BuildManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildOnTile: BuildManager 캐스팅 실패"));
        return false;
    }

    AActor* DefaultActor = InType->GetDefaultObject<AActor>();
    if (!DefaultActor)
    {
        UE_LOG(LogTemp, Error, TEXT("BaseTile: 생성하고자 하는 건물의 DefaultActor 찾을 수 없음"));
        return false;
    }

    //BuildComponent가 존재해야만 건설이 가능하므로, 대상 클래스가 IBuildable인지 확인
    if (!InType->ImplementsInterface(UBuildable::StaticClass()))
    {
        UE_LOG(LogTemp, Error, TEXT("BaseTile: 생성하고자 하는 건물이 Buildable이 아님(인터페이스 미구현): %s"), *InType->GetName());
        return false;
    }

    // 건물 타입에 따른 크기 설정
    int32 BuildingWidth = 1;
    int32 BuildingHeight = 1;
    if (const IBuildable* Buildable = Cast<IBuildable>(DefaultActor))
    {
        BuildingWidth = Buildable->GetWidth();
        BuildingHeight = Buildable->GetHeight();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseTile:BuildOnTile: 건설 대상에서 Width, Height 추출불가: Buildable이 아닙니다."));
    }

    // 기본 배치 가능 여부만 확인 (경사로 체크 제거)
    if (!BuildManager->CanPlaceBuilding(this, BuildingWidth, BuildingHeight))
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildOnTile: 해당 위치에 건물 배치 불가"));
        return false;
    }

    TArray<ADwarfUndergroundBaseTile*> RequiredTiles = BuildManager->GetAdjacentTiles(this, BuildingWidth, BuildingHeight);

    if (RequiredTiles.Num() != BuildingWidth * BuildingHeight)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildOnTile: 필요한 타일 수 부족 (요구: %d, 실제: %d)"),
            BuildingWidth * BuildingHeight, RequiredTiles.Num());
        return false;
    }

    for (ADwarfUndergroundBaseTile* Tile : RequiredTiles)
    {
        if (!Tile || Tile->bIsOccupied)
        {
            UE_LOG(LogTemp, Warning, TEXT("BuildOnTile: 타일이 없거나 이미 점유됨"));
            return false;
        }
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildOnTile: World가 유효하지 않음"));
        return false;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FVector BuildLocation;
    FRotator BuildRotation = FRotator::ZeroRotator;

    // 흙인 경우 특별한 위치 계산
    if (InType->IsChildOf<ASoil>())
    {
        // 층의 기본 높이 찾기
        float LevelBaseZ = BaseStoneZ;
        float SoilThickness = 75.0f; // 기본값

        if (BuildManager)
        {
            for (int32 i = 0; i < BuildManager->LevelTilesArray.Num(); i++)
            {
                if (BuildManager->LevelTilesArray[i].Tiles.Contains(this))
                {
                    LevelBaseZ = BuildManager->LevelTilesArray[i].LevelZ;
                    break;
                }
            }

            // SoilLayerThickness 안전하게 가져오기
            if (BuildManager->SoilLayerThickness > 0.0f)
            {
                SoilThickness = BuildManager->SoilLayerThickness;
            }
        }

        // 흙 위치 = 층 기본 높이 + (현재 흙 스택 개수 * 흙 층 두께)
        FVector TileXY = GetActorLocation();
        BuildLocation = FVector(TileXY.X, TileXY.Y, LevelBaseZ + (SoilStack.Num() * SoilThickness));

        UE_LOG(LogTemp, Warning, TEXT("흙 위치 계산: 층기본Z=%.1f, 현재스택=%d, 두께=%.1f, 계산된Z=%.1f"),
            LevelBaseZ, SoilStack.Num(), SoilThickness, BuildLocation.Z);
    }
    else
    {
        // 일반 건물은 기존 방식
        BuildLocation = GetActorLocation() + FVector(0.f, 0.f, -10.f);
    }

    UE_LOG(LogTemp, Warning, TEXT("BuildOnTile: 건물 스폰 시도"));
    AActor* NewBuilding = World->SpawnActor<AActor>(
        InType,
        BuildLocation,
        BuildRotation,
        SpawnParams
    );

    if (NewBuilding)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildOnTile: 건물 스폰 성공"));

        if (InType->IsChildOf<ASoil>())
        {
            // 흙 전용 스케일 설정
            const float ScaleX = 200.f * 1.175 / 200.f;  // TILE_WIDTH * 1.2 / SoilMeshBaseSize
            const float ScaleY = 200.f * 1.175 / 200.f;  // TILE_HEIGHT * 1.2 / SoilMeshBaseSize
            const float ScaleZ = 60.f * 2.5 / 200.f;   // SoilLayerThickness * 2.5 / SoilMeshBaseSize
            NewBuilding->SetActorScale3D(FVector(ScaleX, ScaleY, ScaleZ));

            // 흙을 설치했을 때 기존 방식과 동일하게 처리
            ASoil* NewSoil = Cast<ASoil>(NewBuilding);
            if (NewSoil)
            {
                // 기존 SpawnSoilOnTile 방식과 동일한 관계 설정
                NewSoil->AddOccupiedTile(this);  // ← 이 부분이 누락되어 있었음
                SoilStack.Add(NewSoil);          // AddSoil 대신 직접 추가

                // 위치 업데이트 (기존 방식과 동일)
                if (BuildManager)
                {
                    BuildManager->UpdateTilePositionAboveSoil(this);
                    BuildManager->UpdateNavMeshAroundLocation(NewBuilding->GetActorLocation());
                }

                UE_LOG(LogTemp, Warning, TEXT("★ BuildOnTile: 흙 설치 완료 (기존 방식과 동일)"));
            }
        }
        else
        {
            // 기존 건물 스케일 로직
            if (BuildingWidth > 1 || BuildingHeight > 1)
            {
                //스케일 설정
                NewBuilding->SetActorScale3D(FVector(BuildingWidth, BuildingHeight, 1.0f));

                // 위치 조정 (새로운 위치 계산)
                FVector NewLocation = BuildLocation;
                float TileWidth = 200.f;
                float TileHeight = 173.2f;

                // 멀티타일 건물의 중심 계산
                NewLocation.X += TileWidth * (0.5f * (BuildingWidth - 1));
                NewLocation.Y += TileHeight * (0.5f * (BuildingHeight - 1));

                // 계산된 새 위치로 건물 이동
                NewBuilding->SetActorLocation(NewLocation);
                UE_LOG(LogTemp, Warning, TEXT("건물 위치 조정: 원래(%s) -> 조정(%s)"),
                    *BuildLocation.ToString(), *NewLocation.ToString());
            }
        }

        IBuildable* NewBuildable = Cast<IBuildable>(NewBuilding);
        for (ADwarfUndergroundBaseTile* Tile : RequiredTiles)
        {
            if (Tile)
            {
                // 흙이 아닌 경우만 타일을 점유 상태로 설정
                if (!InType->IsChildOf<ASoil>())
                {
                    Tile->bIsOccupied = true;
                    Tile->OccupyingBuilding = NewBuilding;
                }
                NewBuildable->AddOccupiedTile(Tile);
            }
        }

        // 흙이 아닌 경우만 PlacedBuilding 설정
        if (!InType->IsChildOf<ASoil>())
        {
            PlacedBuilding = NewBuilding;
        }

        UE_LOG(LogTemp, Warning, TEXT("BuildOnTile: 건물 배치 완료, 이름: %s"), *NewBuilding->GetName());
        
        //자원 소모
        auto* Inventory = GetGameInstance()->GetSubsystem<UInventorySubSystem>();
        Inventory->ConsumeBuildResource(InType);
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BuildOnTile: 건물 스폰 실패"));
        return false;
    }
}

bool ADwarfUndergroundBaseTile::RemoveBuilding()
{
    if (!OccupyingBuilding && !PlacedBuilding)
    {
        return false;
    }

    TScriptInterface<IBuildable> BuildingToRemove = OccupyingBuilding ? OccupyingBuilding : PlacedBuilding;

    if (!BuildingToRemove)
    {
        return false;
    }

    // 제거할 건물이 흙인지 확인
    UObject* RemoveObject = BuildingToRemove.GetObject();
    bool bIsSoil = false;
    ASoil* SoilToRemove = nullptr;

    if (AActor* RemoveActor = Cast<AActor>(RemoveObject))
    {
        SoilToRemove = Cast<ASoil>(RemoveActor);
        bIsSoil = (SoilToRemove != nullptr);
    }

    // 흙인 경우 특별 처리
    if (bIsSoil && SoilToRemove)
    {
        // RemoveSoil 함수를 사용하여 흙 제거 (타일 위치 자동 업데이트 포함)
        bool bRemoveSuccess = RemoveSoil(SoilToRemove);

        // PlacedBuilding에서도 제거
        if (PlacedBuilding == BuildingToRemove)
        {
            PlacedBuilding = nullptr;
        }

        return bRemoveSuccess;
    }
    else
    {
        // 일반 건물인 경우 기존 로직 사용
        for (ADwarfUndergroundBaseTile* Tile : BuildingToRemove->GetOccupiedTiles())
        {
            if (Tile)
            {
                Tile->bIsOccupied = false;
                Tile->OccupyingBuilding = nullptr;
                if (Tile->PlacedBuilding == BuildingToRemove)
                {
                    Tile->PlacedBuilding = nullptr;
                }
            }
        }

        //대상 빌딩 삭제
        if (AActor* RemoveActor = Cast<AActor>(RemoveObject))
        {
            RemoveActor->Destroy();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("BaseTile: 빌딩 삭제 실패: 빌더블은 존재하지만 액터가 아닙니다."));
        }

        if (OccupyingBuilding == BuildingToRemove)
        {
            OccupyingBuilding = nullptr;
        }

        if (PlacedBuilding == BuildingToRemove)
        {
            PlacedBuilding = nullptr;
        }
    }

    return true;
}

void ADwarfUndergroundBaseTile::OnTileBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
    if (!GetWorld()) return;

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    ADwarfUndergroundPlayerController* MyController = Cast<ADwarfUndergroundPlayerController>(PC);
    if (!MyController) return;

    if (MyController->GetBuildSelectionMode() == 0)
    {
        ShowPreview();
    }

    if (DynamicMaterial_BaseTile)
    {
        if (MyController->GetBuildSelectionMode() == 0)
        {
            DynamicMaterial_BaseTile->SetVectorParameterValue(TEXT("ColorParam"), BuildModeColor);
        }
        else if (MyController->GetBuildSelectionMode() == 1)
        {
            DynamicMaterial_BaseTile->SetVectorParameterValue(TEXT("ColorParam"), DestroyModeColor);
        }
    }
}

void ADwarfUndergroundBaseTile::OnTileEndCursorOver(UPrimitiveComponent* TouchedComponent)
{
    HidePreview();
    if (DynamicMaterial_BaseTile)
    {
        DynamicMaterial_BaseTile->SetVectorParameterValue(TEXT("ColorParam"), NormalColor);
    }
}

void ADwarfUndergroundBaseTile::ShowPreview()
{
    if (bIsOccupied || PreviewBuilding != nullptr)
    {
        return;
    }

    // 플레이어 컨트롤러에서 현재 선택된 빌딩 타입 가져오기
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    ADwarfUndergroundPlayerController* MyPC = Cast<ADwarfUndergroundPlayerController>(PC);
    if (!MyPC) return;

    TSubclassOf<AActor> SelectedType = MyPC->GetBuildingType();
    if (!SelectedType)
    {
        UE_LOG(LogTemp, Error, TEXT("BaseTile: 미리보기 생성실패: BuildingType이 설정되지 않음"));
        return;
    }

    // 빌드 매니저 참조 가져오기
    ADwarfUndergroundBuildManager* BuildManager = nullptr;
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADwarfUndergroundBuildManager::StaticClass(), FoundActors);
    if (FoundActors.Num() > 0)
    {
        BuildManager = Cast<ADwarfUndergroundBuildManager>(FoundActors[0]);
    }

    if (!BuildManager)
        return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;

    FVector BuildLocation = GetActorLocation() + FVector(0.f, 0.f, 10.f);
    FRotator BuildRotation = FRotator::ZeroRotator;

    PreviewBuilding = GetWorld()->SpawnActor<AStaticMeshActor>(
        AStaticMeshActor::StaticClass(),
        BuildLocation,
        BuildRotation,
        SpawnParams
    );

    if (PreviewBuilding)
    {
        //미리보기용 액터&스태틱메시컴포넌트 관련 값 초기화
        UStaticMeshComponent* PreviewMeshComponent = PreviewBuilding->GetComponentByClass<UStaticMeshComponent>();
        if (!PreviewMeshComponent)
        {
            UE_LOG(LogTemp, Error, TEXT("BaseTile: ShowPreview: StaticMeshComponent 탐색 실패"));
            return;
        }
        PreviewMeshComponent->SetMobility(EComponentMobility::Movable);
        PreviewBuilding->SetActorEnableCollision(false);

        //스폰된 미리보기용 액터에 StaticMesh 할당
        //설치하고자 하는 액터의 CDO 받아와서 Mesh 추출
        const AActor* BuildableDefaultObject = SelectedType->GetDefaultObject<AActor>();
        if (!BuildableDefaultObject)
        {
            UE_LOG(LogTemp, Error, TEXT("BaseTile: 건설 대상의 CDO가 없습니다. %s"), *SelectedType->GetName());
            return;
        }
        const UStaticMeshComponent* BuildableDefaultComponent = BuildableDefaultObject->FindComponentByClass<UStaticMeshComponent>();
        if (!BuildableDefaultComponent)
        {
            UE_LOG(LogTemp, Error, TEXT("BaseTile: 건설 대상 기본 오브젝트에서 메쉬를 찾을 수 없습니다. %s"), *SelectedType->GetName());
            return;
        }

        //Mesh 추출 성공 시 PreviewBuilding에 할당
        UStaticMesh* PreviewMesh = BuildableDefaultComponent->GetStaticMesh();
        if (!PreviewMesh)
        {
            UE_LOG(LogTemp, Error, TEXT("BaseTile: 건설 대상에 메쉬가 없습니다. Preview 실패"));
            return;
        }
        PreviewMeshComponent->SetStaticMesh(PreviewMesh);

        // 건물 타입에 따른 크기 설정
        int32 BuildingWidth = 1;
        int32 BuildingHeight = 1;
        if (const IBuildable* Buildable = Cast<IBuildable>(BuildableDefaultObject))
        {
            BuildingWidth = Buildable->GetWidth();
            BuildingHeight = Buildable->GetHeight();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("BaseTile:ShowPreview: 건설 대상이 Buildable이 아닙니다."));
        }

        // 배치 가능한지 확인 (경사로 체크 제거됨)
        bool bCanPlace = BuildManager->CanPlaceBuilding(this, BuildingWidth, BuildingHeight);

        //Material 설정
        UMaterialInterface* PreviewMat = PreviewBuildingMat;
        if (!bCanPlace)
        {
            // 배치 불가능할 때 다른 색상의 머티리얼 사용 가능
        }

        if (PreviewMat)
        {
            PreviewMeshComponent->SetMaterial(0, PreviewMat);
        }

        // 크기 설정
        if (BuildingWidth > 1 || BuildingHeight > 1)
        {
            // 스케일 설정
            PreviewBuilding->SetActorScale3D(FVector(BuildingWidth, BuildingHeight, 1.0f));

            // 위치 조정 (새로운 위치 계산)
            FVector NewLocation = BuildLocation;
            float TileWidth = 200.f;
            float TileHeight = 173.2f;

            NewLocation.X += TileWidth * 0.5f * (BuildingWidth - 1);
            NewLocation.Y += TileHeight * 0.5f * (BuildingHeight - 1);

            // 계산된 새 위치로 미리보기 건물 이동
            PreviewBuilding->SetActorLocation(NewLocation);
        }
    }
}

void ADwarfUndergroundBaseTile::HidePreview()
{
    if (PreviewBuilding)
    {
        PreviewBuilding->Destroy();
        PreviewBuilding = nullptr;
    }
}

void ADwarfUndergroundBaseTile::SetTileOpacity(float InOpacity)
{
    if (DynamicMaterial_BaseTile)
    {
        DynamicMaterial_BaseTile->SetScalarParameterValue(TEXT("OpacityParam"), InOpacity);
    }
}

bool ADwarfUndergroundBaseTile::AddSoil(ASoil* NewSoil)
{
    if (!NewSoil)
    {
        UE_LOG(LogTemp, Error, TEXT("AddSoil: NewSoil이 nullptr입니다."));
        return false;
    }

    UE_LOG(LogTemp, Warning, TEXT("★ AddSoil 시작: 타일 (%d,%d), 현재 스택: %d"),
        GridX, GridY, SoilStack.Num());

    // 새 흙을 스택에 추가
    SoilStack.Add(NewSoil);

    UE_LOG(LogTemp, Warning, TEXT("★ 흙 스택에 추가됨. 새 스택 크기: %d"), SoilStack.Num());

    // 흙 추가 후 즉시 타일 위치 업데이트
    if (UWorld* World = GetWorld())
    {
        ADwarfUndergroundBuildManager* BuildManager = nullptr;
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(World, ADwarfUndergroundBuildManager::StaticClass(), FoundActors);

        if (FoundActors.Num() > 0)
        {
            BuildManager = Cast<ADwarfUndergroundBuildManager>(FoundActors[0]);
            if (BuildManager)
            {
                UE_LOG(LogTemp, Warning, TEXT("★ BuildManager를 통한 타일 위치 업데이트 호출"));
                BuildManager->UpdateTilePositionAboveSoil(this);

                // 직접 위치 업데이트도 시도
                UpdateTilePosition();

                UE_LOG(LogTemp, Warning, TEXT("★ 흙 추가 후 타일 위치 업데이트 완료: (%d,%d)"), GridX, GridY);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("BuildManager 캐스팅 실패"));
                // 폴백: 직접 위치 업데이트
                UpdateTilePosition();
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("BuildManager를 찾을 수 없음"));
            // 폴백: 직접 위치 업데이트
            UpdateTilePosition();
        }
    }

    UE_LOG(LogTemp, Verbose, TEXT("흙 층 추가 완료: 타일=(%d,%d), 최종 스택 크기=%d"),
        GridX, GridY, SoilStack.Num());

    return true;
}

bool ADwarfUndergroundBaseTile::RemoveSoil(ASoil* SoilToRemove)
{
    if (!SoilToRemove) return false;

    // 제거할 흙의 인덱스 찾기
    int32 soilIndex = SoilStack.Find(SoilToRemove);
    if (soilIndex == INDEX_NONE) return false;

    // 흙 스택에서 제거
    SoilStack.RemoveAt(soilIndex);

    // 해당 흙 객체 제거
    SoilToRemove->Destroy();

    // 제거된 흙 이후의 층들을 아래로 이동시키기 위해
    // 빌드 매니저의 조정 함수 호출
    if (UWorld* World = GetWorld())
    {
        ADwarfUndergroundBuildManager* BuildManager = nullptr;
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(World, ADwarfUndergroundBuildManager::StaticClass(), FoundActors);

        if (FoundActors.Num() > 0)
        {
            BuildManager = Cast<ADwarfUndergroundBuildManager>(FoundActors[0]);
            if (BuildManager)
            {
                BuildManager->AdjustTilePositionByLevel(this);
            }
            else
            {
                // 빌드 매니저를 찾을 수 없는 경우 기존 함수로 폴백
                UpdateTilePosition();
            }
        }
        else
        {
            UpdateTilePosition();
        }
    }

    return true;
}

float ADwarfUndergroundBaseTile::CalculateTopSoilZ() const
{
    // 빌드 매니저를 통해 타일이 속한 층의 기본 높이 찾기
    if (UWorld* World = GetWorld())
    {
        ADwarfUndergroundBuildManager* BuildManager = nullptr;
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(World, ADwarfUndergroundBuildManager::StaticClass(), FoundActors);

        if (FoundActors.Num() > 0)
        {
            BuildManager = Cast<ADwarfUndergroundBuildManager>(FoundActors[0]);
            if (BuildManager)
            {
                // 타일이 속한 층의 기본 높이 찾기
                for (int32 i = 0; i < BuildManager->LevelTilesArray.Num(); i++)
                {
                    if (BuildManager->LevelTilesArray[i].Tiles.Contains(const_cast<ADwarfUndergroundBaseTile*>(this)))
                    {
                        float LevelBaseZ = BuildManager->LevelTilesArray[i].LevelZ;

                        if (SoilStack.Num() == 0)
                        {
                            return LevelBaseZ; // 흙이 없으면 층 기본 높이
                        }

                        // 층 기본 높이 + (흙 개수 * 흙 두께)
                        return LevelBaseZ + (SoilStack.Num() * 60.0f); // SoilLayerThickness 사용
                    }
                }
            }
        }
    }

    // 빌드 매니저를 찾을 수 없는 경우 BaseStoneZ 사용 (폴백)
    if (SoilStack.Num() == 0)
    {
        return BaseStoneZ;
    }
    return BaseStoneZ + (SoilStack.Num() * 60.0f);
}

void ADwarfUndergroundBaseTile::UpdateTilePosition()
{
    // 빌드 매니저를 통해 올바른 위치 계산
    if (UWorld* World = GetWorld())
    {
        ADwarfUndergroundBuildManager* BuildManager = nullptr;
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(World, ADwarfUndergroundBuildManager::StaticClass(), FoundActors);

        if (FoundActors.Num() > 0)
        {
            BuildManager = Cast<ADwarfUndergroundBuildManager>(FoundActors[0]);
            if (BuildManager)
            {
                BuildManager->AdjustTilePositionByLevel(this);
                return;
            }
        }
    }

    // 빌드 매니저를 찾을 수 없는 경우 직접 계산
    float newZ = CalculateTopSoilZ() + 10.0f; // 흙 위 10.0f 간격

    FVector currentLocation = GetActorLocation();
    float deltaZ = newZ - currentLocation.Z;

    currentLocation.Z = newZ;
    SetActorLocation(currentLocation);

    // 건물 위치 조정
    if (PlacedBuilding)
    {
        if (AActor* BuildingActor = PlacedBuilding.GetInterface()->GetActor())
        {
            FVector buildingLocation = BuildingActor->GetActorLocation();
            buildingLocation.Z += deltaZ;
            BuildingActor->SetActorLocation(buildingLocation);
        }
    }

    if (PreviewBuilding)
    {
        FVector previewLocation = PreviewBuilding->GetActorLocation();
        previewLocation.Z += deltaZ;
        PreviewBuilding->SetActorLocation(previewLocation);
    }
}