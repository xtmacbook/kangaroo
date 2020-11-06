
#ifndef  _FRAME_BUFFER_
#define  _FRAME_BUFFER_

#include "decl.h"

typedef enum {
	FBO_DepthBufferType_NONE = 0,
	FBO_DepthBufferType_RENDERTARGET = 1,						//24bits depth buffer
	FBO_DepthBufferType_TEXTURE = 2,							//24bits depth buffer
	FBO_DepthBufferType_TEXTURE_PACKED_DEPTH24_STENCIL8 = 3,	//24bits depth buffer + packed 8bits stencil (32bits per pixel)
	FBO_DepthBufferType_TEXTURE_DEPTH32F = 4,					//32bits floating point depth buffer
	FBO_DepthBufferType_TEXTURE_PACKED_DEPTH32F_STENCIL8 = 5,	//32bits floating point depth buffer + packed 8bits stencil (64bits per pixel)
	FBO_DepthBufferType_TEXTURE_FROM_ANOTHER_FBO = 6			//link to this FBO the same dept buffer as another FBO passed in as a parameter
} FBO_DepthBufferType;

class Texture;

class FrameBufferObject  :public base::BaseObject
{

public:

	enum FrameType
	{
		CFI,
		CO
	};

	FrameBufferObject(unsigned int target,unsigned int width, unsigned int height,
		unsigned int nbColorBuffer,
		const unsigned int* colorBufferInternalFormat,
		const unsigned int* colorBufferSWRAP,
		const unsigned int* colorBufferTWRAP,
		const unsigned int* colorBufferMinFiltering,
		const unsigned int* colorBufferMagFiltering,
		FBO_DepthBufferType depthBufferType,
		const unsigned int depthBufferMinFiltering,
		const unsigned int depthBufferMagFiltering,
		const unsigned int depthBufferSWRAP,
		const unsigned int depthBufferTWRAP,
		bool depthTextureCompareToR ,
		FrameBufferObject* fboContainingDepthBuffer);

	FrameBufferObject(Texture * colorTextures, unsigned int nbColorBuffer);

	~FrameBufferObject();

	FrameBufferObject();
	
	void		colorTextureAttachments(Texture *, unsigned int nColorBuffer);

	unsigned int getNumberOfColorAttachement() const;
	
	FBO_DepthBufferType getDepthBufferType() const;
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	static const int getMaxColorAttachments();
	
	static const int getMaxBufferSize();
	
	void enableRenderToColorAndDepth(const unsigned int colorBufferNum) const;
	
	void FrameBufferObject::enableRenderToColorAndDepth_MRT(const unsigned int numBuffers, const  unsigned int* drawbuffers) const;
	 
	static void disableRenderToColorDepth();

	 
	void saveAndSetViewPort() const;
	 
	void restoreViewPort() const;

	 
	void bindColorTexture(const unsigned int colorBufferNum) const;
	 
	void bindDepthTexture() const;

	 
	void generateColorBufferMipMap(const unsigned int colorBufferNum) const;
	 
	void generateDepthBufferMipMap() const;
	 
	bool isColorMipMapPyramidValid(const unsigned int colorBufferNum) const;
	 
	bool isDepthMipMapPyramidValid() const;
	void saveToDisk(const unsigned int colorBufferNum, const char* filepath) const;

	bool isFrameBufferComplete();

private:
	void  colorBuffer(unsigned int target, unsigned int width, unsigned int height,
		const unsigned int* colorBufferInternalFormat, const unsigned int* colorBufferSWRAP,
		const unsigned int* colorBufferTWRAP, const unsigned int* colorBufferMinFiltering,
		const unsigned int* colorBufferMagFiltering);
	void  depthBuffer(FBO_DepthBufferType depthBufferType,
		const unsigned int depthBufferMinFiltering, const unsigned int depthBufferMagFiltering,
		const unsigned int depthBufferSWRAP, const unsigned int depthBufferTWRAP, bool depthTextureCompareToR);
	void  framebuffer(unsigned int nbColorBuffer, FrameBufferObject* fboContainingDepthBuffer,bool bindDefault = true);

	private:

		unsigned int fbo_;
		unsigned int* colorTextures_ = nullptr;
		int nbColorAttachement_;
		unsigned int *colorMinificationFiltering_ = nullptr;
		unsigned int depthID_;
		FBO_DepthBufferType depthType_;
		unsigned int depthMinificationFiltering_;
		unsigned int width_;
		unsigned int height_;
		unsigned int target_;

		mutable bool* validColorMipMapPyramid_ = nullptr;
		mutable bool validDepthMipMapPyramid_;

		FrameBufferObject(FrameBufferObject&);
		FrameBufferObject& operator=(const FrameBufferObject&);

		FrameType  type_;

};


#endif
