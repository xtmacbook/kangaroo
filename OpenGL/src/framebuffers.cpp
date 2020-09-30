#include "framebuffers.h"
#include "gls.h"
#include "log.h"
#include "texture.h"

bool CheckFramebufferStatus(const GLuint fbo)
{
	GLenum status;
	bool ret = false;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

	status = (GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		printf("Frame Buffer Complete");
		ret = true;
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
		printf("Unsupported framebuffer format");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
		printf("Framebuffer incomplete, missing attachment");
		break;
		//case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
		//    printf("Framebuffer incomplete, duplicate attachment");
		//   break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
		printf("Framebuffer incomplete attachment");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		printf("Framebuffer incomplete, attached images must have same dimensions");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		printf("Framebuffer incomplete, attached images must have same format");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
		printf("Framebuffer incomplete, missing draw buffer");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
		printf("Framebuffer incomplete, missing read buffer");
		break;
	default:
		printf("Framebuffer incomplete, UNKNOW ERROR");
		// assert(0);
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return ret;
}


void FrameBufferObject::colorBuffer(unsigned int target, unsigned int width, unsigned int height,
	const unsigned int* colorBufferInternalFormat, const unsigned int* colorBufferSWRAP,
	const unsigned int* colorBufferTWRAP, const unsigned int* colorBufferMinFiltering,
	const unsigned int* colorBufferMagFiltering )
{
	//color render buffer
	if (this->nbColorAttachement_ > 0)
	{
		this->colorTextures_ = new GLuint[this->nbColorAttachement_];
		this->colorMinificationFiltering_ = new GLuint[this->nbColorAttachement_];
		this->validColorMipMapPyramid_ = new bool[this->nbColorAttachement_];
		for (int i = 0; i < this->nbColorAttachement_; i++)
		{
			this->colorTextures_[i] = 0;
			glGenTextures(1, &this->colorTextures_[i]);
			glBindTexture(this->target_, this->colorTextures_[i]);
			glTexParameteri(this->target_, GL_TEXTURE_MIN_FILTER, colorBufferMinFiltering[i]);
			this->colorMinificationFiltering_[i] = colorBufferMinFiltering[i];
			glTexParameteri(this->target_, GL_TEXTURE_MAG_FILTER, colorBufferMagFiltering[i]);
			glTexParameteri(this->target_, GL_TEXTURE_WRAP_S, colorBufferSWRAP[i]);
			glTexParameteri(this->target_, GL_TEXTURE_WRAP_T, colorBufferTWRAP[i]);

			glTexImage2D(this->target_, 0, colorBufferInternalFormat[i], width, height, 0, GL_RGB, GL_FLOAT, NULL);

			this->validColorMipMapPyramid_[i] = false;
			if (this->colorMinificationFiltering_[i] == GL_NEAREST_MIPMAP_NEAREST
				|| this->colorMinificationFiltering_[i] == GL_LINEAR_MIPMAP_NEAREST
				|| this->colorMinificationFiltering_[i] == GL_NEAREST_MIPMAP_LINEAR
				|| this->colorMinificationFiltering_[i] == GL_LINEAR_MIPMAP_LINEAR)
			{
				glGenerateMipmapEXT(this->target_);
			}
		}
	}
}

void FrameBufferObject::depthBuffer( FBO_DepthBufferType depthBufferType,
	const unsigned int depthBufferMinFiltering, const unsigned int depthBufferMagFiltering,
	const unsigned int depthBufferSWRAP, const unsigned int depthBufferTWRAP, bool depthTextureCompareToR)
{
	this->depthType_ = depthBufferType;
	this->validDepthMipMapPyramid_ = false;
	if (this->depthType_ != FBO_DepthBufferType_NONE)
	{
		this->depthID_ = 0;
		switch (this->depthType_)
		{
		case FBO_DepthBufferType_TEXTURE:
			glGenTextures(1, &this->depthID_);
			glBindTexture(this->target_, this->depthID_);
			this->depthMinificationFiltering_ = depthBufferMinFiltering;
			glTexParameteri(this->target_, GL_TEXTURE_MIN_FILTER, depthBufferMinFiltering);
			glTexParameteri(this->target_, GL_TEXTURE_MAG_FILTER, depthBufferMagFiltering);
			glTexParameteri(this->target_, GL_TEXTURE_WRAP_S, depthBufferSWRAP);
			glTexParameteri(this->target_, GL_TEXTURE_WRAP_T, depthBufferTWRAP);
			if (depthTextureCompareToR)
				glTexParameteri(this->target_, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);

			glTexImage2D(this->target_, 0, GL_DEPTH_COMPONENT24_ARB, this->width_, this->height_, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

			if (this->depthMinificationFiltering_ == GL_NEAREST_MIPMAP_NEAREST
				|| this->depthMinificationFiltering_ == GL_LINEAR_MIPMAP_NEAREST
				|| this->depthMinificationFiltering_ == GL_NEAREST_MIPMAP_LINEAR
				|| this->depthMinificationFiltering_ == GL_LINEAR_MIPMAP_LINEAR)
			{
				glGenerateMipmapEXT(this->target_);
			}
			break;

		case FBO_DepthBufferType_TEXTURE_DEPTH32F:
			glGenTextures(1, &this->depthID_);
			glBindTexture(this->target_, this->depthID_);
			if (this->depthMinificationFiltering_ == GL_NEAREST)	//FORCE MIPMAP DISABLED (is it supported??)
				glTexParameteri(this->target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			else
			{
				this->depthMinificationFiltering_ = GL_LINEAR;
				glTexParameteri(this->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			glTexParameteri(this->target_, GL_TEXTURE_MAG_FILTER, depthBufferMagFiltering);
			glTexParameteri(this->target_, GL_TEXTURE_WRAP_S, depthBufferSWRAP);
			glTexParameteri(this->target_, GL_TEXTURE_WRAP_T, depthBufferTWRAP);
			if (depthTextureCompareToR)
				glTexParameteri(this->target_, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);

			glTexImage2D(this->target_, 0, GL_DEPTH_COMPONENT32F, this->width_, this->height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			break;

		case FBO_DepthBufferType_TEXTURE_PACKED_DEPTH24_STENCIL8:
			glGenTextures(1, &this->depthID_);
			glBindTexture(this->target_, this->depthID_);
			if (this->depthMinificationFiltering_ == GL_NEAREST)	//FORCE MIPMAP DISABLED
				glTexParameteri(this->target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			else
			{
				this->depthMinificationFiltering_ = GL_LINEAR;
				glTexParameteri(this->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			glTexParameteri(this->target_, GL_TEXTURE_MAG_FILTER, depthBufferMagFiltering);
			glTexParameteri(this->target_, GL_TEXTURE_WRAP_S, depthBufferSWRAP);
			glTexParameteri(this->target_, GL_TEXTURE_WRAP_T, depthBufferTWRAP);
			if (depthTextureCompareToR)
				glTexParameteri(this->target_, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);

			glTexImage2D(this->target_, 0, GL_DEPTH24_STENCIL8_EXT, this->width_, this->height_, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8_EXT, NULL);
			break;

		case FBO_DepthBufferType_TEXTURE_PACKED_DEPTH32F_STENCIL8:
			glGenTextures(1, &this->depthID_);
			glBindTexture(this->target_, this->depthID_);
			if (this->depthMinificationFiltering_ == GL_NEAREST)	//FORCE MIPMAP DISABLED
				glTexParameteri(this->target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			else
			{
				this->depthMinificationFiltering_ = GL_LINEAR;
				glTexParameteri(this->target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			glTexParameteri(this->target_, GL_TEXTURE_MAG_FILTER, depthBufferMagFiltering);
			glTexParameteri(this->target_, GL_TEXTURE_WRAP_S, depthBufferSWRAP);
			glTexParameteri(this->target_, GL_TEXTURE_WRAP_T, depthBufferTWRAP);
			if (depthTextureCompareToR)
				glTexParameteri(this->target_, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);

			glTexImage2D(this->target_, 0, GL_DEPTH32F_STENCIL8, this->width_, this->height_, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
			break;

		case FBO_DepthBufferType_RENDERTARGET:
		default:
			glGenRenderbuffersEXT(1, &this->depthID_);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->depthID_);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24_ARB, this->width_, this->height_);
			break;

		case FBO_DepthBufferType_TEXTURE_FROM_ANOTHER_FBO:
			//nothing to create
			break;
		}
	}
}

void FrameBufferObject::framebuffer(unsigned int nbColorBuffer, FrameBufferObject* fboContainingDepthBuffer, bool bindDefault)
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);

	//color render buffer attachement
	if (nbColorBuffer > 0)
	{
		for (int i = 0; i < this->nbColorAttachement_; i++)
		{
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, this->target_, this->colorTextures_[i], 0);
		}
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	}
	else
	{
		glReadBuffer(GL_NONE);
		glDrawBuffer(GL_NONE);
	}

	//depth render buffer attachement
	if (this->depthType_ != FBO_DepthBufferType_NONE)
	{
		switch (this->depthType_)
		{
		case FBO_DepthBufferType_RENDERTARGET:
		default:
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->depthID_);
			break;

		case FBO_DepthBufferType_TEXTURE:
		case FBO_DepthBufferType_TEXTURE_DEPTH32F:
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, this->target_, this->depthID_, 0);
			break;

		case FBO_DepthBufferType_TEXTURE_PACKED_DEPTH24_STENCIL8:
		case FBO_DepthBufferType_TEXTURE_PACKED_DEPTH32F_STENCIL8:
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, this->target_, this->depthID_, 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, this->target_, this->depthID_, 0);
			break;


		case FBO_DepthBufferType_TEXTURE_FROM_ANOTHER_FBO:
			switch (fboContainingDepthBuffer->depthType_)
			{
			case FBO_DepthBufferType_RENDERTARGET:
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fboContainingDepthBuffer->depthID_);
				this->depthID_ = 0;
				break;
			case FBO_DepthBufferType_TEXTURE:
			case FBO_DepthBufferType_TEXTURE_DEPTH32F:
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, this->target_, fboContainingDepthBuffer->depthID_, 0);
				this->depthID_ = 0;
				break;
			case FBO_DepthBufferType_TEXTURE_PACKED_DEPTH24_STENCIL8:
			case FBO_DepthBufferType_TEXTURE_PACKED_DEPTH32F_STENCIL8:
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, this->target_, fboContainingDepthBuffer->depthID_, 0);
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, this->target_, fboContainingDepthBuffer->depthID_, 0);
				this->depthID_ = 0;
				break;
			case FBO_DepthBufferType_NONE:
				break;
			}
			break;
		}
	}

	if(bindDefault) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	CheckFramebufferStatus(this->fbo_);
}

FrameBufferObject::FrameBufferObject(unsigned int target,unsigned int width, unsigned int height, 
	unsigned int nbColorBuffer,
	const unsigned int* colorBufferInternalFormat, const unsigned int* colorBufferSWRAP,
	const unsigned int* colorBufferTWRAP, const unsigned int* colorBufferMinFiltering,
	const unsigned int* colorBufferMagFiltering, FBO_DepthBufferType depthBufferType, 
	const unsigned int depthBufferMinFiltering, const unsigned int depthBufferMagFiltering,
	const unsigned int depthBufferSWRAP, const unsigned int depthBufferTWRAP, bool depthTextureCompareToR, 
	FrameBufferObject* fboContainingDepthBuffer)
{
	type_ = CFI;
	glGenFramebuffersEXT(1, &this->fbo_);

	this->target_ = target;
	this->width_ = width;
	this->height_ = height;
	this->nbColorAttachement_ = nbColorBuffer;
	if (this->nbColorAttachement_ > this->getMaxColorAttachments())
		this->nbColorAttachement_ = this->getMaxColorAttachments();

	colorBuffer(target,width,height,colorBufferInternalFormat,colorBufferSWRAP,colorBufferTWRAP,colorBufferMinFiltering,colorBufferMagFiltering);

	//depth render buffer
	depthBuffer(depthBufferType,depthBufferMinFiltering,depthBufferMagFiltering,depthBufferSWRAP,depthBufferTWRAP,depthTextureCompareToR);

	framebuffer(nbColorBuffer,fboContainingDepthBuffer);
}

FrameBufferObject::FrameBufferObject()
{
	type_ = CO;
	glGenFramebuffersEXT(1, &this->fbo_);
}

FrameBufferObject::FrameBufferObject(FrameBufferObject&)
{

}


FrameBufferObject::FrameBufferObject(Texture * colorTextures, unsigned int nbColorBuffer)
{
	type_ = CO;

	glGenFramebuffersEXT(1, &this->fbo_);

	this->target_ = colorTextures[0].target_;
	this->width_ = colorTextures[0].width_;
	this->height_ = colorTextures[0].height_;
	this->nbColorAttachement_ = nbColorBuffer;
	if (this->nbColorAttachement_ > this->getMaxColorAttachments())
		this->nbColorAttachement_ = this->getMaxColorAttachments();

	if (this->nbColorAttachement_ > 0)
	{
		this->colorTextures_ = new GLuint[this->nbColorAttachement_];
		this->colorMinificationFiltering_ = new GLuint[this->nbColorAttachement_];
		this->validColorMipMapPyramid_ = new bool[this->nbColorAttachement_];
		for (int i = 0; i < this->nbColorAttachement_; i++)
		{
			this->colorTextures_[i] = colorTextures[i].getTexture();
			this->colorMinificationFiltering_[i] = colorTextures[i].eMinFilter;
			this->validColorMipMapPyramid_[i] = false;
		}
	}

	depthType_ = FBO_DepthBufferType_NONE;

	framebuffer(nbColorBuffer, nullptr);
}

void FrameBufferObject::colorTextureAttachments(Texture *colorTextures, unsigned int nbColorBuffer)
{
	type_ = CO;
	this->target_ = colorTextures[0].target_;
	this->width_ = colorTextures[0].width_;
	this->height_ = colorTextures[0].height_;
	this->nbColorAttachement_ = nbColorBuffer;
	if (this->nbColorAttachement_ > this->getMaxColorAttachments())
		this->nbColorAttachement_ = this->getMaxColorAttachments();

	if (this->nbColorAttachement_ > 0)
	{
		if (this->colorTextures_ == nullptr)
		{
			this->colorTextures_ = new GLuint[this->nbColorAttachement_];
			this->colorMinificationFiltering_ = new GLuint[this->nbColorAttachement_];
			this->validColorMipMapPyramid_ = new bool[this->nbColorAttachement_];
		}
		for (int i = 0; i < this->nbColorAttachement_; i++)
		{
			this->colorTextures_[i] = colorTextures[i].getTexture();
			this->colorMinificationFiltering_[i] = colorTextures[i].eMinFilter;
			this->validColorMipMapPyramid_[i] = false;
		}
	}

	depthType_ = FBO_DepthBufferType_NONE;

	framebuffer(nbColorBuffer, nullptr,false);
}

FrameBufferObject::~FrameBufferObject()
{
	
	if (type_ == CFI && this->nbColorAttachement_ > 0)
	{
		glDeleteTextures(this->nbColorAttachement_, this->colorTextures_);

		switch (this->depthType_)
		{
		case FBO_DepthBufferType_RENDERTARGET:
			glDeleteRenderbuffersEXT(1, &this->depthID_);
			break;
		case FBO_DepthBufferType_NONE:
			break;
		default:
			glDeleteTextures(1, &this->depthID_);
			break;
		}
	}
	
	glDeleteFramebuffers(1, &this->fbo_);

	delete[] this->colorTextures_;
	delete[] this->colorMinificationFiltering_;
	delete[] this->validColorMipMapPyramid_;
}

unsigned int FrameBufferObject::getNumberOfColorAttachement() const
{
	return this->nbColorAttachement_;
}

FBO_DepthBufferType FrameBufferObject::getDepthBufferType() const
{
	return this->depthType_;
}

unsigned int FrameBufferObject::getWidth() const
{
	return width_;
}

unsigned int FrameBufferObject::getHeight() const
{
	return height_;
}

const int FrameBufferObject::getMaxColorAttachments()
{
	GLint maxAttach = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxAttach);
	return maxAttach;
}

const int FrameBufferObject::getMaxBufferSize()
{
	GLint maxSize = 0;
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &maxSize);
	return maxSize;
}

void FrameBufferObject::enableRenderToColorAndDepth(const unsigned int colorBufferNum) const
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);

	if (this->nbColorAttachement_ > 0)
	{
		this->validColorMipMapPyramid_[colorBufferNum] = false;
		this->validDepthMipMapPyramid_ = false;
		unsigned int colorBuffer = GL_COLOR_ATTACHMENT0_EXT + colorBufferNum;
		if ((int)colorBufferNum > this->nbColorAttachement_)
			colorBuffer = GL_COLOR_ATTACHMENT0_EXT;
		glDrawBuffer(colorBuffer);
	}
	else
	{
		glDrawBuffer(GL_NONE);	 
	}
}

void FrameBufferObject::disableRenderToColorDepth()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void FrameBufferObject::saveAndSetViewPort() const
{
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, this->width_, this->height_);
}

void FrameBufferObject::restoreViewPort() const
{
	glPopAttrib();
}

void FrameBufferObject::bindColorTexture(const unsigned int colorBufferNum) const
{
	if (this->nbColorAttachement_ > 0 && (int)colorBufferNum < this->nbColorAttachement_)
	{
		glBindTexture(this->target_, this->colorTextures_[colorBufferNum]);
	}
	else
		glBindTexture(this->target_, 0);
}

void FrameBufferObject::bindDepthTexture() const
{
	if (this->depthType_ >= FBO_DepthBufferType_TEXTURE)
	{
		glBindTexture(this->target_, this->depthID_);
	}
	else
		glBindTexture(this->target_, 0);
}

void FrameBufferObject::generateColorBufferMipMap(const unsigned int colorBufferNum) const
{
	if (this->nbColorAttachement_ > 0 && (int)colorBufferNum < this->nbColorAttachement_)
	{
		if (this->colorMinificationFiltering_[colorBufferNum] == GL_NEAREST
			|| this->colorMinificationFiltering_[colorBufferNum] == GL_LINEAR)
			return;	 

		glBindTexture(this->target_, this->colorTextures_[colorBufferNum]);
		glGenerateMipmapEXT(this->target_);
		this->validColorMipMapPyramid_[colorBufferNum] = true;
	}
}

void FrameBufferObject::generateDepthBufferMipMap() const
{
	if (this->depthType_ >= FBO_DepthBufferType_TEXTURE)
	{
		if (this->depthMinificationFiltering_ == GL_NEAREST
			|| this->depthMinificationFiltering_ == GL_LINEAR)
			return; 

		glBindTexture(this->target_, this->depthID_);
		glGenerateMipmapEXT(this->target_);
		this->validDepthMipMapPyramid_ = true;
	}
}

bool FrameBufferObject::isColorMipMapPyramidValid(const unsigned int colorBufferNum) const
{
	if (this->nbColorAttachement_ > 0 && (int)colorBufferNum < this->nbColorAttachement_)
	{
		return this->validColorMipMapPyramid_[colorBufferNum];
	}
	else
		return false;
}

bool FrameBufferObject::isDepthMipMapPyramidValid() const
{
	return this->validDepthMipMapPyramid_;
}

void FrameBufferObject::saveToDisk(const unsigned int colorBufferNum, const char* filepath) const
{

}

bool FrameBufferObject::isFrameBufferComplete()
{
	GLenum status;
	bool ret = false;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);

	status = (GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status == GL_FRAMEBUFFER_COMPLETE_EXT)
		ret = true;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return ret;
}

void FrameBufferObject::enableRenderToColorAndDepth_MRT(const unsigned int numBuffers, const unsigned int* drawbuffers) const
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);

	for (GLuint i = 0; i<numBuffers; i++)
	{
		this->validColorMipMapPyramid_[(unsigned int)(drawbuffers[i] - GL_COLOR_ATTACHMENT0_EXT)] = false;
	}
	this->validDepthMipMapPyramid_ = false;

	//HERE, THERE SHOULD HAVE A TEST OF THE DRAW BUFFERS ID AND NUMBER
	/*for(GLuint i=0; i<numBuffers; i++)
	{
	}*/
	glDrawBuffers(numBuffers, drawbuffers);
}
