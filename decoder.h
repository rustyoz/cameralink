#ifndef DECODER_H
#define DECODER_H

#include <libavcodec/avcodec.h>
//#include <libavutil/frame>
#include <libswscale/swscale.h>
#include <opencv2/opencv.hpp>


typedef void (*decodeCallBack)(void *camlink, cv::Mat);

class Decoder
{
	private:
		cv::Mat avframe_to_cvmat(AVFrame *frame);

protected:
	AVCodecContext  *m_pCodecCtx;
	AVCodec         *m_pCodec;
	AVFrame         *m_pFrame;
	decodeCallBack  m_callback;
public:
    Decoder();
	Decoder(AVCodecID decoder, decodeCallBack callback);

	~Decoder();


	void DecodeStreamData(unsigned char * pData, size_t sz, void* pcamlink);


};

#endif // DECODER_H
