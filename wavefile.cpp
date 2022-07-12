#include "wavefile.h"
#include <QDebug>


WaveFile::WaveFile()
{

}

WaveFile::~WaveFile()
{
	if (Data)
	{
		delete Data;
	}
}

bool WaveFile::WavRead(string filename)
{
	std::thread readThread(&WaveFile::readWave, this, filename);
	readThread.detach();
	return true;
}

bool WaveFile::readWave(string filename)
{
	if (Data)
	{
		std::unique_lock<std::mutex> locker(m_mtx);
		delete Data;
		Data = nullptr;
	}

	FILE *fp = nullptr;
	if ((fp = fopen(filename.c_str(), "rb")) == NULL)
		return false;

	int nSizeH = sizeof(head);

	fread(&head, sizeof(head), 1, fp);

	char datasign[4];
	fread(datasign, 4, 1, fp);			// "DATA" 子块数据域大小
	fread(&m_nDataLength, 4, 1, fp);	// PCM音频数据长度

	m_nTotalSample = m_nDataLength / head.adjustnum;		// 采样数 = 文件总长 / 采样位深
	m_nBitPerSample = head.databitnum / head.channelnum;	// 每秒数据大小 / 声道数
	m_nDataNum = m_nTotalSample * m_nBitPerSample / 16;	// 总采样数 * 每

	//////////////////////////////////////////////////////////////////////////
	m_bytePerSample = head.databitnum / 8;
	m_nSampleNum = m_nDataLength / m_bytePerSample;

	m_nChannelNum = head.channelnum;

	std::unique_lock<std::mutex> locker(m_mtx);		
	unsigned char chTemp;
	unsigned short nTemp;

	//单字节样本值v为无符号整数(0~255)，实际样本值应为v-128；多字节样本值本身就是有符号的，可直接使用
	if (m_nChannelNum == 1)	// 单声道
	{
		Data = new short[m_nSampleNum];
		if (m_bytePerSample == 1)
		{
			for (int i = 0; i < m_nSampleNum; i++)
			{
				fread(&chTemp, 1, 1, fp);
				Data[i] = chTemp - 128;
			}
		}
		else if (m_bytePerSample == 2)
		{
			for (int i = 0; i < m_nSampleNum; i++)
			{
				fread(&nTemp, 2, 1, fp);
				Data[i] = (short)nTemp;
			}
		}
		else if (m_bytePerSample == 3)
		{
			for (int i = 0; i < m_nSampleNum; i++)
			{
				fread(&chTemp, 1, 1, fp);
				fread(&nTemp, 2, 1, fp);
				Data[i] = chTemp + nTemp;
			}
		}
		else if (m_bytePerSample == 4)
		{
			for (int i = 0; i < m_nSampleNum; i++)
			{
				fread(&nTemp, 2, 1, fp);
				Data[i] = (short)nTemp;
			}
		}
	}
	else if (m_nChannelNum == 2)	//	双声道
	{
		Data = new short[m_nSampleNum / 2];
		if (m_bytePerSample == 1)
		{
			for (int i = 0; i < m_nSampleNum / 2; i++)
			{
				fread(&chTemp, 1, 1, fp);
				Data[i] = chTemp - 128;

				fread(&chTemp, 1, 1, fp);
			}
		}
		else if (m_bytePerSample == 2)
		{
			for (int i = 0; i < m_nSampleNum / 2; i++)
			{
				fread(&nTemp, 2, 1, fp);
				Data[i] =nTemp;

				fread(&nTemp, 2, 1, fp);
			}
		}
		else if (m_bytePerSample == 3)
		{
			for (int i = 0; i < m_nSampleNum / 2; i++)
			{
				fread(&chTemp, 1, 1, fp);
				fread(&nTemp, 2, 1, fp);
				Data[i] = chTemp + nTemp;

				fread(&chTemp, 1, 1, fp);
				fread(&nTemp, 2, 1, fp);
			}
		}
		else if (m_bytePerSample == 4)
		{
			for (int i = 0; i < m_nSampleNum / 2; i++)
			{
				fread(&nTemp, 2, 1, fp);
				Data[i] = (short)nTemp;

				fread(&nTemp, 2, 1, fp);
			}
		}
	}

	fclose(fp);
	return true;
}

short* WaveFile::getData()
{
	std::unique_lock<std::mutex> locker(m_mtx);
	return Data;
}

unsigned short WaveFile::getChannelNum()
{
	return m_nChannelNum;
}

unsigned short WaveFile::getBytePerSample()
{
	return m_nBitPerSample;
}

int WaveFile::getSampleNum()
{
	return m_nSampleNum;
}
