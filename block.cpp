#include "block.h"
#include "draw.h"

/* 
A function that selects a random block from the block table with a custom rotation.
Additionally, sets the starting block location.
*/
GameBlock
GetRandomBlock()
{
	GameBlock RandomBlock = {};
	uint16_t BlockIndex = (rand() % BlockTypeCount);
	RandomBlock.Rotation = rand() % BlockRotationCount;
	RandomBlock.Type = (BlockIndex * BlockRotationCount) + RandomBlock.Rotation;
	RandomBlock.Structure = BlockTypes[RandomBlock.Type];
	RandomBlock.Color = BlockColors[BlockIndex];
	RandomBlock.X = 3;
	RandomBlock.Y = 0;
	return RandomBlock;
}

/*
A function to test the state of a row. If a row has all elements filled, return true. 
If the row has any missing elements, return false.
*/
bool
CheckBoardRow(GameBoard* Board, uint16_t Row)
{
	for (uint16_t Column = 0; Column < Board->ColumnCount; Column++)
	{
		if (Board->Grid[Row][Column] == Board->DefaultColor)
		{
			return false;
		}
	}
	return true;
}

/*
A function to sort the board by the content of the rows. Moves all rows with full rows to the top
and shifts each non-full row down.
*/
void
SortBoard(GameBoard * Board)
{
	for (uint16_t i = 0; i < Board->RowCount - 1; i++)
	{
		for (uint16_t j = 0; j < Board->RowCount - i - 1; j++)
		{
			if (CheckBoardRow(Board, j) < CheckBoardRow(Board, j + 1))
			{
				for (uint16_t Column = 0; Column < Board->ColumnCount; Column++)
				{
					uint32_t temp = Board->Grid[j][Column];
					Board->Grid[j][Column] = Board->Grid[j + 1][Column];
					Board->Grid[j + 1][Column] = temp;
				}
			}

		}
	}
}

/* 
A function that counts all the rows with full elements up to the first row that is partially full.
Additionally resets the row color to the selected color. Should only be called after calling SortBoard.
*/
uint16_t
ResetFullRows(GameBoard* Board)
{
	uint16_t Count = 0;
	for (uint16_t Row = 0; Row < Board->RowCount; Row++)
	{
		if (!CheckBoardRow(Board, Row))
			break;

		for (uint16_t Column = 0; Column < Board->ColumnCount; Column++)
		{
			Board->Grid[Row][Column] = Board->DefaultColor;
		}
		Count++;
	}
	return Count;
}

/*
A function to update the score based on the removed rows and the level of the current game.
*/
void
UpdateScore(GameSession* Session, uint16_t RowsRemoved)
{
	switch (RowsRemoved)
	{
	case 1:
		Session->Score += (40 * (Session->Level + 1));
		break;
	case 2:
		Session->Score += (100 * (Session->Level + 1));
		break;
	case 3:
		Session->Score += (300 * (Session->Level + 1));
		break;
	case 4:
		Session->Score += (1200 * (Session->Level + 1));
		break;
	}
}

/* 
A function that checks if the block supplied can fit at the desired position
*/
bool 
CanMoveBlock(GameBoard * Board, GameBlock CopyBlock)
{
	uint16_t Type = CopyBlock.Structure;
	uint16_t HighBit = 1 << (16 - 1);
	for (int Row = 0; Row < 4; Row++)
	{
		for (int Column = 0; Column < 4; Column++)
		{
			if (Type & HighBit)
			{
				int32_t RealColumn = CopyBlock.X + Column;
				int32_t RealRow = CopyBlock.Y + Row;

				if (RealColumn > Board->ColumnCount - 1 || RealColumn < 0 || RealRow > Board->RowCount - 1 || RealRow < 0)
				{
					return false;
				}

				uint32_t CheckColor = CopyBlock.Color;
				uint32_t BoardColor = Board->Grid[RealRow][RealColumn];

				if (BoardColor != Board->DefaultColor)
				{
					return false;
				}
			}
			HighBit >>= 1;
		}
	}
	return true;
}

/* 
A function to press a block into the board.
*/
void 
PressBlock(GameBoard * Board, GameBlock CopyBlock)
{
	uint16_t Type = CopyBlock.Structure;
	uint16_t HighBit = 1 << (16 - 1);
	for (int Row = 0; Row < 4; Row++)
	{
		for (int Column = 0; Column < 4; Column++)
		{
			if (Type & HighBit)
			{
				int32_t RealColumn = CopyBlock.X + Column;
				int32_t RealRow = CopyBlock.Y + Row;
				Board->Grid[RealRow][RealColumn] = CopyBlock.Color;
			}
			HighBit >>= 1;
		}
	}
}

/* 
A function that returns a new block with the rotated contents of the provided block.
*/
GameBlock 
RotateBlock(GameBlock CopyBlock)
{
	uint16_t Rotation = CopyBlock.Rotation;
	CopyBlock.Rotation = (Rotation + 1) % BlockRotationCount;
	CopyBlock.Type = (CopyBlock.Type - Rotation) + CopyBlock.Rotation;
	CopyBlock.Structure = BlockTypes[CopyBlock.Type];
	return CopyBlock;
}

/* 
A function that returns a new block with the moved contents of the provided block.
*/
GameBlock 
MoveBlock(GameBlock CopyBlock, int16_t X, int16_t Y)
{
	CopyBlock.X += X;
	CopyBlock.Y += Y;
	return CopyBlock;
}

/* 
A function that returns the last available block that can be pressed into a board. 
If the inital block cannot be moved, that block is returned. 
*/
GameBlock
DropBlock(GameSession* Session, GameBlock CopyBlock)
{
	GameBlock LastBlock = CopyBlock;
	while (CanMoveBlock(&Session->Board, CopyBlock))
	{
		LastBlock = CopyBlock;
		CopyBlock = MoveBlock(CopyBlock, 0, 1);
	}
	return LastBlock;
}

/*
A function that resets the board to the default color.
*/
void
ResetBoard(GameBoard* Board)
{
	for (uint32_t Row = 0; Row < Board->RowCount; Row++)
	{
		for (uint32_t Column = 0; Column < Board->ColumnCount; Column++)
		{
			Board->Grid[Row][Column] = Board->DefaultColor;
		}
	}
}

void 
ProcessKeyAction(GameSession * Session, GameKey Key)
{
	GameBlock CopyBlock;
	switch (Key)
	{
		case Left:
			CopyBlock = MoveBlock(Session->CurrentBlock, -1, 0);
			break;
		case Right:
			CopyBlock = MoveBlock(Session->CurrentBlock, 1, 0);
			break;
		case Down:
			CopyBlock = MoveBlock(Session->CurrentBlock, 0, 1);
			break;
		case Rotate:
			CopyBlock = RotateBlock(Session->CurrentBlock);
			break;
		case Drop:
			CopyBlock = DropBlock(Session, Session->CurrentBlock);
			break;
		default: // Not a servicable game action.
			return;
	}

	bool CanBlockMove = CanMoveBlock(&Session->Board, CopyBlock);
	if ((!CanBlockMove && Key == Down) || Key == Drop)
	{
		if (Key == Down)
		{
			CopyBlock = Session->CurrentBlock;
		}

		PressBlock(&Session->Board, CopyBlock);

		SortBoard(&Session->Board);

		uint32_t LinesCleared = ResetFullRows(&Session->Board);
		UpdateScore(Session, LinesCleared);
		Session->LinesCleared += LinesCleared;

		// Update Level Here?

		if (!CanMoveBlock(&Session->Board, Session->NextBlock))
		{
			Session->State = Finished;
			return;
		}
		
		Session->CurrentBlock = Session->NextBlock;
		Session->NextBlock = GetRandomBlock();
	}
	else if (CanBlockMove)
	{
		Session->CurrentBlock = CopyBlock;
	}
}

/* 
A function to initalize the game state. If the memory allocation for the board states fail, then we return false.
*/
bool 
GameInitialize(GameGraphics * Graphics, GameSession* Session)
{
	// Seed the random block generator
	srand(time(NULL));

	// Wipe the video buffer
	DrawCoordinateBox(Graphics, 0xFF121212, 0, 0, Graphics->Width, Graphics->Height);

	// Create and reset the board
	GameBoard Board;
	Board.ColumnCount = TileColumnCount;
	Board.RowCount = TileRowCount;
	Board.RenderX = BoardStartPositionX;
	Board.RenderY = BoardStartPositionY;
	Board.RenderSize = TileRenderSize;
	Board.RenderRowOffset = 0;
	Board.RenderColumnOffset = 0;
	Board.DefaultColor = TileBackgroundColor;

	Board.Grid = (uint32_t * *)malloc(sizeof(uint32_t*) * TileRowCount); // Add error checking
	if (Board.Grid == NULL)
		return false;
	for (int i = 0; i < TileRowCount; i++)
	{
		Board.Grid[i] = (uint32_t*)malloc(sizeof(uint32_t) * TileColumnCount);
		if (Board.Grid == NULL)
			return false;
	}

	ResetBoard(&Board);

	// Create and reset the preview board
	GameBoard PreviewBoard;
	PreviewBoard.ColumnCount = PreviewColumnCount;
	PreviewBoard.RowCount = PreviewRowCount;
	PreviewBoard.RenderX = PreviewStartPositionX;
	PreviewBoard.RenderY = PreviewStartPositionY;
	PreviewBoard.RenderSize = TileRenderSize;
	PreviewBoard.RenderRowOffset = 1;
	PreviewBoard.RenderColumnOffset= 1;
	PreviewBoard.DefaultColor = TileBackgroundColor;
	
	PreviewBoard.Grid = (uint32_t * *)malloc(sizeof(uint32_t*) * PreviewRowCount); // Add error checking
	if (PreviewBoard.Grid == NULL)
		return false;
	for (int i = 0; i < PreviewRowCount; i++)
	{
		PreviewBoard.Grid[i] = (uint32_t*)malloc(sizeof(uint32_t) * PreviewColumnCount);
		if (PreviewBoard.Grid == NULL)
			return false;
	}

	ResetBoard(&PreviewBoard);

	Session->CurrentBlock = GetRandomBlock();
	Session->NextBlock = GetRandomBlock();

	Session->Board = Board;
	Session->PreviewBoard = PreviewBoard;

	Session->BlockDropSeconds = AutomaticBlockDropTimeSeconds;
	Session->LinesCleared = 0;

	// Draw the board.
	// Draw the board outline.
	DrawCoordinateBox(Graphics, 0xFF9E351A, BoardStartPositionX - 1, BoardStartPositionY - 1, BoardStartPositionX + (TileRenderSize * TileColumnCount), BoardStartPositionY + (TileRenderSize * TileRowCount));
	// Draw the tile border
	DrawCoordinateBox(Graphics, 0xFF212121, BoardStartPositionX, BoardStartPositionY, BoardStartPositionX + (TileRenderSize * TileColumnCount) - 1, BoardStartPositionY + (TileRenderSize * TileRowCount) - 1);
	DrawBoard(Graphics, &Board, false);


	DrawCoordinateBox(Graphics, 0xFF9E351A, PreviewStartPositionX - 1, PreviewStartPositionY - 1, PreviewStartPositionX + (TileRenderSize * PreviewColumnCount), PreviewStartPositionY + (TileRenderSize * PreviewRowCount));
	// Draw the preview tile border
	DrawCoordinateBox(Graphics, 0xFF212121, PreviewStartPositionX, PreviewStartPositionY, PreviewStartPositionX + (TileRenderSize * PreviewColumnCount) - 1, PreviewStartPositionY + (TileRenderSize * PreviewRowCount) - 1);

	DrawBoard(Graphics, &PreviewBoard, true);

	Session->State = Initalized;
	return true;
}

void 
GameStart(GameGraphics* Graphics, GameSession * Session)
{
	if (Session->State == Finished)
	{
		// call shutdown or free memory
		GameInitialize(Graphics, Session);
	}
	if (Session->State == Initalized)
	{
		Session->State = Playing;
		Session->Time = clock();
	}
}

void 
GameUpdate(GameGraphics * Graphics, GameSession* GameSession, GameKey Key)
{
	if (GameSession->State == Playing)
	{
		if ((clock() - GameSession->Time) / (float)CLOCKS_PER_SEC > GameSession->BlockDropSeconds)
		{
			ProcessKeyAction(GameSession, Down);
			GameSession->Time = clock();
		}

		ProcessKeyAction(GameSession, Key);
	}

	DrawBoard(Graphics, &GameSession->Board, false);
	DrawBlock(Graphics, GameSession->CurrentBlock, &GameSession->Board, false); 

	DrawBoard(Graphics, &GameSession->PreviewBoard, true);
	DrawBlock(Graphics, GameSession->NextBlock, &GameSession->PreviewBoard, true);

}
