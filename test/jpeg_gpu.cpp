
#include "jpeg_gpu.h"
#include "texture.h"
#include "framebuffers.h"
#include "log.h"
#include "gls.h"
#include "jpgd.h"

JPEG_GPU::JPEG_GPU()
{

}

int JPEG_GPU::intitialize(int width, int height, char *pQuantTable)
{
	pTextureDCT = new Texture;
	pTextureDCT->target_ = GL_TEXTURE_2D;
	pTextureDCT->internalformat_ = GL_R8_SNORM;
	pTextureDCT->width_ = width;
	pTextureDCT->height_ = height;
	pTextureDCT->createObj();
	pTextureDCT->bind();
	pTextureDCT->contextNULL();
	pTextureDCT->unBind();
	CHECK_GL_ERROR;

	pTextureQ = new Texture;
	pTextureQ->target_ = GL_TEXTURE_2D;
	pTextureQ->internalformat_ = GL_R8;
	pTextureQ->width_ = 8;
	pTextureQ->height_ = 8;
	pTextureQ->createObj();
	pTextureQ->bind();
	pTextureQ->context(pQuantTable);
	pTextureQ->unBind();
	CHECK_GL_ERROR;

	pTexture1Row = new Texture;
	pTexture1Row->target_ = GL_TEXTURE_2D;
	pTexture1Row->internalformat_ = GL_RGBA16F;
	pTexture1Row->width_ = width / 8;
	pTexture1Row->height_ = height;
	pTexture1Row->createObj();
	pTexture1Row->bind();
	pTexture1Row->contextNULL();
	pTexture1Row->unBind();
	row1FrameBuffer_ = new FrameBufferObject(GL_FRAMEBUFFER);
	row1FrameBuffer_->bindObj(true, false);
	row1FrameBuffer_->colorTextureAttachments(pTexture1Row);
	row1FrameBuffer_->bindObj(false, false);
	CHECK_GL_ERROR;


	pTexture2Row = new Texture;
	pTexture2Row->target_ = GL_TEXTURE_2D;
	pTexture2Row->internalformat_ = GL_RGBA16F;
	pTexture2Row->width_ = width / 8;
	pTexture2Row->height_ = height;
	pTexture2Row->createObj();
	pTexture2Row->bind();
	pTexture2Row->contextNULL();
	pTexture2Row->unBind();
	CHECK_GL_ERROR;
	row2FrameBuffer_ = new FrameBufferObject(GL_FRAMEBUFFER);
	row2FrameBuffer_->bindObj(true, false);
	row2FrameBuffer_->colorTextureAttachments(pTexture2Row);
	row2FrameBuffer_->bindObj(false, false);



	pTexture1Col = new Texture;
	pTexture1Col->target_ = GL_TEXTURE_2D;
	pTexture1Col->internalformat_ = GL_RGBA16F;
	pTexture1Col->width_ = width;
	pTexture1Col->height_ = height / 8;
	pTexture1Col->createObj();
	pTexture1Col->bind();
	pTexture1Col->contextNULL();
	pTexture1Col->unBind();
	col1FrameBuffer_ = new FrameBufferObject(GL_FRAMEBUFFER);
	col1FrameBuffer_->bindObj(true, false);
	col1FrameBuffer_->colorTextureAttachments(pTexture1Col);
	col1FrameBuffer_->bindObj(false, false);
	CHECK_GL_ERROR;


	pTexture2Col = new Texture;
	pTexture2Col->target_ = GL_TEXTURE_2D;
	pTexture2Col->internalformat_ = GL_RGBA16F;
	pTexture2Col->width_ = width;
	pTexture2Col->height_ = height / 8;
	pTexture2Col->createObj();
	pTexture2Col->bind();
	pTexture2Col->contextNULL();
	pTexture2Col->unBind();
	CHECK_GL_ERROR;
	col2FrameBuffer_ = new FrameBufferObject(GL_FRAMEBUFFER);
	col2FrameBuffer_->bindObj(true, false);
	col2FrameBuffer_->colorTextureAttachments(pTexture2Col);
	col2FrameBuffer_->bindObj(false, false);


	pTextureTarget = new Texture;
	pTextureTarget->target_ = GL_TEXTURE_2D;
	pTextureTarget->internalformat_ = GL_R16F;
	pTextureTarget->width_ = width;
	pTextureTarget->height_ = height;
	pTextureTarget->createObj();
	pTextureTarget->bind();
	pTextureTarget->contextNULL();
	pTextureTarget->unBind();
	CHECK_GL_ERROR;

	return  1;
}

void JPEG_GPU::release()
{

}

bool Jpeg_Data::loadFile(const char*file)
{
	jpgd::jpeg_decoder_file_stream* pinput_stream = new jpgd::jpeg_decoder_file_stream();

	if (!pinput_stream->open(file))
	{
		delete pinput_stream;
		return false;
	}

	jpgd::jpeg_decoder* Pd = new jpgd::jpeg_decoder(pinput_stream);
	Pd->begin_decoding();

	std::string temporaryName = std::string(file) + "_UNCOMPRESSED";
	HANDLE fileHandle = CreateFile((temporaryName).c_str(),
		GENERIC_ALL,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_DELETE_ON_CLOSE,
		NULL);

	if (!fileHandle)
	{
		return S_FALSE;
	}

	Pd->dump_DCT_data(fileHandle);

	HANDLE mappingHandle = CreateFileMapping(
		fileHandle,
		NULL,
		PAGE_READONLY,
		0,
		0,
		NULL);

	char * pFileData = (char*)MapViewOfFile(
		mappingHandle,
		FILE_MAP_READ,
		0,
		0,
		0);


	imageWidth = Pd->get_width();
	imageHeight = Pd->get_height();

	componentsNum = Pd->get_num_components();

	static float scaleFactor[8] = { 1.0f, 1.387039845f, 1.306562965f, 1.175875602f,
		1.0f, 0.785694958f, 0.541196100f, 0.275899379f };

	for (int i = 0; i < componentsNum; i++)
	{
		jpgd::jpgd_quant_t * pTable = Pd->getQuantizationTable(i);

		for (int row = 0; row < 8; row++)
		{
			for (int col = 0; col < 8; col++)
			{
				quantTables[i][row * 8 + col] = char(pTable[row * 8 + col] * scaleFactor[row] * scaleFactor[col]);
			}
		}
		MCU_per_row = Pd->get_mcus_per_row();

	}
	MCU_blocks_num = Pd->get_blocks_per_mcu();

	pinput_stream->close();

	delete Pd;
	delete pinput_stream;

	return true;
}
