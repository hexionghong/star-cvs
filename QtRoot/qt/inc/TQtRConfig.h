// Author: Valeri Fine   28/06/2004
/****************************************************************************
** $Id: TQtRConfig.h,v 1.1 2006/08/16 19:29:07 fine Exp $
**
** Copyright (C) 2004 by Valeri Fine.  All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
*****************************************************************************/

#ifndef ROOT_TQTRCONFIG
#define ROOT_TQTRCONFIG
#include "RConfig.h"

// defined the CPP symbols to create the different versions
#ifdef R__QTX11
#undef R__QTX11
#endif /* R__QTX11 */

#ifdef   R__QTWIN32
#undef   R__QTWIN32
#endif /*R__QTWIN32*/

#ifdef   R__QTMACOS
#undef   R__QTMACOS
#endif /*R__QTMACOS*/

#ifdef  R__QTGUITHREAD
#undef  R__QTGUITHREAD
#endif /*R__QTGUITHREAD*/

#if defined(R__UNIX) && !defined(R__MACOSX)
# define R__QTX11
#endif

#if defined(R__WIN32)
# define R__QTWIN32
#endif

#if defined(R__MACOSX)
# define R__QTMACOS
#endif

#ifndef ROOT_VALID_QT_VERSION
// The least Qt version the ROOT package can work with
#define ROOT_VALID_QT_VERSION "3.2.0"
#endif

#endif /*ROOT_TQTCONFIG*/
