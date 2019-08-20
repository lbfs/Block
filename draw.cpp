#include "block.h"

void
DrawCoordinateBox(GameGraphics* Graphics, uint32_t HexColor, uint32_t StartX, uint32_t StartY, uint32_t EndX, uint32_t EndY)
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

/* This is entire function is stupid and needs to be reworked. */
BitmapCharacters*
LoadFont(const char* Fontconfig, const char* Fontname)
{
	FILE* FileHandle = NULL;
	fopen_s(&FileHandle, Fontconfig, "rt");

	if (FileHandle == NULL)
		return NULL;

	uint32_t cat = 0;
	unsigned char c = 0;
	bool mark = false;
	uint32_t ElementIndex = 0;

	BitmapCharacters* Characters = (BitmapCharacters*)malloc(sizeof(BitmapCharacters));
	if (Characters == NULL)
		return NULL;

	Characters->Length = -1;

	uint32_t CharacterIndex = 0;

	while ((c = fgetc(FileHandle)) != EOF)
	{
		if (c >= '0' && c <= '9')
		{
			uint32_t pow = 10;
			while ((unsigned)(c - '0') >= pow)
				pow *= 10;
			cat = cat * pow + (unsigned)(c - '0');

			mark = true;
		}
		else
		{
			if (mark)
			{
				if (Characters->Length == -1)
				{
					Characters->Length = cat;
					Characters->Elements = (BitmapCharacterInfo*)malloc(sizeof(BitmapCharacterInfo) * Characters->Length);
				}
				else
				{
					if (ElementIndex > 7)
					{
						ElementIndex = 0;
						CharacterIndex++;
						if (CharacterIndex >= (Characters->Length - 1))
						{
							break;
						}
					}
					if (CharacterIndex != -1)
					{
						Characters->Elements[CharacterIndex].Elements[ElementIndex] = cat;
						ElementIndex++;
					}
				}
			}
			mark = false;
			cat = 0;
		}
	}

	GameGraphics Graphics = {};
	//@HACK
	Graphics.Width = 256;
	Graphics.Height = 256;
	Graphics.Buffer = malloc(Graphics.Width * Graphics.Height * sizeof(uint32_t));
	if (Graphics.Buffer == NULL)
	{
		free(Characters->Elements);
		free(Characters);
		fclose(FileHandle);
		return NULL;
	}

	FILE* BitmapFileHandle = NULL;
	fopen_s(&BitmapFileHandle, Fontname, "rb");
	if (BitmapFileHandle == NULL)
	{
		free(Characters->Elements);
		free(Characters);
		fclose(FileHandle);
		return NULL;
	}

	uint32_t Pixel = 0x00000000;
	uint32_t* PixelIterator = (uint32_t *)Graphics.Buffer;
	while (fread(&Pixel, 1, 3, BitmapFileHandle) == 3) {
		*PixelIterator = Pixel;
		uint32_t garbage = *PixelIterator;
		PixelIterator++;
	}

	Characters->Graphics = Graphics;
	fclose(BitmapFileHandle);
	fclose(FileHandle);
	return Characters;
}

BitmapCharacterInfo* LookupGlyph(BitmapCharacters* Characters, const char Letter)
{
	for (uint32_t index = 0; index < Characters->Length; index++)
	{
		if (Characters->Elements[index].Id == Letter)
		{
			return &Characters->Elements[index];
		}
	}
	return NULL;
}

void DrawWord(GameGraphics* Graphics, BitmapCharacters* Characters, const char * word, uint32_t StartX, uint32_t StartY)
{

	uint32_t* Pixel = (uint32_t*)Graphics->Buffer;
	Pixel += (StartX + Graphics->Width * StartY);
	for (uint32_t index = 0; index < strlen(word); index++)
	{
		BitmapCharacterInfo* Glyph = LookupGlyph(Characters, word[index]);
		if (Glyph == NULL)
		{
			continue;
		}

		uint32_t* StartingBitmapPixel = ((uint32_t*)Characters->Graphics.Buffer) + Characters->Graphics.Width * Glyph->Y + Glyph->X;
		uint32_t* StartPixel = (uint32_t*)Pixel;
		for (int Y = 0; Y < Glyph->Height; Y++)
		{
			for (int X = 0; X < Glyph->Width; X++)
			{
				uint32_t BitmapPixel = *(StartingBitmapPixel + X + (Y * Characters->Graphics.Width));
				if (BitmapPixel != 0x00000000)
				{
					*StartPixel = BitmapPixel;
				}
				StartPixel++;
			}
			StartPixel += Graphics->Width - Glyph->Width;
		}
		Pixel += Glyph->XAdvance;
	}


}