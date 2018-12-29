// Copyright 2018 Schemepunk Studios

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "Rivers/IslandRivers.h"

#include "NamedIslandRivers.generated.h"

UCLASS(BlueprintType)
class ISLANDGENERATOR_API UNamedRiver : public URiver
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RiverName;
};

/**
 * 
 */
UCLASS()
class ISLANDGENERATOR_API UNamedIslandRivers : public UIslandRivers
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* NameDataTable;
	
protected:
	virtual void AssignSideFlow_Implementation(TArray<int32>& s_flow, TArray<URiver*>& Rivers, UTriangleDualMesh* Mesh, const TArray<FSideIndex>& t_downslope_s, const TArray<FTriangleIndex>& river_t, FRandomStream& RiverRNG) const override;
};
