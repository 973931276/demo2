#ifndef WAVEFILE_H
#define WAVEFILE_H

#include <thread>
#include <mutex>
#include <string>
using namespace std;

class WaveFile
{
public:
	struct waveHead
	{
		char sign[4];				// "RIFF"标志 4
		unsigned long int flength;     // 文件长度 8
		char wavesign[4];     // "WAVE"标志 12
		char fmtsign[4];       // "fmt"标志 16
		unsigned long int unused;			// 过渡字节（不定）20

		unsigned short formattype;		// 格式类别（10H为PCM形式的声音数据) 22
		unsigned short  channelnum;		// 声道数，单声道为1，双声道为2  24

		unsigned long int  samplerate;	// 采样频率（每秒样本数），表示每个通道的播放速度 28
		unsigned long int transferrate;   // 数据传输速率，每秒字节数

		unsigned short int adjustnum;    // 每样本字节数，一个数据单位所占的字节/单个采样位深
		unsigned short int databitnum;  // 每样本位数，调整数*8 36
	}head;

public:
	WaveFile();
	~WaveFile();

public:
	bool WavRead(string filename);
	bool readWave(string filename);

	short* getData();
	unsigned short getChannelNum();
	unsigned short getBytePerSample();
	int getSampleNum();		// 样本数

	short* Data = nullptr; //数据块指针

protected:
public:
	unsigned long int m_nDataLength = 0;		// 采样数据总数
	unsigned long int m_nTotalSample = 0;		// 采样点数
	unsigned long int m_nBitPerSample = 0;	// 采样位数
	unsigned long int m_nDataNum = 0;         // 数据块大小


	unsigned short  m_nChannelNum = 0;
	unsigned short  m_bytePerSample = 0;
	int m_nSampleNum = 0;		// 样本数
	std::mutex		m_mtx;
};

#endif // WAVEFILE_H
