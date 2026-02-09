#include "mlsdialog.h"

namespace MLSUTIL
{

static MlsDialog*	g_pDialog = NULL;
static MlsProgress*	g_pProgress = NULL;

void	SetDialogProgress( MlsDialog* pDialog, MlsProgress* pProgress )
{
	LOG("SetDialogProgress Dialog [%p] Progress [%p]",
				pDialog, pProgress );
	LOG("SetDialogProgress g_pDialog [%p] g_pProgress [%p]",
				g_pDialog, g_pProgress );
	g_pDialog = pDialog;
	g_pProgress= pProgress;
}

///	@brief	 message box.
void	MsgBox(const string& sTitle, const string& sMsg)
{
	if ( g_pDialog )
		g_pDialog->MsgBox(sTitle, sMsg);
}

///	@brief	 yes, no message box.
bool	YNBox(const string& sTitle, const string& sMsg, bool bYes )
{
	if ( g_pDialog )
		return g_pDialog->YNBox(sTitle, sMsg, bYes);
	else
		return false;
}

///	@brief	 start of message box. general use from a lot of wait job.
void*	MsgWaitBox(const string& sTitle, const string& sMsg)
{
	if ( !g_pDialog ) return NULL;
	return g_pDialog->MsgWaitBox(sTitle, sMsg);
}

///	@brief	 end of message box. general use from a lot of wait job.
void	MsgWaitEnd(void* p)
{
	if ( g_pDialog )
		g_pDialog->MsgWaitEnd(p);
}

///	@brief	 Input Box
/// @param	sTitle		Title
/// @param	sInputStr	inputed string
/// @param	bPasswd		password type. (English only)
/// @return SUCCESS
int		InputBox(const string& sTitle, string& sInputStr, bool bPasswd )
{
	if ( !g_pDialog ) return ERROR;
	return g_pDialog->InputBox(sTitle, sInputStr, bPasswd);
}

///	@brief	 Select Box.
/// @param	sTitle		Title
/// @param	vMsgStr		Select buttons on screen.
/// @param	n			Select number on screen.
/// @return	Selected number
int 	SelectBox(	const string& sTitle,
					vector<string>& vMsgStr,
					int n)
{
	if ( !g_pDialog ) return ERROR;
	return g_pDialog->SelectBox(sTitle, vMsgStr, n);
}

///	@brief	 key read
int 	GetChar(bool bNoDelay )
{
	if ( !g_pDialog ) return ERROR;
	return g_pDialog->GetChar(bNoDelay);
}

int		SetKeyBreakUse( bool bBreak )
{
	if ( !g_pDialog ) return ERROR;
	return g_pDialog->SetKeyBreakUse( bBreak );
}

/// @brief	text box
int		TextListBox( const string& sTitle, vector<string>& vTextStr, bool bCurShow, int width )
{
	if ( !g_pDialog ) return ERROR;
	return g_pDialog->TextBox( sTitle, vTextStr, bCurShow, width);
}

CommonProgress::CommonProgress(const string& sTitle, const string& sMsg, bool bDouble)
{
	_sTitle = sTitle; _sMsg = sMsg; _bDouble = bDouble;
	_bStarted = false;
	_pProgress = g_pProgress;
}

CommonProgress::~CommonProgress()
{
	if (_bStarted && _pProgress) _pProgress->End();
	_bStarted = false;
}

void CommonProgress::SetProgress( MlsProgress* p) 
{
	_pProgress = p;
}

void CommonProgress::setLeftStr(const string& p)
{ 
	if (_pProgress)
		_pProgress->setLeftStr( p ); 
}

void CommonProgress::setRightStr(const string& p)
{
	if (_pProgress)
		_pProgress->setRightStr( p ); 
}

void CommonProgress::setLeftStr2(const string& p)
{
	if (_pProgress)
		_pProgress->setLeftStr2( p ); 
}

void CommonProgress::setRightStr2(const string& p)
{
	if (_pProgress)
		_pProgress->setRightStr2( p ); 
}

bool CommonProgress::isExit()
{
	if (!_pProgress)
		return false;
	return _pProgress->isExit();
}

void CommonProgress::show()
{
	if (!_pProgress) return;
	_pProgress->_sTitle = _sTitle;
	_pProgress->_sMsg = _sMsg;
	_pProgress->_bDouble = _bDouble;
	_pProgress->Show();
}

void CommonProgress::redraw()
{
	if (!_pProgress) return;
	_pProgress->_sTitle = _sTitle;
	_pProgress->_sMsg = _sMsg;
	_pProgress->_bDouble = _bDouble;
	_pProgress->Redraw();
}

void CommonProgress::setCount(int nCnt, int nCnt2)
{
	if (!_pProgress) return;
	_pProgress->setCount(nCnt, nCnt2);
}

void CommonProgress::Start(void* pArg)
{
	try
	{
		if (!_pProgress) return;
		_bStarted = true;
		_pProgress->_bStarted = true;
		show();
		LOG("CommonProgress::Start");
		_pProgress->Start(pArg, JOIN);
	}
	catch(Exception& ex)
	{
		LOG("CommonProgress::Start exception: %s", ex.GetInfo());
	}
}

void CommonProgress::End()
{
	try
	{
		if (!_pProgress) return;
		_pProgress->_bStarted = false;
		//_pProgress->Cancel(); // error occurred.
		_pProgress->End();
		LOG("CommonProgress End ...");
	}
	catch(Exception& ex)
	{
		LOG("CommonProgress::End exception: %s", ex.GetInfo());
	}
}

};
