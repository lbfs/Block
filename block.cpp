#include "block.h"

void
DrawCoordinateBox(GraphicsInfo* GameGraphicsInfo, uint32_t HexColor, int StartX, int StartY, int EndX, int EndY)
{
	uint32_t* Pixel = (uint32_t*)GameGraphicsInfo->BitmapMemory;
	Pixel += (StartX + GameGraphicsInfo->Width * StartY);
	for (int Y = 0; Y <= EndY - StartY; ++Y)
	{
		for (int X = 0; X <= EndX - StartX; ++X)
		{
			*(Pixel + X) = HexColor;
		}
		Pixel += GameGraphicsInfo->Width;
	}
}

void
DrawGrid(GraphicsInfo * GameGraphicsInfo, uint32_t GridColor, int StartX, int StartY)
{
	uint32_t GridWidth = (StartX + (TileCountX * TileRenderSize)) - StartX;
	uint32_t GridHeight = (StartY + (TileCountY * TileRenderSize)) - StartY;

	uint32_t* Row = (uint32_t*)GameGraphicsInfo->BitmapMemory;
	Row += (StartX + GameGraphicsInfo->Width * StartY);

	for (uint32_t RowIndex = 0; RowIndex < GridHeight; RowIndex++)
	{
		uint32_t* Pixel = Row;
		for (uint32_t ColumnIndex = 0; ColumnIndex < GridWidth; ColumnIndex++)
		{
			if ((RowIndex % TileRenderSize == 0 && RowIndex != 0) || (ColumnIndex % TileRenderSize == 0 && ColumnIndex != 0))
			{
				*Pixel = GridColor;
			}
			Pixel++;
		}
		Row += GameGraphicsInfo->Width;
	}
}

void
DrawTetrisGrid(GraphicsInfo * GameGraphicsInfo, uint32_t GridColor, int StartX, int StartY)
{
	for (int GridXCount = 0; GridXCount < TileCountX; GridXCount++)
	{
		for (int GridYCount = 0; GridYCount < TileCountY; GridYCount++)
		{
			uint32_t ElementStartX = StartX + (GridXCount * TileRenderSize);
			uint32_t ElementStartY = StartY + (GridYCount * TileRenderSize);
			uint32_t ElementEndX = ElementStartX + TileRenderSize - 1;
			uint32_t ElementEndY = ElementStartY + TileRenderSize - 1;
			DrawCoordinateBox(GameGraphicsInfo, 0xFF000000, ElementStartX, ElementStartY, ElementEndX, ElementEndY);
		}
	}
	DrawGrid(GameGraphicsInfo, GridColor, StartX, StartY);
}


void
GameInitialize(GraphicsInfo * GameGraphicsInfo)
{
	DrawCoordinateBox(GameGraphicsInfo, 0xFF9E351A, 215, 30, 426, 451); // Replace with box outline
	DrawCoordinateBox(GameGraphicsInfo, 0xFF000000, 216, 31, 425, 450);
	DrawTetrisGrid(GameGraphicsInfo, 0xFF212121, 216, 31);
}

void
GameUpdate(GraphicsInfo * GameGraphicsInfo)
{

}