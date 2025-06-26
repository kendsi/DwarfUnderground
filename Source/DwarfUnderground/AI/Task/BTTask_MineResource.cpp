#include "BTTask_MineResource.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "DwarfUnderground/Unit/DwarfUnit.h"
#include "DwarfUnderground/Resource/ResourceBase.h"
#include "DwarfUnderground/Equipment/Weapon.h"
#include "DwarfUnderground/Interface/MinableInterface.h"

UBTTask_MineResource::UBTTask_MineResource()
{
    NodeName = TEXT("Mine Resource");
}

EBTNodeResult::Type UBTTask_MineResource::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8*)
{
    auto* BB = OwnerComp.GetBlackboardComponent();
    auto* Dwarf = Cast<ADwarfUnit>(OwnerComp.GetAIOwner()->GetPawn());
    auto* Target = Cast<AResourceBase>(BB->GetValueAsObject(TargetResourceKey));

    if (!Dwarf || !Target) return EBTNodeResult::Failed;

    // 채굴 도구 확인
    if (auto* Tool = Dwarf->EquippedTool)
    {
        if (IMinableInterface* Minable = Cast<IMinableInterface>(Tool))
        {
            FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
            if (Dwarf->EquippedWeapon)
                Dwarf->EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            Dwarf->EquippedTool->AttachToComponent(Dwarf->GetMesh(), AttachRules, TEXT("ToolSocket"));
            Dwarf->PlayWorkMontage();

            AResourceBase* Resource = Cast<AResourceBase>(BB->GetValueAsObject(TargetResourceKey));
            Dwarf->SetResourceType(Resource->GetResourceType());
            Dwarf->SetResourceAmount(Minable->MineResource(Resource, 3));
            BB->SetValueAsBool(HasResourceKey, true);

            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}