// Copyright 2018 Schemepunk Studios

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "Rivers/IslandRivers.h"

#include "RiverSplineComponent.h"

#include "NamedIslandRivers.generated.h"

UCLASS(BlueprintType)
class ISLANDGENERATOR_API UNamedRiver : public URiver
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RiverName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> RiverFlow;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RiverErosionFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMaterialInterface* RiverMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<FName, UTexture*> RiverTextures;

	// How much width gets added for each additional unit of river "flow."
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float FlowScale;
	// How small the smallest river is.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MinRiverWidth;
	// An offset that will be applied to every river spline point
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector RiverOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMesh* RiverMesh;

public:
	UNamedIslandRivers();
	
protected:
	virtual void AssignSideFlow_Implementation(TArray<int32>& s_flow, TArray<URiver*>& Rivers, UTriangleDualMesh* Mesh, const TArray<FSideIndex>& t_downslope_s, const TArray<FTriangleIndex>& river_t, FRandomStream& RiverRNG) const override;

public:
	URiverSplineComponent* CreateRiverSpine(AActor* Parent, UNamedRiver* River, const TArray<FVector>& SplinePositions) const;
	TArray<FVector> ProcessRiver(TArray<float>& r_elevation, UTriangleDualMesh* Mesh, UNamedRiver* River, const TArray<float>& t_elevation, const TArray<int32>& s_flow, int32 MaxFlow, float ZScale) const;
};
