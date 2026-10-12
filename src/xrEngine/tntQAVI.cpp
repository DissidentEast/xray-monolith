#include "stdafx.h"
#pragma hdrstop

//#include <crtdbg.h>

#include "tntQAVI.h"

CAviPlayerCustom::CAviPlayerCustom()
{
	alpha = nullptr;

	m_pMovieIndex = nullptr;
	m_pMovieData = nullptr;
	m_aviIC = nullptr;
	m_pDecompressedBuf = nullptr;

	ZeroMemory(&m_biOutFormat, sizeof(BITMAPINFOHEADER));
	ZeroMemory(&m_biInFormat, sizeof(BITMAPINFOHEADER));

	m_fRate = 0.0f;
	m_fCurrentRate = 0.0f;

	m_dwFrameTotal = 0;	   
	m_dwFrameCurrent = 0xfffffffd; // ÑÑ‚Ñ€Ð°Ñ…ÑƒÐµÐ¼ÑÑ Ð¾Ñ‚ 0xffffffff + 1 == 0
	m_dwFirstFrameOffset = 0;
	m_dwWidth = 0;
	m_dwHeight = 0;
}

CAviPlayerCustom::~CAviPlayerCustom()
{
	if (m_aviIC)
	{
		ICDecompressEnd(m_aviIC);
		ICClose(m_aviIC);
	}

	if (m_pDecompressedBuf) xr_free(m_pDecompressedBuf);

	if (m_pMovieData) xr_free(m_pMovieData);
	if (m_pMovieIndex) xr_free(m_pMovieIndex);

	xr_delete(alpha);
}

//---------------------------------
BOOL CAviPlayerCustom::Load(char* fname)
{
	// Check for alpha
	string_path aname;
	strconcat(sizeof(aname), aname, fname, "_alpha");
	if (FS.exist(aname))
	{
		alpha = xr_new<CAviPlayerCustom>();
		alpha->Load(aname);
	}

	// ÐžÑ‚ÐºÑ€Ñ‹Ñ‚ÑŒ Ñ‡ÐµÑ€ÐµÐ· mmioOpen( ) AVI Ñ„Ð°Ð¹Ð»
	HMMIO hmmioFile = mmioOpen(fname, nullptr, MMIO_READ /*MMIO_EXCLUSIVE*/);
	if (hmmioFile == nullptr)
	{
		return FALSE;
	}

	// ÐÐ°Ð¹Ñ‚Ð¸ Ñ‡Ð°Ð½Ðº FOURCC('movi')

	MMCKINFO mmckinfoParent;
	ZeroMemory(&mmckinfoParent, sizeof(mmckinfoParent));
	mmckinfoParent.fccType = mmioFOURCC('A', 'V', 'I', ' ');
	MMRESULT res;
	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, nullptr, MMIO_FINDRIFF)))
	{
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	ZeroMemory(&mmckinfoParent, sizeof(mmckinfoParent));
	mmckinfoParent.fccType = mmioFOURCC('h', 'd', 'r', 'l');
	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, nullptr, MMIO_FINDLIST)))
	{
		mmioClose(hmmioFile, 0);
		return FALSE;
	}
	//-------------------------------------------------------------------
	//++strl
	ZeroMemory(&mmckinfoParent, sizeof(mmckinfoParent));
	mmckinfoParent.fccType = mmioFOURCC('s', 't', 'r', 'l');
	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, nullptr, MMIO_FINDLIST)))
	{
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	//++strh
	ZeroMemory(&mmckinfoParent, sizeof(mmckinfoParent));
	mmckinfoParent.fccType = mmioFOURCC('s', 't', 'r', 'h');
	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, nullptr, MMIO_FINDCHUNK)))
	{
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	AVIStreamHeaderCustom strh;
	ZeroMemory(&strh, sizeof(strh));
	if (mmckinfoParent.cksize != (DWORD)mmioRead(hmmioFile, (HPSTR)&strh, mmckinfoParent.cksize))
	{
		mmioClose(hmmioFile, 0);
		return FALSE;
	}


	AVIFileInit();
	PAVIFILE aviFile = 0;
	if (AVIERR_OK != AVIFileOpen(&aviFile, fname, OF_READ, 0)) return FALSE;

	AVIFILEINFO aviInfo;
	ZeroMemory(&aviInfo, sizeof(aviInfo));
	if (AVIERR_OK != AVIFileInfo(aviFile, &aviInfo, sizeof(aviInfo)))
	{
		AVIFileRelease(aviFile);
		return FALSE;
	}

	m_dwFrameTotal = aviInfo.dwLength;
	m_fCurrentRate = (float)aviInfo.dwRate / (float)aviInfo.dwScale;

	m_dwWidth = aviInfo.dwWidth;
	m_dwHeight = aviInfo.dwHeight;

	AVIFileRelease(aviFile);

	R_ASSERT(m_dwWidth && m_dwHeight);

	m_pDecompressedBuf = (BYTE*)xr_malloc(m_dwWidth * m_dwHeight * 4 + 4);

	//++strf
	ZeroMemory(&mmckinfoParent, sizeof(mmckinfoParent));
	mmckinfoParent.fccType = mmioFOURCC('s', 't', 'r', 'f');
	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, nullptr, MMIO_FINDCHUNK)))
	{
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	// Ð¿Ð¾Ð»ÑƒÑ‡Ð°ÐµÐ¼ Ð²Ñ…Ð¾Ð´Ð½Ð¾Ð¹ Ñ„Ð¾Ñ€Ð¼Ð°Ñ‚ Ð´ÐµÐºÐ¾Ð¼Ð¿Ñ€ÐµÑÑÐ¾Ñ€Ð° Ð² BITMAPINFOHEADER
	if (mmckinfoParent.cksize != (DWORD)mmioRead(hmmioFile, (HPSTR)&m_biInFormat, mmckinfoParent.cksize))
	{
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	// ÑÐ¾Ð·Ð´Ð°ÐµÐ¼ Ð²Ñ‹Ñ…Ð¾Ð´Ð½Ð¾Ð¹ Ñ„Ð¾Ñ€Ð¼Ð°Ñ‚ Ð´ÐµÐºÐ¾Ð¼Ð¿Ñ€ÐµÑÑÐ¾Ñ€Ð° (xRGB)
	m_biOutFormat.biSize = sizeof(m_biOutFormat);
	m_biOutFormat.biBitCount = 32;
	m_biOutFormat.biCompression = BI_RGB;
	m_biOutFormat.biPlanes = 1;
	m_biOutFormat.biWidth = m_dwWidth;
	m_biOutFormat.biHeight = m_dwHeight;
	m_biOutFormat.biSizeImage = m_dwWidth * m_dwHeight * 4;

	// ÐÐ°Ð¹Ñ‚Ð¸ Ð¿Ð¾Ð´Ñ…Ð¾Ð´ÑÑ‰Ð¸Ð¹ Ð´ÐµÐºÐ¾Ð¼Ð¿Ñ€ÐµÑÑÐ¾Ñ€
	m_aviIC = ICLocate(ICTYPE_VIDEO, 0, &m_biInFormat, &m_biOutFormat,
	                   // ICMODE_DECOMPRESS
	                   ICMODE_FASTDECOMPRESS
	);
	if (m_aviIC == 0)
	{
		return FALSE;
	}

	// ÐŸÑ€Ð¾Ð¸Ð½Ð¸Ñ‚Ð¸Ñ‚ÑŒ Ð´ÐµÐºÐ¾Ð¼Ð¿Ñ€ÐµÑÑÐ¾Ñ€
	if (ICERR_OK != ICDecompressBegin(m_aviIC, &m_biInFormat, &m_biOutFormat))
	{
		return FALSE;
	}

	//--strf
	if (MMSYSERR_NOERROR != mmioAscend(hmmioFile, &mmckinfoParent, 0))
	{
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	//--strh
	if (MMSYSERR_NOERROR != mmioAscend(hmmioFile, &mmckinfoParent, 0))
	{
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	//--strl
	if (MMSYSERR_NOERROR != mmioAscend(hmmioFile, &mmckinfoParent, 0))
	{
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	//-------------------------------------------------------------------
	MMCKINFO mmckinfoSubchunk;
	ZeroMemory(&mmckinfoSubchunk, sizeof(mmckinfoSubchunk));
	mmckinfoSubchunk.fccType = mmioFOURCC('m', 'o', 'v', 'i');
	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoSubchunk, nullptr, MMIO_FINDLIST))
		|| mmckinfoSubchunk.cksize <= 4)
	{
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	mmioSeek(hmmioFile, mmckinfoSubchunk.dwDataOffset, SEEK_SET);

	// Ð’Ñ‹Ð´ÐµÐ»Ð¸Ñ‚ÑŒ Ð¿Ð°Ð¼ÑÑ‚ÑŒ Ð¿Ð¾Ð´ ÑÐ¶Ð°Ñ‚Ñ‹Ðµ Ð´Ð°Ð½Ð½Ñ‹Ðµ Ð²ÑÐµÐ³Ð¾ ÐºÐ»Ð¸Ð¿Ð°
	m_pMovieData = (BYTE*)xr_malloc(mmckinfoSubchunk.cksize);
	if (m_pMovieData == nullptr)
	{
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	if (mmckinfoSubchunk.cksize != (DWORD)mmioRead(hmmioFile, (HPSTR)m_pMovieData, mmckinfoSubchunk.cksize))
	{
		xr_free(m_pMovieData);
		m_pMovieData = nullptr;
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	if (MMSYSERR_NOERROR != mmioAscend(hmmioFile, &mmckinfoSubchunk, 0))
	{
		xr_free(m_pMovieData);
		m_pMovieData = nullptr;
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	// ÐÐ°Ð¹Ñ‚Ð¸ Ñ‡Ð°Ð½Ðº FOURCC('idx1')
	ZeroMemory(&mmckinfoSubchunk, sizeof(mmckinfoSubchunk));
	mmckinfoSubchunk.fccType = mmioFOURCC('i', 'd', 'x', '1');

	if (MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoSubchunk, nullptr, MMIO_FINDCHUNK))
		|| mmckinfoSubchunk.cksize <= 4)
	{
		xr_free(m_pMovieData);
		m_pMovieData = nullptr;
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	// Ð’Ñ‹Ð´ÐµÐ»Ð¸Ñ‚ÑŒ Ð¿Ð°Ð¼ÑÑ‚ÑŒ Ð¿Ð¾Ð´ Ð¸Ð½Ð´ÐµÐºÑ
	m_pMovieIndex = (AVIINDEXENTRY*)xr_malloc(mmckinfoSubchunk.cksize);
	if (m_pMovieIndex == nullptr)
	{
		xr_free(m_pMovieData);
		m_pMovieData = nullptr;
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	if (mmckinfoSubchunk.cksize != (DWORD)mmioRead(hmmioFile, (HPSTR)m_pMovieIndex, mmckinfoSubchunk.cksize))
	{
		xr_free(m_pMovieIndex);
		m_pMovieIndex = nullptr;
		xr_free(m_pMovieData);
		m_pMovieData = nullptr;
		mmioClose(hmmioFile, 0);
		return FALSE;
	}

	// Ð—Ð°ÐºÑ€Ñ‹Ñ‚ÑŒ AVI Ñ„Ð°Ð¹Ð» Ñ‡ÐµÑ€ÐµÐ· mmioClose( )
	mmioClose(hmmioFile, 0);

	if (alpha)
	{
		R_ASSERT(m_dwWidth == alpha->m_dwWidth);
		R_ASSERT(m_dwHeight == alpha->m_dwHeight);
	}

	//-----------------------------------------------------------------
	return TRUE;
}

BOOL CAviPlayerCustom::DecompressFrame(DWORD dwFrameNum)
{
	// Ð¿Ð¾Ð»ÑƒÑ‡Ð°ÐµÐ¼ ÑÐ»ÐµÐ¼ÐµÐ½Ñ‚ Ð¸Ð½Ð´ÐµÐºÑÐ°
	AVIINDEXENTRY* pCurrFrameIndex = &m_pMovieIndex[dwFrameNum];

	m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
	R_ASSERT(m_biInFormat.biSizeImage != 0);

	DWORD dwFlags;
	dwFlags = (pCurrFrameIndex->dwFlags & AVIIF_KEYFRAME) ? 0 : ICDECOMPRESS_NOTKEYFRAME;
	m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
	dwFlags |= (m_biInFormat.biSizeImage) ? 0 : ICDECOMPRESS_NULLFRAME;

	if (ICERR_OK != ICDecompress(m_aviIC, dwFlags, &m_biInFormat, (m_pMovieData + pCurrFrameIndex->dwChunkOffset + 8),
	                             &m_biOutFormat, m_pDecompressedBuf))
	{
		return FALSE;
	}

	if (alpha)
	{
		// update
		BYTE* alpha_buf;
		alpha->GetFrame(&alpha_buf);
		u32* dst = (u32*)m_pDecompressedBuf;
		u32* src = (u32*)alpha_buf;
		u32* end = dst + u32(m_dwWidth * m_dwHeight);
		for (; dst != end; src++, dst++)
		{
			u32& d = *dst;
			u32 s = *src;
			u32 a = (color_get_R(s) + color_get_G(s) + color_get_B(s)) / 3;
			d = subst_alpha(d, a);
		}
	}

	return TRUE;
}

/*
GetFrame

Ð²Ð¾Ð·Ð²Ñ€Ð°Ñ‰Ð°ÐµÑ‚ TRUE ÐµÑÐ»Ð¸ ÐºÐ°Ð´Ñ€ Ð¸Ð·Ð¼ÐµÐ½Ð¸Ð»ÑÑ, Ð¸Ð½Ð°Ñ‡Ðµ FALSE
*/
BOOL CAviPlayerCustom::GetFrame(BYTE** pDest)
{
	R_ASSERT(pDest);

	DWORD dwCurrFrame;
	dwCurrFrame = CalcFrame();

	//** debug dwCurrFrame = 112;

	// Ð•ÑÐ»Ð¸ Ð·Ð°Ð´Ð°Ð½Ð½Ñ‹Ð¹ ÐºÐ°Ð´Ñ€ Ñ€Ð°Ð²ÐµÐ½ Ð¿Ñ€ÐµÐ´Ð¸Ð´ÑƒÑ‰ÐµÐ¼Ñƒ
	if (dwCurrFrame == m_dwFrameCurrent)
	{
		*pDest = m_pDecompressedBuf;

		return FALSE;
	}
	else
		// Ð•ÑÐ»Ð¸ Ð·Ð°Ð´Ð°Ð½Ð½Ñ‹Ð¹ ÐºÐ°Ð´Ñ€ ÑÑ‚Ð¾ ÐŸÑ€ÐµÐ´Ð¸Ð´ÑƒÑ‰Ð¸Ð¹ ÐºÐ°Ð´Ñ€ + 1
		if (dwCurrFrame == m_dwFrameCurrent + 1)
		{
			++m_dwFrameCurrent; // dwCurrFrame == m_dwFrameCurrent + 1

			*pDest = m_pDecompressedBuf;

			DecompressFrame(m_dwFrameCurrent);
			return TRUE;
		}
		else
		{
			// Ð­Ñ‚Ð¾ Ð¿Ñ€Ð¾Ð¸Ð·Ð²Ð¾Ð»ÑŒÐ½Ñ‹Ð¹ ÐºÐ°Ð´Ñ€

			if (!(m_pMovieIndex[dwCurrFrame].dwFlags & AVIIF_KEYFRAME))
			{
				// Ð­Ñ‚Ð¾ ÐÐ• ÐºÐ»ÑŽÑ‡ÐµÐ²Ð¾Ð¹ ÐºÐ°Ð´Ñ€ -
				// Ð´ÐµÐ»Ð°ÐµÐ¼ PreRoll Ð¾Ñ‚ Ð±Ð»Ð¸Ð¶Ð°Ð¹ÑˆÐµÐ³Ð¾ Ð¿Ñ€ÐµÐ´Ð¸Ð´ÑƒÑ‰ÐµÐ³Ð¾ ÐºÐ»ÑŽÑ‡ÐµÐ²Ð¾Ð³Ð¾ ÐºÐ°Ð´Ñ€Ð° Ð´Ð¾ Ð—Ð°Ð´Ð°Ð½Ð½Ð¾Ð³Ð¾-1
				PreRoll(dwCurrFrame);
			}

			m_dwFrameCurrent = dwCurrFrame;

			*pDest = m_pDecompressedBuf;

			// Ð”ÐµÐºÐ¾Ð¼Ð¿Ñ€ÐµÑÑÐ¸Ð¼ Ð·Ð°Ð´Ð°Ð½Ð½Ñ‹Ð¹ ÐºÐ°Ð´Ñ€
			DecompressFrame(m_dwFrameCurrent);
			return TRUE;
		}
}

// Ð¼Ð¸Ð½Ð¸Ð¼ÑƒÐ¼ Ð¿Ñ€Ð¾Ð²ÐµÑ€Ð¾Ðº Ð½Ð° Ð²Ð°Ð»Ð¸Ð´Ð½Ð¾ÑÑ‚ÑŒ Ð¿ÐµÑ€ÐµÐ´Ð°Ð½Ð½Ð¾Ð³Ð¾ Ð´Ð»Ñ Ð¿Ñ€ÐµÑ€Ð¾Ð»Ð»Ð° ÐºÐ°Ð´Ñ€Ð° - Ð½ÑƒÐ¶Ð½Ð° ÑÐºÐ¾Ñ€Ð¾ÑÑ‚ÑŒ
VOID CAviPlayerCustom::PreRoll(DWORD dwFrameNum)
{
	int i;

	AVIINDEXENTRY* pCurrFrameIndex;
	DWORD res;

	// Ð½Ð°Ñ…Ð¾Ð´Ð¸Ð¼ Ð² Ð¼Ð°ÑÑÐ¸Ð²Ðµ Ð¸Ð½Ð´ÐµÐºÑÐ¾Ð² Ð¿ÐµÑ€Ð²Ñ‹Ð¹ Ð¿Ñ€ÐµÐ´ÑˆÐµÑÑ‚Ð²ÑƒÑŽÑ‰Ð¸Ð¹ ÐµÐ¼Ñƒ ÐºÐ»ÑŽÑ‡ÐµÐ²Ð¾Ð¹ ÐºÐ°Ð´Ñ€
	// Ð¸Ð»Ð¸ Ð±ÐµÑ€ÐµÐ¼ ÐºÐ°Ð´Ñ€, ÐºÐ¾Ñ€Ñ€ÐµÐºÑ‚Ð½Ð¾ Ñ€Ð°ÑÐ¶Ð°Ñ‚Ñ‹Ð¹ Ð´Ð¾ ÑÑ‚Ð¾Ð³Ð¾
	for (i = (int)dwFrameNum - 1; i > 0; i--)
	{
		if (m_pMovieIndex[i].dwFlags & AVIIF_KEYFRAME) break;

		if ((int)m_dwFrameCurrent == i)
		{
			// Ð½Ð°Ð¼ Ñ€Ð°Ð½ÑŒÑˆÐµ Ð²ÑÑ‚Ñ€ÐµÑ‚Ð¸Ð»ÑÑ Ñ€Ð°ÑÐ¶Ð°Ñ‚Ñ‹Ð¹ Ð¿ÐµÑ€ÐµÐ´ ÑÑ‚Ð¸Ð¼ ÐºÐ°Ð´Ñ€:
			// Ð´ÐµÐºÐ¾Ð¼Ð¿Ñ€ÐµÑÑÐ¸Ð¼ Ð²ÑÐµ Ð¿Ð¾ÑÐ»ÐµÐ´ÑƒÑŽÑ‰Ð¸Ðµ ÐÐ•ÐºÐ»ÑŽÑ‡ÐµÐ²Ñ‹Ðµ ÐºÐ°Ð´Ñ€Ñ‹ Ñ Ñ„Ð»Ð°Ð³Ð°Ð¼Ð¸ PREROLL & NOTKEYFRAME
			for (i++; i < (int)dwFrameNum; i++)
			{
				pCurrFrameIndex = &m_pMovieIndex[i];

				DWORD dwFlags;
				dwFlags = ICDECOMPRESS_PREROLL | ICDECOMPRESS_NOTKEYFRAME | ICDECOMPRESS_HURRYUP;
				m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
				dwFlags |= (m_biInFormat.biSizeImage) ? 0 : ICDECOMPRESS_NULLFRAME;

				res = ICDecompress(m_aviIC, dwFlags, &m_biInFormat,
				                   (m_pMovieData + pCurrFrameIndex->dwChunkOffset + 8) /*m_pCompressedBuf*/,
				                   &m_biOutFormat, m_pDecompressedBuf);
				if (ICERR_OK != res && ICERR_DONTDRAW != res)
				{
					// Ð¿Ñ€Ð¾Ð²ÐµÑ€ÐºÐ° Ð½Ð° ICERR_DONTDRAW Ð²Ð²ÐµÐ´ÐµÐ½Ð° Ð¸Ð·-Ð·Ð° indeo 5.11

					R_ASSERT(0);
				}
			} // for(...

			return;
		} // if( (int)m_dwFrameCurrent == i )...
	} // for( i=(int)dwFrameNum-1 ; i>0 ; i-- )...


	// Ð¿Ð¾Ð»ÑƒÑ‡Ð°ÐµÐ¼ ÑÐ»ÐµÐ¼ÐµÐ½Ñ‚ Ð¸Ð½Ð´ÐµÐºÑÐ°
	pCurrFrameIndex = &m_pMovieIndex[i];
	m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
	R_ASSERT(m_biInFormat.biSizeImage);

	// Ð´ÐµÐºÐ¾Ð¼Ð¿Ñ€ÐµÑÑÐ¸Ð¼ ÐºÐ»ÑŽÑ‡ÐµÐ²Ð¾Ð¹ ÐºÐ°Ð´Ñ€ Ñ Ñ„Ð»Ð°Ð³Ð¾Ð¼ ICDECOMPRESS_PREROLL
	res = ICDecompress(m_aviIC, ICDECOMPRESS_PREROLL | ICDECOMPRESS_HURRYUP, &m_biInFormat,
	                   m_pMovieData + pCurrFrameIndex->dwChunkOffset + 8/*m_pCompressedBuf*/, &m_biOutFormat,
	                   m_pDecompressedBuf);
	if (ICERR_OK != res && ICERR_DONTDRAW != res)
	{
		R_ASSERT(0);
	}

	// Ð´ÐµÐºÐ¾Ð¼Ð¿Ñ€ÐµÑÑÐ¸Ð¼ Ð²ÑÐµ Ð¿Ð¾ÑÐ»ÐµÐ´ÑƒÑŽÑ‰Ð¸Ðµ ÐÐ•ÐºÐ»ÑŽÑ‡ÐµÐ²Ñ‹Ðµ ÐºÐ°Ð´Ñ€Ñ‹ Ñ Ñ„Ð»Ð°Ð³Ð°Ð¼Ð¸ PREROLL & NOTKEYFRAME
	for (i++; i < (int)dwFrameNum; i++)
	{
		pCurrFrameIndex = &m_pMovieIndex[i];

		DWORD dwFlags;
		dwFlags = ICDECOMPRESS_PREROLL | ICDECOMPRESS_NOTKEYFRAME | ICDECOMPRESS_HURRYUP;
		m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
		dwFlags |= (m_biInFormat.biSizeImage) ? 0 : ICDECOMPRESS_NULLFRAME;

		res = ICDecompress(m_aviIC, dwFlags, &m_biInFormat,
		                   (m_pMovieData + pCurrFrameIndex->dwChunkOffset + 8) /*m_pCompressedBuf*/, &m_biOutFormat,
		                   m_pDecompressedBuf);
		if (ICERR_OK != res && ICERR_DONTDRAW != res)
		{
			// Ð¿Ñ€Ð¾Ð²ÐµÑ€ÐºÐ° Ð½Ð° ICERR_DONTDRAW Ð²Ð²ÐµÐ´ÐµÐ½Ð° Ð¸Ð·-Ð·Ð° indeo 5.11

			R_ASSERT(0);
		}
	} // for(...
}

VOID CAviPlayerCustom::GetSize(DWORD* dwWidth, DWORD* dwHeight)
{
	if (dwWidth) *dwWidth = m_dwWidth;
	if (dwHeight) *dwHeight = m_dwHeight;
}

INT CAviPlayerCustom::SetSpeed(INT nPercent)
{
	INT res = INT(m_fCurrentRate / m_fRate * 100);

	m_fCurrentRate = m_fRate * FLOAT(nPercent / 100.0f);

	return res;
}

DWORD CAviPlayerCustom::CalcFrame()
{
	if (0 == m_dwFirstFrameOffset)
		m_dwFirstFrameOffset = RDEVICE.dwTimeContinual - 1;

	return DWORD(floor((RDEVICE.dwTimeContinual - m_dwFirstFrameOffset) * m_fCurrentRate / 1000.0f)) % m_dwFrameTotal;
}
