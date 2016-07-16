#include "MazeGeneratorPrivatePCH.h"
#include "Generator.h"


const FRotator AGenerator::rotation = FRotator(0.0f, 0.0f, 0.0f);
const float AGenerator::offset = 400.0f;
const float AGenerator::BlockPadding = 200.0f;

AGenerator::AGenerator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), StartX(-2000), StartY(-2000), StartZ(-200), Rows(33), Cols(33)
{

}

void AGenerator::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	GenMaze();
}


void AGenerator::GenMaze()
{
	auto World = GetWorld();
	if (!World || !BlockBP || !GroundBP || !EndBP || !StartBP) {
		return;
	}
	OddAlgo();
	//DFS();
	//Kruskal();
}

FIntTuple AGenerator::find(FIntTuple node)
{
	if (parent[node.Row][node.Col] == node) {
		return node;
	}
	return parent[node.Row][node.Col] = find(parent[node.Row][node.Col]);
}

void AGenerator::treeUnion(FIntTuple p1, FIntTuple p2)
{
	if (rank[p1.Row][p1.Col] == rank[p2.Row][p2.Col]) {
		parent[p1.Row][p1.Col] = p2;
		rank[p2.Row][p2.Col]++;
	}
	else if (rank[p1.Row][p1.Col] < rank[p2.Row][p2.Col]) {
		parent[p1.Row][p1.Col] = p2;
	}
	else {
		parent[p2.Row][p2.Col] = p1;
	}
}

void AGenerator::Kruskal() {
	auto World = GetWorld();
	for (int row = 0; row < Rows; row++) {
		for (int col = 0; col < Cols; col++) {
			if (row != 0 && col != 0 && row != Rows - 1 && col != Cols - 1 && row % 2 == 1 && col % 2 == 1) {
				Grid[row][col] = World->SpawnActor<AStaticMeshActor>(GroundBP, FVector(StartX + col*offset, StartY + row * offset, StartZ), rotation);
				BoolGrid[row][col] = false;
				rank[row][col] = 0;
				parent[row][col] = FIntTuple(row, col);
			}
			else {
				Grid[row][col] = World->SpawnActor<AStaticMeshActor>(BlockBP, FVector(StartX + col*offset + BlockPadding, StartY + row * offset + BlockPadding, StartZ + BlockPadding - 80), rotation);
				BoolGrid[row][col] = true;
			}
		}
	}
	int toRow, toCol;
	int dx[2] = { 2, 0 };
	int dy[2] = { 0, 2 };
	TArray<FEdgeKruskal> edges;
	for (int row = 1; row < Rows - 1; row += 2) {
		for (int col = 1; col < Cols - 1; col += 2) {
			for (int k = 0; k < 2; k++) {
				toRow = row + dy[k];
				toCol = col + dx[k];
				if (toRow >= 1 && toRow < Rows - 1 && toCol >= 1 && toCol < Cols - 1) {
					edges.Emplace(FEdgeKruskal(FIntTuple(row, col), FIntTuple(toRow, toCol)));
				}
			}
		}
	}
	FEdgeKruskal edge;
	FIntTuple p1, p2, from, to;
	int count = 0, i = 0, N;
	N = Rows / 2;
	if (Rows % 2 == 0) {
		N--;
		Grid[Rows - 2][Cols - 2]->Destroy();
		Grid[Rows - 2][Cols - 2] = GetWorld()->SpawnActor<AStaticMeshActor>(GroundBP, FVector(StartX + (Cols - 2)*offset, StartY + (Rows - 2) * offset, StartZ), rotation);
		Grid[Rows - 2][Cols - 3]->Destroy();
		Grid[Rows - 2][Cols - 3] = GetWorld()->SpawnActor<AStaticMeshActor>(GroundBP, FVector(StartX + (Cols - 3)*offset, StartY + (Rows - 2) * offset, StartZ), rotation);

	}
	if (Cols % 2 == 0) {
		N *= (Cols / 2 - 1);
		Grid[Rows - 2][Cols - 2]->Destroy();
		Grid[Rows - 2][Cols - 2] = GetWorld()->SpawnActor<AStaticMeshActor>(GroundBP, FVector(StartX + (Cols - 2)*offset, StartY + (Rows - 2) * offset, StartZ), rotation);
		Grid[Rows - 3][Cols - 2]->Destroy();
		Grid[Rows - 3][Cols - 2] = GetWorld()->SpawnActor<AStaticMeshActor>(GroundBP, FVector(StartX + (Cols - 2)*offset, StartY + (Rows - 3) * offset, StartZ), rotation);
	}
	else {
		N *= (Cols / 2);
	}
	edges.Sort([this](const FEdgeKruskal &lhs, const FEdgeKruskal &rhs) {
		return FMath::FRand() < 0.5f;
	});
	while (count < N - 1 && edges.Num() != 0) {
		from = edges[i].from;
		to = edges[i].to;
		p1 = find(from);
		p2 = find(to);
		if (p1 != p2) {
			treeUnion(p1, p2);
			int posRow = from.Row, posCol = from.Col;
			if (from.Row + 2 == to.Row) {
				posRow++;
			}
			else {
				posCol++;
			}
			if (BoolGrid[posRow][posCol]) {
				Grid[posRow][posCol]->Destroy();
				Grid[posRow][posCol] = GetWorld()->SpawnActor<AStaticMeshActor>(GroundBP, FVector(StartX + posCol*offset, StartY + posRow * offset, StartZ), rotation);
				BoolGrid[posRow][posCol] = false;
			}
			count++;
		}
		i++;
	}
	World->SpawnActor<AStaticMeshActor>(StartBP, FVector(StartX + 2 * offset - BlockPadding, StartY + 2 * offset - BlockPadding, StartZ), rotation);
	World->SpawnActor<AStaticMeshActor>(EndBP, FVector(StartX + (Cols - 2)*offset + BlockPadding, StartY + (Rows - 2)*offset + BlockPadding, StartZ), rotation);
}

void AGenerator::DFS() {
	auto World = GetWorld();
	for (int row = 0; row < Rows; row++) {
		for (int col = 0; col < Cols; col++) {
			Grid[row][col] = World->SpawnActor<AStaticMeshActor>(BlockBP, FVector(StartX + col*offset + BlockPadding, StartY + row * offset + BlockPadding, StartZ + BlockPadding - 80), rotation);
			BoolGrid[row][col] = true;
		}
	}
	DFSRecursive(1, 1);
	World->SpawnActor<AStaticMeshActor>(StartBP, FVector(StartX + 2 * offset - BlockPadding, StartY + 2 * offset - BlockPadding, StartZ), rotation);
	World->SpawnActor<AStaticMeshActor>(EndBP, FVector(StartX + (Cols - 2)*offset + BlockPadding, StartY + (Rows - 2)*offset + BlockPadding, StartZ), rotation);
}

void AGenerator::DFSRecursive(int row, int col)
{
	Grid[row][col]->Destroy();
	Grid[row][col] = GetWorld()->SpawnActor<AStaticMeshActor>(GroundBP, FVector(StartX + col*offset, StartY + row * offset, StartZ), rotation);
	BoolGrid[row][col] = false;
	int32 NewRow, NewCol;
	bool flag = true;
	int8 dx[4] = { 1, -1, 0, 0 };
	int8 dy[4] = { 0,  0, -1, 1 };
	TArray<int8> indices;
	indices.Add(0); indices.Add(1); indices.Add(2); indices.Add(3);
	indices.Sort([this](const int8 Item1, const int8 Item2) {
		return FMath::FRand() < 0.5f;
	});
	for (int i = 0; i < 4; i++) {
		NewRow = dy[indices[i]] + row;
		NewCol = dx[indices[i]] + col;
		if (NewRow >= 1 && NewRow <= Rows - 1 && NewCol >= 1 && NewCol <= Cols - 1 && BoolGrid[NewRow][NewCol]) {
			flag = true;
			for (int i = 0; i < 4; i++) {
				flag = flag && (BoolGrid[NewRow + dy[i]][NewCol + dx[i]] || (NewRow + dy[i] == row && NewCol + dx[i] == col));
			}
			if (flag && NewRow != Rows - 1 && NewCol != Cols - 1) {
				DFSRecursive(NewRow, NewCol);
			}
		}
	}
}

void AGenerator::OddAlgo() {
	auto World = GetWorld();
	if (Rows % 2 == 0) {
		Rows++;
	}
	if (Cols % 2 == 0) {
		Cols++;
	}
	/* Step 1:
	*  Get the Maze to look like this:
	*			###########
	*			#---------#
	*			#-#-#-#-#-#
	*			#---------#
	*			#-#-#-#-#-#
	*			#---------#
	*			#-#-#-#-#-#
	*			#---------#
	*			###########
	*/
	for (int row = 0; row < Rows; row++) {
		for (int col = 0; col < Cols; col++) {
			if (row == 0 || col == 0 || row == Rows - 1 || col == Cols - 1 || (row % 2 == 0 && col % 2 == 0)) {
				Grid[row][col] = World->SpawnActor<AStaticMeshActor>(BlockBP, FVector(StartX + col*offset + BlockPadding, StartY + row * offset + BlockPadding, StartZ + BlockPadding - 80), rotation);
				BoolGrid[row][col] = true;
			}
			else {
				Grid[row][col] = World->SpawnActor<AStaticMeshActor>(GroundBP, FVector(StartX + col*offset, StartY + row * offset, StartZ), rotation);
				BoolGrid[row][col] = false;
			}
		}
	}

	/* Step 2:
	*  Pass through every already spawned block and generate random
	*  direction and then put a block in this direction
	*/
	int drow, dcol, RandNum;
	for (int col = 2; col < Cols - 1; col += 2) {
		dcol = col;
		drow = 2;
		RandNum = rand() % 4;
		switch (RandNum) {
		case 0: drow++; break;
		case 1: drow--; break;
		case 2: dcol++; break;
		case 3: dcol--; break;
		}
		if (!BoolGrid[drow][dcol]) {
			auto location = Grid[drow][dcol]->GetActorLocation();
			location.X += BlockPadding;
			location.Y += BlockPadding;
			location.Z += BlockPadding - 80;
			Grid[drow][dcol]->Destroy();
			Grid[drow][dcol] = World->SpawnActor<AStaticMeshActor>(BlockBP, location, rotation);
			BoolGrid[drow][dcol] = true;
		}
		else {
			col -= 2;
		}
	}
	for (int row = 4; row < Rows - 1; row += 2) {
		for (int col = 2; col < Cols - 1; col += 2) {
			dcol = col;
			drow = row;
			RandNum = rand() % 3;
			switch (RandNum) {
			case 0: dcol++; break;
			case 1: dcol--; break;
			case 2: drow++; break;
			}
			if (!BoolGrid[drow][dcol]) {
				auto location = Grid[drow][dcol]->GetActorLocation();
				location.X += BlockPadding;
				location.Y += BlockPadding;
				location.Z += BlockPadding - 80;
				Grid[drow][dcol]->Destroy();
				Grid[drow][dcol] = World->SpawnActor<AStaticMeshActor>(BlockBP, location, rotation);
				BoolGrid[drow][dcol] = true;
			}
			else {
				col -= 2;
			}
		}
	}
	World->SpawnActor<AStaticMeshActor>(StartBP, FVector(StartX + 2 * offset - BlockPadding, StartY + 2 * offset - BlockPadding, StartZ), rotation);
	World->SpawnActor<AStaticMeshActor>(EndBP, FVector(StartX + (Cols - 2)*offset + BlockPadding, StartY + (Rows - 2)*offset + BlockPadding, StartZ), rotation);
}