//original by nyaga (?)
//20190706 mod by bnnm
//20230124 mod by mos9527
//--------------------------------------------------
// インクルード
//--------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <direct.h>  // _mkdir
#include "clCRID.h"
#ifndef _countof
#define _countof(_array) (sizeof(_array)/sizeof(_array[0]))
#endif
#define KEY_PERSONA_5_ROYAL_SWITCH_PC 0x2341683D2FDBA6

#define MSG_HELP "cri_demux_p5r " __DATE__ " " __TIME__  " [options] file.usm\n" \
    "-o (name) [internal name output folder]\n" \
    "-n (use internal names)\n" \
    "-k (key) [8-bytes USM key]\n" \
    "-m (file) [32-bytes audio mask keyfile]\n" \
    "-x [demux audio]\n" \
    "-v [demux video]\n" \
    "-i [demux info]\n" \
    "-c [convert adx to wav instead of demuxing]\n" 
#define MSG_DEMUXING   u8"%s を分離中...\n"
#define MSG_INPUT      u8"Error: 入力ファイルを指定してください。\n"
#define MSG_FAIL       u8"Error: 分離に失敗しました。\n"
#define MSG_FILE       u8"Error: file not found\n"
#define MSG_OPTIONS    u8"Error: missing options\n"


//--------------------------------------------------
// 文字列を16進数とみなして数値に変換
//--------------------------------------------------
UINT64 atoi16(const char* s) {
	INT64 r = 0;
	bool sign = false; if (*s == '+') { s++; }
	else if (*s == '-') { sign = true; s++; }
	while (*s) {
		if (*s >= '0' && *s <= '9')r = (r << 4) | (*s - '0');
		else if (*s >= 'A' && *s <= 'F')r = (r << 4) | (*s - 'A' + 10);
		else if (*s >= 'a' && *s <= 'f')r = (r << 4) | (*s - 'a' + 10);
		else break;
		s++;
	}
	return sign ? -r : r;	
}

//--------------------------------------------------
// ディレクトリを取得
//--------------------------------------------------
char* GetDirectory(char* directory, int size, const char* path) {
	if (size > 0)directory[0] = '\0';
	for (int i = strlen(path) - 1; i >= 0; i--) {
		if (path[i] == '\\') {
			if (i > size - 1)i = size - 1;
			memcpy(directory, path, i);
			directory[i] = '\0';
			break;
		}
	}
	return directory;
}

//--------------------------------------------------
// ディレクトリ作成
//--------------------------------------------------
bool DirectoryCreate(const char* directory) {

	// チェック
	if (!(directory && *directory))return false;

	// 相対パス(ディレクトリ名のみ)
	if (!(strchr(directory, '\\') || strchr(directory, '/'))) {
		return _mkdir(directory) == 0;
	}

	// ディレクトリ名チェック
	if (directory[1] != ':' || directory[2] != '\\')return false;  // ドライブ記述のチェック
	if (!directory[3])return false;                          // ドライブ以外の記述チェック
	if (strpbrk(directory + 3, "/,:;*<|>\""))return false;      // ディレクトリ禁止文字のチェック
	if (strstr(directory, "\\\\"))return false;               // 連続する'\'記号のチェック
	if (strstr(directory, " \\"))return false;                // スペースの後の'\'記号のチェック

	// ディレクトリ作成
	if (_mkdir(directory)) {
		char current[0x400];
		if (!GetDirectory(current, _countof(current), directory))return false;
		if (!DirectoryCreate(current))return false;
		if (_mkdir(directory))return false;
	}

	return true;
}

//--------------------------------------------------
// メイン
//--------------------------------------------------
int main(int argc, char* argv[]) {
	SetConsoleOutputCP(65001);
	// コマンドライン解析
	unsigned int count = 0;
	char* filenameOut = NULL;
	UINT64 ciphKey = KEY_PERSONA_5_ROYAL_SWITCH_PC;

	char* audiomask_name = NULL;
	bool is_demux_video = false;
	bool is_demux_info = false;
	bool is_demux_audio = false;
	bool is_convert_adx = false;
	bool is_internal_names = false;

	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-' || argv[i][0] == '/') {
			switch (argv[i][1]) {
			case 'o':if (i + 1 < argc) { filenameOut = argv[++i]; }break;
			case 'k':if (i + 1 < argc) { ciphKey = atoi16(argv[++i]); }break;			
			case 'm':if (i + 1 < argc) { audiomask_name = argv[++i]; }break;
			case 'v':if (i < argc) { is_demux_video = true; }break;
			case 'i':if (i < argc) { is_demux_info = true; }break;
			case 'x':if (i < argc) { is_demux_audio = true; is_convert_adx = false; }break;
			case 'c':if (i < argc) { is_convert_adx = true; is_demux_audio = true; }break;
			case 'n':if (i < argc) { is_internal_names = true; }break;
			}
		}
		else if (*argv[i]) {
			argv[count++] = argv[i];
		}
	}

	// 入力チェック
	if (!count) {
		printf(MSG_HELP);
		printf(MSG_INPUT);
		return -1;
	}

	if (!is_demux_video && !is_demux_info && !is_demux_audio && !is_convert_adx) {
		printf(MSG_HELP);
		printf(MSG_OPTIONS);
		return -1;
	}


	// 分離
	for (unsigned int i = 0; i < count; i++) {

		// 2つ目以降のファイルは、出力ファイル名オプションが無効
		if (i)filenameOut = NULL;

		// デフォルト出力ファイル名
		char path[0x400];
		if (!(filenameOut && filenameOut[0])) {
			strcpy_s(path, sizeof(path), argv[i]);
			char* d1 = strrchr(path, '\\');
			char* d2 = strrchr(path, '/');
			char* e = strrchr(path, '.');
			if (e && d1 < e && d2 < e)*e = '\0';
			if (is_internal_names)
				strcat_s(path, sizeof(path), ".demux");
			filenameOut = path;
		}

		printf(MSG_DEMUXING, argv[i]);
		if (is_internal_names)
			DirectoryCreate(filenameOut);
		clCRID crid(ciphKey);

		FILE* fp;
		if (audiomask_name != NULL) {
			if (fopen_s(&fp, audiomask_name, "rb")) {
				printf(MSG_FILE);
				return 0;
			}
			crid.SetMaskAudioFromFile(fp);
			fclose(fp);
		}


		if (!crid.Demux(argv[i], filenameOut, is_demux_video, is_demux_info, is_demux_audio, is_convert_adx, is_internal_names)) {
			printf(MSG_FAIL);
		}

	}

	return 0;
}
