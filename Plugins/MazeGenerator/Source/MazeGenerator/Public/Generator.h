#pragma once

#include "GameFramework/Actor.h"
#include "Generator.generated.h"


USTRUCT()
struct FIntTuple
{
	GENERATED_BODY()
	int Row;
	int Col;
	FIntTuple(int Row = 0, int Col = 0) : Row(Row), Col(Col) {}
	FIntTuple(const FIntTuple &other) : Row(other.Row), Col(other.Col) {}
	bool operator==(const FIntTuple &other) const {
		return Row == other.Row && Col == other.Col;
	}
	bool operator!=(const FIntTuple &other) const {
		return !(*this == other);
	}
	FIntTuple& operator=(const FIntTuple &other) {
		Row = other.Row;
		Col = other.Col;
		return *this;
	}
};

USTRUCT()
struct FEdgeKruskal
{
	GENERATED_BODY()
		FIntTuple from, to;
	FEdgeKruskal() {}
	FEdgeKruskal(FIntTuple from, FIntTuple to) : from(from), to(to) {}
	bool operator==(const FEdgeKruskal &other) const {
		return from == other.from && to == other.to;
	}
};



UCLASS()
class  AGenerator : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MazeProperties)
		int StartX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MazeProperties)
		int StartY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MazeProperties)
		int StartZ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MazeProperties)
		int Rows;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MazeProperties)
		int Cols;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MazeTiles)
		UClass* GroundBP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MazeTiles)
		UClass* BlockBP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MazeTiles)
		UClass* StartBP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MazeTiles)
		UClass* EndBP;
	UFUNCTION(BlueprintCallable, Category = MazeGen)
		void GenMaze();
	UFUNCTION(BlueprintCallable, Category = MazeGen)
		void DFS();
	UFUNCTION(BlueprintCallable, Category = MazeGen)
		void OddAlgo();
	UFUNCTION(BlueprintCallable, Category = MazeGen)
		void Kruskal();
	static const int MaxSize = 101;
	static const FRotator rotation;
	static const float offset;
	static const float BlockPadding;
	AStaticMeshActor *Grid[MaxSize][MaxSize];
	bool BoolGrid[MaxSize][MaxSize];
	FIntTuple parent[MaxSize][MaxSize];
	int rank[MaxSize][MaxSize];
	void treeUnion(FIntTuple p1, FIntTuple p2);
	FIntTuple find(FIntTuple node);
	void DFSRecursive(int row, int col);
public:
	virtual void BeginPlay();
};
