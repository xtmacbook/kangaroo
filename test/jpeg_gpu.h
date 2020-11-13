

#include "smartPointer.h"

class Texture;
class FrameBufferObject;

#define MAX_CHANELS 3

class JPEG_GPU
{
public:

	base::SmartPointer<Texture> pTextureDCT;
	base::SmartPointer<Texture> pTextureQ;

	// Row textures
	base::SmartPointer<Texture>                pTexture1Row;
	base::SmartPointer<Texture>                pTexture2Row;

	// Column textures
	base::SmartPointer<Texture>                pTexture1Col;
	base::SmartPointer<Texture>                pTexture2Col;

	base::SmartPointer<FrameBufferObject>	row1FrameBuffer_;
	base::SmartPointer<FrameBufferObject>	row2FrameBuffer_;
	base::SmartPointer<FrameBufferObject>	col1FrameBuffer_;
	base::SmartPointer<FrameBufferObject>	col2FrameBuffer_;

	// Final texture
	base::SmartPointer<Texture>                pTextureTarget;


	JPEG_GPU();


	int intitialize(int width, int height, char *pQuantTable);
	void release();
};


class Jpeg_Data
{
protected:
	int     imageWidth;
	int     imageHeight;
	int     componentsNum;

	char    quantTables[MAX_CHANELS][64];

	int     MCU_per_row;
	int     MCU_blocks_num;

	char    *pFileData;

	int      format;
	int		 type;
public:
	bool		loadFile(const char*file);

	JPEG_GPU	textureData[MAX_CHANELS];
};
