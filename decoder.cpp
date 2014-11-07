#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
#include <opencv2/opencv.hpp>
#include "decoder.h"



    Decoder::Decoder()
    {};

    Decoder::Decoder(AVCodecID decoder, decodeCallBack callback)
    {

        m_pCodec= avcodec_find_decoder(decoder);
        m_pCodecCtx = avcodec_alloc_context3(m_pCodec);
        avcodec_open2(m_pCodecCtx,m_pCodec,0);
		m_pFrame = av_frame_alloc();
		m_callback = callback;

    }

    Decoder::~Decoder()
    {
        av_free(m_pFrame);
        avcodec_close(m_pCodecCtx);
    }

	cv::Mat Decoder::avframe_to_cvmat(AVFrame *frame)
	{
		AVFrame dst;
		cv::Mat m;

		memset(&dst, 0, sizeof(dst));

		int w = frame->width, h = frame->height;
		m = cv::Mat(h, w, CV_8UC3);
		dst.data[0] = (uint8_t *)m.data;
		avpicture_fill((AVPicture *)&dst, dst.data[0], PIX_FMT_BGR24, w, h);

		struct SwsContext *convert_ctx = NULL;
		enum PixelFormat src_pixfmt = (enum PixelFormat)frame->format;
		enum PixelFormat dst_pixfmt = PIX_FMT_BGR24;
		convert_ctx = sws_getContext(w, h, src_pixfmt, w, h, dst_pixfmt,
			SWS_FAST_BILINEAR, NULL, NULL, NULL);
		sws_scale(convert_ctx, frame->data, frame->linesize, 0, h,
			dst.data, dst.linesize);
		sws_freeContext(convert_ctx);

		return m;
	}

	void Decoder::DecodeStreamData(unsigned char * pData, size_t sz, void* pcamlink)
    {
        AVPacket        packet;
        av_init_packet(&packet);

        packet.data=pData;
        packet.size=(int)sz;
        int framefinished=0;
        int nres = avcodec_decode_video2(this->m_pCodecCtx,this->m_pFrame,&framefinished,&packet);

        if(framefinished)
        {
			cv::Mat frame = avframe_to_cvmat(m_pFrame);

			this->m_callback(pcamlink, frame);

        }

        return;
    }



