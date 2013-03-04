/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 $Id$
 
 Author(s):  Balazs Vagvolgyi
 Created on: 2010
 
 (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
 Reserved.
 
 --- begin cisst license - do not edit ---
 
 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.
 
 --- end cisst license ---
 
 */

#ifndef _svlVidCapSrcInitializer_h
#define _svlVidCapSrcInitializer_h

#include "svlInitializerMacros.h"

// Includes for handling device specific API
// interactions that cannot otherwise be abstracted

#if CISST_SVL_HAS_DIRECTSHOW
#include "svlVidCapSrcDirectShow.h"
#endif // CISST_SVL_HAS_DIRECTSHOW

#if CISST_SVL_HAS_MIL
#include "svlVidCapSrcMIL.h"
//#endif // CISST_SVL_HAS_MIL

//TODO::CMAKE flag for SDI
#else //#if CISST_HAS_OPENGL
#include <cisstStereoVision/svlVidCapSrcSDI.h>
#endif//CISST_HAS_OPENGL


void svlInitializeVideoCapture();


#endif // _svlVidCapSrcInitializer_h

