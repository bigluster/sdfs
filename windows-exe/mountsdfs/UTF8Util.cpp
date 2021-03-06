//////////////////////////////////////////////////////////////////////////////
//
// *** Routines to convert between Unicode UTF-8 and Unicode UTF-16 ***
//
// By Giovanni Dicanio <giovanni.dicanio AT gmail.com>
//
// Last update: 2010, January 2nd
//
//
// These routines use ::MultiByteToWideChar and ::WideCharToMultiByte
// Win32 API functions to convert between Unicode UTF-8 and UTF-16.
//
// UTF-16 strings are stored in instances of CStringW.
// UTF-8 strings are stored in instances of CStringA.
//
// On error, the conversion routines use AtlThrow to signal the
// error condition.
//
// If input string pointers are NULL, empty strings are returned.
//
//
// Prefixes used in these routines:
// 覧覧覧覧覧�
//
//  � cch  : count of characters (CHAR痴 or WCHAR痴)
//  � cb   : count of bytes
//  � psz  : pointer to a NUL-terminated string (CHAR* or WCHAR*)
//  � str  : instance of CString(A/W) class
//
//
//
// Useful Web References:
// 覧覧覧�-
//
// WideCharToMultiByte Function
// http://msdn.microsoft.com/en-us/library/dd374130.aspx
//
// MultiByteToWideChar Function
// http://msdn.microsoft.com/en-us/library/dd319072.aspx
//
// AtlThrow
// http://msdn.microsoft.com/en-us/library/z325eyx0.aspx
//
//
// Developed on VC9 (Visual Studio 2008 SP1)
//
//
//////////////////////////////////////////////////////////////////////////////



namespace UTF8Util
{



	//覧覧覧覧覧覧覧覧覧覧覧覧�-
	// FUNCTION: ConvertUTF8ToUTF16
	// DESC: Converts Unicode UTF-8 text to Unicode UTF-16 (Windows default).
	//覧覧覧覧覧覧覧覧覧覧覧覧�-
	CStringW ConvertUTF8ToUTF16(__in const CHAR * pszTextUTF8)
	{
		//
		// Special case of NULL or empty input string
		//
		if ((pszTextUTF8 == NULL) || (*pszTextUTF8 == �))
		{
			// Return empty string
			return L灯;
		}


		//
		// Consider CHAR痴 count corresponding to total input string length,
		// including end-of-string () character
		//
		const size_t cchUTF8Max = INT_MAX � 1;
		size_t cchUTF8;
		HRESULT hr = ::StringCchLengthA(pszTextUTF8, cchUTF8Max, &cchUTF8);
		if (FAILED(hr))
		{
			AtlThrow(hr);
		}

		// Consider also terminating
		++cchUTF8;

		// Convert to 訴nt� for use with MultiByteToWideChar API
		int cbUTF8 = static_cast<int>(cchUTF8);


		//
		// Get size of destination UTF-16 buffer, in WCHAR痴
		//
		int cchUTF16 = ::MultiByteToWideChar(
			CP_UTF8,                // convert from UTF-8
			MB_ERR_INVALID_CHARS,   // error on invalid chars
			pszTextUTF8,            // source UTF-8 string
			cbUTF8,                 // total length of source UTF-8 string,
			// in CHAR痴 (= bytes), including end-of-string
			NULL,                   // unused � no conversion done in this step
			0                       // request size of destination buffer, in WCHAR痴
			);
		ATLASSERT(cchUTF16 != 0);
		if (cchUTF16 == 0)
		{
			AtlThrowLastWin32();
		}


		//
		// Allocate destination buffer to store UTF-16 string
		//
		CStringW strUTF16;
		WCHAR * pszUTF16 = strUTF16.GetBuffer(cchUTF16);

		//
		// Do the conversion from UTF-8 to UTF-16
		//
		int result = ::MultiByteToWideChar(
			CP_UTF8,                // convert from UTF-8
			MB_ERR_INVALID_CHARS,   // error on invalid chars
			pszTextUTF8,            // source UTF-8 string
			cbUTF8,                 // total length of source UTF-8 string,
			// in CHAR痴 (= bytes), including end-of-string
			pszUTF16,               // destination buffer
			cchUTF16                // size of destination buffer, in WCHAR痴
			);
		ATLASSERT(result != 0);
		if (result == 0)
		{
			AtlThrowLastWin32();
		}

		// Release internal CString buffer
		strUTF16.ReleaseBuffer();

		// Return resulting UTF16 string
		return strUTF16;
	}



	//覧覧覧覧覧覧覧覧覧覧覧覧�-
	// FUNCTION: ConvertUTF16ToUTF8
	// DESC: Converts Unicode UTF-16 (Windows default) text to Unicode UTF-8.
	//覧覧覧覧覧覧覧覧覧覧覧覧�-
	CStringA ConvertUTF16ToUTF8(__in const WCHAR * pszTextUTF16)
	{
		//
		// Special case of NULL or empty input string
		//
		if ((pszTextUTF16 == NULL) || (*pszTextUTF16 == L�))
		{
			// Return empty string
			return 灯;
		}


		//
		// Consider WCHAR痴 count corresponding to total input string length,
		// including end-of-string (L�) character.
		//
		const size_t cchUTF16Max = INT_MAX � 1;
		size_t cchUTF16;
		HRESULT hr = ::StringCchLengthW(pszTextUTF16, cchUTF16Max, &cchUTF16);
		if (FAILED(hr))
		{
			AtlThrow(hr);
		}

		// Consider also terminating
		++cchUTF16;


		//
		// WC_ERR_INVALID_CHARS flag is set to fail if invalid input character
		// is encountered.
		// This flag is supported on Windows Vista and later.
		// Don稚 use it on Windows XP and previous.
		//
#if (WINVER >= 0x0600)
		DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
#else
		DWORD dwConversionFlags = 0;
#endif

		//
		// Get size of destination UTF-8 buffer, in CHAR痴 (= bytes)
		//
		int cbUTF8 = ::WideCharToMultiByte(
			CP_UTF8,                // convert to UTF-8
			dwConversionFlags,      // specify conversion behavior
			pszTextUTF16,           // source UTF-16 string
			static_cast<int>(cchUTF16),   // total source string length, in WCHAR痴,
			// including end-of-string
			NULL,                   // unused � no conversion required in this step
			0,                      // request buffer size
			NULL, NULL              // unused
			);
		ATLASSERT(cbUTF8 != 0);
		if (cbUTF8 == 0)
		{
			AtlThrowLastWin32();
		}


		//
		// Allocate destination buffer for UTF-8 string
		//
		CStringA strUTF8;
		int cchUTF8 = cbUTF8; // sizeof(CHAR) = 1 byte
		CHAR * pszUTF8 = strUTF8.GetBuffer(cchUTF8);


		//
		// Do the conversion from UTF-16 to UTF-8
		//
		int result = ::WideCharToMultiByte(
			CP_UTF8,                // convert to UTF-8
			dwConversionFlags,      // specify conversion behavior
			pszTextUTF16,           // source UTF-16 string
			static_cast<int>(cchUTF16),   // total source string length, in WCHAR痴,
			// including end-of-string
			pszUTF8,                // destination buffer
			cbUTF8,                 // destination buffer size, in bytes
			NULL, NULL              // unused
			);
		ATLASSERT(result != 0);
		if (result == 0)
		{
			AtlThrowLastWin32();
		}

		// Release internal CString buffer
		strUTF8.ReleaseBuffer();

		// Return resulting UTF-8 string
		return strUTF8;
	}



} // namespace UTF8Util


//////////////////////////////////////////////////////////////////////////////