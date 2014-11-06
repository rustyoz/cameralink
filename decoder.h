#include "libavcodec\avcodec.h"
#include "libavutil\frame.h"
#include "libswscale\swscale.h"
#include "opencv.hpp"

typedef int (__stdcall *decodecallback)(cv::Mat frame);

class decoder
{
	private:
		cv::Mat avframe_to_cvmat(AVFrame *frame); 

protected:
	AVCodecContext  *m_pCodecCtx;
	AVCodec         *m_pCodec;
	AVFrame         *m_pFrame;
	decodecallback  m_callback;
public:
	decoder(AVCodecID decoder, decodecallback callback);

	~decoder();
	

	void decodeStreamData(unsigned char * pData, size_t sz);


};