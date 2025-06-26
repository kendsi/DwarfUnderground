#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "roadManager.generated.h"

UCLASS()
class DWARFUNDERGROUND_API AroadManager : public AActor
{
    GENERATED_BODY()

public:
    AroadManager();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* RoadMeshComponent;

    // 투명 머티리얼 레퍼런스
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* InvisibleCubeMaterial;

    // 게임 시작 전 보이는 머티리얼 (옵션)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* VisibleMaterial;

public:
    virtual void Tick(float DeltaTime) override;

    // 수동으로 투명하게 만드는 함수
    UFUNCTION(BlueprintCallable, Category = "Visibility")
    void MakeInvisible();

    // 수동으로 보이게 만드는 함수
    UFUNCTION(BlueprintCallable, Category = "Visibility")
    void MakeVisible();
};