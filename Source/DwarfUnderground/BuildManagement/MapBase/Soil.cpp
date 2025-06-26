#include "Soil.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "../DwarfUndergroundBuildManager.h"
#include "Kismet/GameplayStatics.h"

ASoil::ASoil()
{
    PrimaryActorTick.bCanEverTick = false;
    SoilMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SoilMesh"));
    RootComponent = SoilMesh;

    Width = 1;
    Height = 1;

    // 기본 콜리전 설정
    SoilMesh->SetBoundsScale(2.0f);                    // 바운딩 박스 확대
    SoilMesh->SetForcedLodModel(1);                    // LOD 고정
    SoilMesh->bUseAsOccluder = false;                  // 오클루더로 사용 안함
    SoilMesh->SetCanEverAffectNavigation(false);       // 네비게이션 영향 없음
    SoilMesh->SetGenerateOverlapEvents(false);         // 오버랩 이벤트 없음
    SoilMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SoilMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    SoilMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    SoilMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    SoilMesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Block);
    SoilMesh->SetCanEverAffectNavigation(true);
    SoilMesh->bFillCollisionUnderneathForNavmesh = true;

    // 기본 흙 메시 로드
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SoilMeshRef(
        TEXT("/Game/Building/Meshes/MapBase/New2_soil.New2_soil")
    );
    if (SoilMeshRef.Succeeded())
    {
        NormalSoilMesh = SoilMeshRef.Object;
        SoilMesh->SetStaticMesh(NormalSoilMesh);
    }

    // 기본 흙 재질 로드
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> NormalSoilMatRef(
        TEXT("/Game/Building/Meshes/MapBase/Material_003.Material_003")
    );
    if (NormalSoilMatRef.Succeeded())
    {
        NormalSoilMaterial = NormalSoilMatRef.Object;
        SoilMesh->SetMaterial(0, NormalSoilMaterial);
    }
}

void ASoil::BeginPlay()
{
    Super::BeginPlay();

    // NavMesh에 영향주기
    if (SoilMesh)
    {
        SoilMesh->SetCanEverAffectNavigation(true);
        SoilMesh->ComponentTags.Add(FName("NavMeshGeneration"));
    }
}

void ASoil::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

int32 ASoil::GetWidth() const
{
    return Width;
}

int32 ASoil::GetHeight() const
{
    return Height;
}

void ASoil::AddOccupiedTile(ADwarfUndergroundBaseTile* Tile)
{
    OccupiedTiles.Add(Tile);
}

void ASoil::RemoveOccupiedTile(ADwarfUndergroundBaseTile* Tile)
{
    OccupiedTiles.Remove(Tile);
}

TArray<ADwarfUndergroundBaseTile*> ASoil::GetOccupiedTiles()
{
    return OccupiedTiles;
}

AActor* ASoil::GetActor()
{
    return this;
}

void ASoil::SetSoilType(ESoilType NewType)
{
    SoilType = NewType;

    // Normal 타입만 지원하므로 단순화
    if (NewType == ESoilType::Normal)
    {
        if (NormalSoilMesh)
        {
            SoilMesh->SetVisibility(true);
            SoilMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            SoilMesh->SetStaticMesh(NormalSoilMesh);
        }
        if (NormalSoilMaterial)
        {
            SoilMesh->SetMaterial(0, NormalSoilMaterial);
        }
        UE_LOG(LogTemp, Verbose, TEXT("흙 타입 설정: Normal"));
    }

    // NavMesh 업데이트
    if (UWorld* World = GetWorld())
    {
        if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World))
        {
            const FVector Origin = GetActorLocation();
            const FVector Extent = FVector(120.f, 120.f, 200.f);
            NavSys->AddDirtyArea(FBox::BuildAABB(Origin, Extent),
                ENavigationDirtyFlag::All);
        }
    }
}