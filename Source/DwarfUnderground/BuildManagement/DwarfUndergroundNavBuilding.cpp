#include "DwarfUndergroundNavBuilding.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"

UDwarfUndergroundNavBuilding::UDwarfUndergroundNavBuilding()
{
    ExtentScale = 1.0f;
}

FBox UDwarfUndergroundNavBuilding::GetNavigationBounds() const
{
    if (AActor* Owner = GetOwner())
    {
        if (UStaticMeshComponent* MeshComp = Owner->FindComponentByClass<UStaticMeshComponent>())
        {
            FBox Box = MeshComp->Bounds.GetBox();
            return FBox::BuildAABB(Box.GetCenter(), Box.GetExtent() * ExtentScale);
        }
    }
    return FBox(ForceInitToZero);
}