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
	fread(datasign, 4, 1, fp);			// "DATA" �ӿ��������С
	fread(&m_nDataLength, 4, 1, fp);	// PCM��Ƶ���ݳ���

	m_nTotalSample = m_nDataLength / head.adjustnum;		// ������ = �ļ��ܳ� / ����λ��
	m_nBitPerSample = head.databitnum / head.channelnum;	// ÿ�����ݴ�С / ������
	m_nDataNum = m_nTotalSample * m_nBitPerSample / 16;	// �ܲ����� * ÿ

	//////////////////////////////////////////////////////////////////////////
	m_bytePerSample = head.databitnum / 8;
	m_nSampleNum = m_nDataLength / m_bytePerSample;

	m_nChannelNum = head.channelnum;

	std::unique_lock<std::mutex> locker(m_mtx);		
	unsigned char chTemp;
	unsigned short nTemp;

	//���ֽ�����ֵvΪ�޷�������(0~255)��ʵ������ֵӦΪv-128�����ֽ�����ֵ��������з��ŵģ���ֱ��ʹ��
	if (m_nChannelNum == 1)	// ������
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
	else if (m_nChannelNum == 2)	//	˫����
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
