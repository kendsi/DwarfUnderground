#include "roadManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"

// 생성자 이름 수정: roadManager::AroadManager() → AroadManager::AroadManager()
AroadManager::AroadManager()
{
    PrimaryActorTick.bCanEverTick = false;

    // Static Mesh Component 생성
    RoadMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoadMesh"));
    RootComponent = RoadMeshComponent;

    // roadCube 메시 로드
    static ConstructorHelpers::FObjectFinder<UStaticMesh> RoadMeshRef(
        TEXT("/Game/Building/Meshes/roadCube.roadCube")
    );
    if (RoadMeshRef.Succeeded())
    {
        RoadMeshComponent->SetStaticMesh(RoadMeshRef.Object);
    }

    // invisibleCube 머티리얼 로드
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> InvisibleMatRef(
        TEXT("/Game/Building/Materials/invisibleCube.invisibleCube")
    );
    if (InvisibleMatRef.Succeeded())
    {
        InvisibleCubeMaterial = InvisibleMatRef.Object;
    }

    // 콜리전 설정: 건설은 막고, 유닛은 통과
    RoadMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    RoadMeshComponent->SetCollisionObjectType(ECC_WorldStatic);
    RoadMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    RoadMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 건설 막기
    RoadMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);       // 유닛 통과
    RoadMeshComponent->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);    // 차량 통과

    // 디버그 로그
    UE_LOG(LogTemp, Warning, TEXT("roadManager 생성됨: 콜리전 설정 완료"));
}

// 클래스 이름 수정: roadManager → AroadManager
void AroadManager::BeginPlay()
{
    Super::BeginPlay();

    // 게임 시작 시 자동으로 투명하게 만들기
    MakeInvisible();
    UE_LOG(LogTemp, Log, TEXT("RoadManager: 게임 시작 시 투명 머티리얼 적용"));
}

void AroadManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AroadManager::MakeInvisible()
{
    if (RoadMeshComponent && InvisibleCubeMaterial)
    {
        // 현재 머티리얼을 백업 (필요시)
        if (!VisibleMaterial && RoadMeshComponent->GetMaterial(0))
        {
            VisibleMaterial = RoadMeshComponent->GetMaterial(0);
        }

        // 투명 머티리얼 적용
        RoadMeshComponent->SetMaterial(0, InvisibleCubeMaterial);
        UE_LOG(LogTemp, Log, TEXT("RoadManager: 투명 머티리얼 적용됨"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("RoadManager: 투명 머티리얼이 설정되지 않음"));
    }
}

void AroadManager::MakeVisible()
{
    if (RoadMeshComponent && VisibleMaterial)
    {
        // 원래 머티리얼로 복원
        RoadMeshComponent->SetMaterial(0, VisibleMaterial);
        UE_LOG(LogTemp, Log, TEXT("RoadManager: 원래 머티리얼로 복원됨"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("RoadManager: 원래 머티리얼이 없음"));
    }
}