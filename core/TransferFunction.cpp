#include "TransferFunction.h"

using namespace MonkeyGL;

TransferFunction::TransferFunction(void)
{
	m_nMinPos = -2048;
	m_nMaxPos = 2048;
}


TransferFunction::~TransferFunction(void)
{
}

bool TransferFunction::GetTransferFunction( RGBA*& pBuffer, int& nLen )
{
	std::map<int, RGBA>::iterator iter = m_pos2rgba.begin();
	while (iter != m_pos2rgba.end())
	{
		if (iter->first<m_nMinPos || iter->first>m_nMaxPos)
		{
			m_pos2rgba.erase(iter);
			iter = m_pos2rgba.begin();
			continue;
		}
		iter++;
	}

	if (m_pos2rgba.size() < 2)
		return false;

	if (m_pos2rgba.begin()->first != m_nMinPos)
	{
		RGBA& rgba = m_pos2rgba.begin()->second;
		m_pos2rgba[m_nMinPos] = RGBA(rgba.red, rgba.green, rgba.blue, 0);
	}
	if (m_pos2rgba.rbegin()->first != m_nMaxPos)
	{
		RGBA& rgba = m_pos2rgba.rbegin()->second;
		m_pos2rgba[m_nMaxPos] = RGBA(rgba.red, rgba.green, rgba.blue, rgba.alpha);
	}

	nLen = m_nMaxPos - m_nMinPos + 1;
	if (NULL != pBuffer)
	{
		delete [] pBuffer;
	}
	pBuffer = new RGBA[nLen];

	iter = m_pos2rgba.begin();
	int posPrev = iter->first;
	RGBA rgbaPrev = iter->second;
	iter++;
	while (iter != m_pos2rgba.end())
	{
		int posCur = iter->first;
		RGBA rgbaCur = iter->second;
		int nLenPart = posCur - posPrev + 1;

		for (int j=posPrev; j<=posCur; j++)
		{
			int index = j-m_nMinPos;

			pBuffer[index].red = rgbaPrev.red + (rgbaCur.red - rgbaPrev.red)*(j-posPrev)/nLenPart;
			pBuffer[index].green = rgbaPrev.green + (rgbaCur.green - rgbaPrev.green)*(j-posPrev)/nLenPart;
			pBuffer[index].blue = rgbaPrev.blue + (rgbaCur.blue - rgbaPrev.blue)*(j-posPrev)/nLenPart;

			pBuffer[index].alpha = rgbaPrev.alpha + (rgbaCur.alpha - rgbaPrev.alpha)*(j-posPrev)/nLenPart;
		}
		posPrev = iter->first;
		rgbaPrev = iter->second;
		iter++;
	}

	if (m_pos2alpha.size() > 1)
	{
		std::map<int, double>::iterator iterAlpha = m_pos2alpha.begin();
		while (iterAlpha != m_pos2alpha.end())
		{
			if (iterAlpha->first<m_nMinPos || iterAlpha->first>m_nMaxPos)
			{
				m_pos2alpha.erase(iterAlpha);
				iterAlpha = m_pos2alpha.begin();
				continue;
			}
			iterAlpha++;
		}

		if (m_pos2alpha.begin()->first != m_nMinPos)
		{
			m_pos2alpha[m_nMinPos] = 0;
		}
		if (m_pos2alpha.rbegin()->first != m_nMaxPos)
		{
			double& alpha = m_pos2alpha.rbegin()->second;
			m_pos2alpha[m_nMaxPos] = alpha;
		}

		iterAlpha = m_pos2alpha.begin();
		int posPrev = iterAlpha->first;
		double alphaPrev = iterAlpha->second;
		iterAlpha++;
		while (iterAlpha != m_pos2alpha.end())
		{
			int posCur = iterAlpha->first;
			double alphaCur = iterAlpha->second;
			int nLenPart = posCur - posPrev + 1;

			for (int j=posPrev; j<=posCur; j++)
			{
				int index = j-m_nMinPos;

				pBuffer[index].alpha = alphaPrev + (alphaCur- alphaPrev)*(j-posPrev)/nLenPart;
			}
			posPrev = iterAlpha->first;
			alphaPrev= iterAlpha->second;
			iterAlpha++;
		}
	}
	return true;
}
