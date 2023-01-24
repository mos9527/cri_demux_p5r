﻿#pragma once

//--------------------------------------------------
// インクルード
//--------------------------------------------------
#include "clUTF.h"

//--------------------------------------------------
// CRIDクラス
//--------------------------------------------------
class clCRID{
public:
	clCRID(UINT64 key);

	// ロード/開放
	static bool CheckFile(void *data,unsigned int size);
	bool LoadFile(const char *filename);

	// 分離/マルチプレクサ
	bool Demux(const char *filename,const char *directory, bool is_demux_video, bool is_demux_info, bool is_demux_audio, bool is_convert_adx);
	bool Mux(const char *filename,const char *filenameMovie,const char *filenameAudio);

	// 取得
	unsigned int GetFileCount(void){return _utf.GetPageCount();}
	const char *GetFilename(unsigned int index){return _utf.GetElement(index,"filename")->GetValueString();}

    void SetMaskAudioFromFile(FILE *mask);
	void ClearMaskAudio();
private:
	struct stInfo{
		unsigned int signature;      // シグネチャ 'CRID'
		unsigned int dataSize;       // データサイズ
		unsigned char r08;           // 不明(0)
		unsigned char dataOffset;    // データオフセット
		unsigned short paddingSize;  // パディングサイズ
		unsigned char chno;          // 不明(0)
		unsigned char r0D;           // 不明(0)
		unsigned char r0E;           // 不明(0)
		unsigned char dataType:2;    // データの種類 0:Data 1:UTF(メタ情報) 2:Comment 3:UTF(シーク情報)
		unsigned char r0F_1:2;       // 不明(0)
		unsigned char r0F_2:4;       // 不明(0)
		unsigned int frameTime;      // フレーム時間(0.01秒単位)
		unsigned int frameRate;      // フレームレート(0.01fps単位)
		unsigned int r18;            // 不明(0)
		unsigned int r1C;            // 不明(0)
	};
	clUTF _utf;
	unsigned char _videoMask1[0x20];
	unsigned char _videoMask2[0x20];
	unsigned char _audioMask[0x20];
	void InitMask(UINT64 key);
	void MaskVideo(unsigned char *data,int size);
	void MaskAudio(unsigned char *data,int size);
	static void WriteInfo(FILE *fp,const char *string);
};
