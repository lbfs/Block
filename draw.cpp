#include "draw.h"

void
DrawCoordinateBox(GameGraphics* Graphics, uint32_t HexColor, int StartX, int StartY, int EndX, int EndY)
{
	uint32_t* Pixel = (uint32_t*)Graphics->Buffer;
	Pixel += (StartX + Graphics->Width * StartY);
	for (int Y = EndY - StartY; Y > 0; --Y)
	{
		for (int X = EndX - StartX; X > 0; --X)
		{
			*(Pixel + X) = HexColor;
		}
		Pixel += Graphics->Width;
	}
}

void
DrawBlock(GameGraphics* Graphics, GameBlock Block, GameBoard* Board, bool UseBlockCoordinates)
{
	uint16_t Type = Block.Structure;
	uint16_t HighBit = 1 << (16 - 1);
	for (uint32_t Row = 0; Row < 4; Row++)
	{
		for (uint32_t Column = 0; Column < 4; Column++)
		{
			if (Type & HighBit)
			{
				int32_t X = Block.X;
				int32_t Y = Block.Y;
				if (UseBlockCoordinates)
				{
					X = 0;
					Y = 0;
				}
				uint32_t StartX = ((Column + Board->RenderColumnOffset) * Board->RenderSize) + (X * Board->RenderSize) + Board->RenderX;
				uint32_t StartY = ((Row + Board->RenderRowOffset) * Board->RenderSize) + (Y * Board->RenderSize) + Board->RenderY;
				int32_t EndX = StartX + Board->RenderSize - 1;
				int32_t EndY = StartY + Board->RenderSize - 1;
				DrawCoordinateBox(Graphics, Block.Color, StartX, StartY, EndX, EndY);
			}
			HighBit >>= 1;
		}
	}
}

void
DrawBoard(GameGraphics* Graphics, GameBoard * Board, bool UseDefaultColor)
{
	for (uint16_t Row = 0; Row < Board->RowCount; Row++)
	{
		for (uint16_t Column = 0; Column < Board->ColumnCount; Column++)
		{
			uint16_t StartX = Column * Board->RenderSize + Board->RenderX;
			uint16_t StartY = Row * Board->RenderSize + Board->RenderY;
			uint16_t EndX = StartX + Board->RenderSize - 1;
			uint16_t EndY = StartY + Board->RenderSize - 1;

			if (UseDefaultColor)
			{
				DrawCoordinateBox(Graphics, UseDefaultColor, StartX, StartY, EndX, EndY);
			}
			else
			{
				DrawCoordinateBox(Graphics, Board->Grid[Row][Column], StartX, StartY, EndX, EndY);
			}

		}
	}
}

