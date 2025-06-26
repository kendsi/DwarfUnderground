#pragma once
#include "DwarfUnit.h"
#include "DwarfManagerSubsystem.generated.h"


UENUM()
enum class EDwarfSortOrderBy : uint8
{
	None = 0,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDwarfRegisteredDelegate, ADwarfUnit*, Dwarf);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDwarfUnregisteredDelegate, ADwarfUnit*, Dwarf);

UCLASS(Blueprintable, BlueprintType)
class DWARFUNDERGROUND_API UDwarfManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "DwarfEvent")
	FOnDwarfRegisteredDelegate OnDwarfRegistered;
	UPROPERTY(BlueprintAssignable, Category = "DwarfEvent")
	FOnDwarfUnregisteredDelegate OnDwarfUnregistered;

	//부대 최소 인원
	//TODO: 부대 관리쪽으로 넘어갈 필요 있음
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SquadCreation")
	int32 MinPersonnel;

	//부대 최대 인원
	//TODO: 부대 관리쪽으로 넘어갈 필요 있음
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SquadCreation")
	int32 MaxPersonnel;
public:
	UDwarfManagerSubsystem();
	//초기화
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//원하는 대상 가져오기
	UFUNCTION(BlueprintCallable, Category="Dwarf Management")
	TArray<ADwarfUnit*> GetDwarves(EUnitState State, EDwarfSortOrderBy OrderBy) const;
	
	//드워프 생성(
	UFUNCTION(BlueprintCallable, Category = "Dwarf Management")
	void RegisterDwarf(ADwarfUnit* TargetDwarf);

	UFUNCTION(BlueprintCallable, Category = "Dwarf Management")
	void UnregisterDwarf(ADwarfUnit* TargetDwarf);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dwarf Management")
	bool CheckIsRegistered(ADwarfUnit* TargetDwarf);
	

	//월드 내 모든 드워프 가져옴
	UFUNCTION(BlueprintCallable, Category = "Dwarf Management")
	void FindAllDwarvesInLevel();

	UFUNCTION(BlueprintCallable, Category = "Dwarf Management")
	TArray<USquadComponent*> GetAllSquads();

	
	UFUNCTION(BlueprintCallable, Category = "Dwarf Management")
	void RegisterSquad(USquadComponent* Squad);

	
	
	
private:
	UPROPERTY()
	TArray<ADwarfUnit*> Dwarves;
	
	UPROPERTY()
	TSubclassOf<ADwarfUnit> DwarfClass;

	UPROPERTY()
	TArray<USquadComponent*> Squads;
};
