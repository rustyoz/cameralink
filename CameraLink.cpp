#include "CameraLink.h"

using namespace std;
using namespace cv;



int CameraLink::CameraLinkInit()
	{

		this->SetRealDataCallBack(this);
		this->SetOnMediaRecvDataCallback(this);
		IP_NET_DVR_SetStatusEventCallBack(this->OnStatusEventCallBack, this);

		int success = 1;
		if (IP_NET_DVR_Init() == 0)
		{
			if (IP_TPS_Init() == 0)
			{
				cout << "IP_NET_DVR_INIT success\n";
				success = 0;
			}

		}
		return success;
	}


	void CameraLink::CameraLinkLogin(CameraLink *camlink)
	{

		LONG UserID = IP_NET_DVR_Login(camlink->devinfo.lanCfg.IPAddress, camlink->devinfo.streamCfg.ptzPort, camlink->devinfo.userCfg.accounts[0].userName, camlink->devinfo.userCfg.accounts[0].password, NULL);
		camlink->lUserID = UserID;

	}
	void CameraLink::SetRealDataCallBack(CameraLink *camlink)
	{
		camlink->rdcallback = &camlink->OnRealDataCallBack;
	}


	CameraLink::CameraLink(string ipaddress, string username, string password, int port)
	{
		memset(this, 0, sizeof(CameraLink));

		

		this->videoinfo.nVideoPort = 554;
		this->videoinfo.nVideoChannle = 0;
		this->videoinfo.pUserData = this;

		strcpy_s(user.userName, username.c_str());
		strcpy_s(user.password, password.c_str());
		devinfo.streamCfg.ptzPort = port;
		strcpy_s(devinfo.lanCfg.IPAddress, ipaddress.c_str());
		devinfo.userCfg.accounts[0] = user;
		m_bSetParam = -1;

		lastyframe.create(360, 656, CV_8UC1);
		lastuframe.create(360, 656, CV_8UC1);
		lastvframe.create(360, 656, CV_8UC1);
		lastbgrframe.create(720, 1312, CV_8UC3);
		lastyuvframe.create(720, 1312, CV_8UC3);
	}

	LONG __stdcall CameraLink::OnStatusEventCallBack(LONG lUser, LONG nStateCode, char *pResponse, void *pUser)
	{
		CameraLink *camlink = (CameraLink *)pUser;
		StateEventParam * pParam = new StateEventParam();
		if (NULL == pParam)
		{
			return -1;
		}
		memset(pParam, 0, sizeof(StateEventParam));

		pParam->lUser = lUser;
		if (nStateCode == EVENT_LOGINOK)
		{
			camlink->loggedin = 1;
			memcpy(pParam->pResponse, pResponse, sizeof(ALARM_ITEM));

			OnLoginOkCallBack(camlink);
		}
		if (nStateCode == EVENT_LOGINFAILED)
		{
			camlink->loggedin = -1;
		}

		if (nStateCode == EVENT_PTZALARM)
		{
			memcpy(pParam->pResponse, pResponse, sizeof(ALARM_ITEM));
		}
		else if (EVENT_RECVVIDEOPARAM == nStateCode)
		{
			memcpy(pParam->pResponse, pResponse, sizeof(VIDEO_PARAM));
		}
		else if (EVENT_RECVAUDIOPARAM == nStateCode)
		{
			memcpy(pParam->pResponse, pResponse, sizeof(AUDIO_PARAM));
		}
		else if (EVENT_RECVVIDEOAUDIOPARAM == nStateCode)
		{
			memcpy(pParam->pResponse, pResponse, sizeof(STREAM_AV_PARAM));
		}
		else
		{
			//sprintf_s(pParam->pResponse, 2048, "%s", pResponse);
			//cout << pResponse;
		}
		cout << pResponse << "\n";

		pParam->nStateCode = nStateCode;
		pParam->pUser = pUser;
		return 0;
	}


	LONG __stdcall CameraLink::OnRealDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LPFRAME_EXTDATA  pExtData)
	{
		CameraLink *camlink = (CameraLink *)pExtData->pUserData;

		if (dwDataType == 0)
		{
			return IP_TPS_InputVideoData(0, pBuffer, dwBufSize, pExtData->bIsKey, (int)pExtData->timestamp);
		}
		else if (dwDataType == 1)
		{

		}
		else if (dwDataType == 2 && camlink->m_bSetParam == -1)
		{
			STREAM_AV_PARAM *pavParam = (STREAM_AV_PARAM *)pBuffer;
			memcpy(&camlink->avParam, pavParam, sizeof(STREAM_AV_PARAM));

			long lRet = IP_TPS_OpenStream(0, (BYTE *)&(camlink->avParam.videoParam), sizeof(VIDEO_PARAM), 0, 1);
			if (lRet != 0)
			{
				cout << "Video IP_TPS_OpenStream faild!!!" << "\n";
				return 0;
			}
			IP_TPS_SetDecCallBack(0, OnMediaDataRecv, camlink);
			camlink->m_bSetParam = 1;
			IP_TPS_Play(0, 0);
		}



		return 0;
	}

	Mat CameraLink::YUVToBGR(uchar *pBuffer, long bufferSize, int width, int height)
	{
		cv::Mat result(height, width, CV_8UC3);
		uchar y, cb, cr;

		long ySize = width*height;
		long uSize;
		uSize = ySize >> 2;

		//assert(bufferSize == ySize + uSize * 2);

		uchar *output = result.data;
		uchar *pY = pBuffer;
		uchar *pU = pY + ySize;
		uchar *pV = pU + uSize;

		uchar r, g, b;
		for (int i = 0; i < uSize; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				y = pY[i * 4 + j];
				cb = pU[i];
				cr = pV[i];

				//ITU-R standard
				b = saturate_cast<uchar>(y + 1.772*(cb - 128));
				g = saturate_cast<uchar>(y - 0.344*(cb - 128) - 0.714*(cr - 128));
				r = saturate_cast<uchar>(y + 1.402*(cr - 128));


				*output++ = b;
				*output++ = g;
				*output++ = r;
			}
		}
		return result;
	}


	int __stdcall CameraLink::OnMediaDataRecv(long nPort, char * pBuf, long nSize, FRAME_INFO * pFrameInfo, void * pUser, long nReserved2)
	{
		CameraLink *camlink = (CameraLink *)pUser;

		while (camlink->frameread != 0)
		{

		}
		

		Rect roi = Rect(0, 0, 1280, 720);

		uchar *pBuffer = (uchar*)(pBuf + 16);
		Mat yframe = Mat(720, 1312, CV_8U, pBuffer);

		//memcpy_s((char*)yframe.data, (char*)pBuffer, 720 * 1312 * sizeof(char),  720 * 1312 * sizeof(char));
		camlink->lastyframe = yframe;

		pBuffer = pBuffer + (720 * 1312);
		Mat uframe = Mat(360, 656, CV_8U, pBuffer);
		//memcpy_s((char*)uframe.data, (char*)pBuffer, 360 * 656 * sizeof(char),  720 * 1312 * sizeof(char));
		resize(uframe, camlink->lastuframe, Size(1312, 720), 0, 0, CV_INTER_LINEAR);

		pBuffer = pBuffer + (360 * 656);
		Mat vframe = Mat(360, 656, CV_8U, pBuffer);
		//memcpy_s((char*)vframe.data, (char*)pBuffer, 360 * 656 * sizeof(char),  720 * 1312 * sizeof(char));
		resize(vframe, camlink->lastvframe, Size(1312, 720), 0, 0, CV_INTER_LINEAR);

		std::vector<cv::Mat> frames;

		frames.push_back(camlink->lastyframe);
		frames.push_back(camlink->lastuframe);
		frames.push_back(camlink->lastvframe);

		merge(frames, camlink->lastyuvframe);

		Mat frame;

		cvtColor(camlink->lastyuvframe, frame, CV_YUV2BGR);

		camlink->lastbgrframe = frame(roi);
		camlink->frameread = 1;
		//camlink->fps.update();
		/*
		ofstream rawimage("bgrimage.bin");
		rawimage.write(reinterpret_cast<char*>(frame.data), 1312 * 720 * 3);
		ofstream yuvimage("rawimage.bin");
		yuvimage.write(pBuf+16, nSize-16);
		rawimage.close();
		yuvimage.close();
		*/

		



		return 0;


	}


	bool CameraLink::ReadFrame(Mat& image)
	{
		if (this->frameread == 1)
		{
			image = this->lastbgrframe.clone();
			this->frameread = 0;
		}
		else
		return false;
	}
		

	
	void CameraLink::SetOnMediaRecvDataCallback(CameraLink *camlink)
	{
		camlink->mrcallback = &camlink->OnMediaDataRecv;
	}

	LONG __stdcall CameraLink::OnLoginOkCallBack(CameraLink *camlink)
	{
		fRealDataCallBack cb = camlink->OnRealDataCallBack;
		camlink->videoinfo.pUserData = camlink;
		camlink->lRealHandle = IP_NET_DVR_RealPlayEx(camlink->lUserID, camlink->devinfo.lanCfg.IPAddress, camlink->devinfo.userCfg.accounts[0].userName, camlink->devinfo.userCfg.accounts[0].password, cb, &camlink->videoinfo, 0);

		return 0;
	}

	void CameraLink::Close()
	{
		
		IP_NET_DVR_StopRealPlay(this->lRealHandle);
		IP_TPS_ReleaseAll();
		IP_NET_DVR_Cleanup();
	}