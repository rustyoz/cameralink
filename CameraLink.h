//#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "NetSDKDLL.h"
#include "DllPlayer.h"
#include "stdafx.h"
#include <string.h>
#include "FPS.h"

using namespace std;
using namespace cv;

typedef struct _StateEventParam
{
	LONG lUser;
	LONG nStateCode;
	char pResponse[2048];
	void *pUser;
}StateEventParam;

class CameraLink
{
private:
	IPC_ENTRY devinfo;
	IP_NET_DVR_DEVICEINFO devinfo2;
	//memset(&devinfo2, 0x00, sizeof(IP_NET_DVR_DEVICEINFO));
	void *consolehandle;
	Mat	YUVToBGR(uchar *pBuffer, long bufferSize, int width, int height);
	STREAM_AV_PARAM avParam;


	UserAccount user;

	fDecCallBackFunction mrcallback;

	LONG lRealHandle;
	USRE_VIDEOINFO videoinfo;

	fRealDataCallBack rdcallback;

	int m_bSetParam = -1;



public:
	CameraLink(string ipaddress, string username, string password, int port);
	int CameraLinkInit();
	void CameraLink::SetRealDataCallBack(CameraLink *camlink);
	void CameraLink::SetOnMediaRecvDataCallback(CameraLink *camlink);

	static void CameraLinkLogin(CameraLink *camlink);
	static long __stdcall OnLoginOkCallBack(CameraLink *camlink);
	static LONG __stdcall OnRealDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LPFRAME_EXTDATA  pExtData);
	static LONG __stdcall OnStatusEventCallBack(LONG lUser, LONG nStateCode, char *pResponse, void *pUser);
	static int __stdcall OnMediaDataRecv(long nPort, char * pBuf, long nSize, FRAME_INFO * pFrameInfo, void * pUser, long nReserved2);
	bool CameraLink::ReadFrame(Mat &image);

	int loggedin = 0;
	LONG lUserID = 0;

	int frameread = 0;
	Mat lastyframe;
	Mat lastuframe;
	Mat lastvframe;
	Mat lastyuvframe;
	Mat lastbgrframe;

	Fps fps;
};