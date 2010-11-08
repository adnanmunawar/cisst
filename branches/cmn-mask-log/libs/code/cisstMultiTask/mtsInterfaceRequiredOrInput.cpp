/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2008-11-13

  (C) Copyright 2008-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsInterfaceRequiredOrInput.h>


mtsInterfaceRequiredOrInput::mtsInterfaceRequiredOrInput(const std::string & interfaceName,
                                                         mtsRequiredType required):
    Name(interfaceName),
    InterfaceProvidedOrOutput(0),
    Required(required)
{
}


mtsInterfaceRequiredOrInput::~mtsInterfaceRequiredOrInput()
{
}


const std::string & mtsInterfaceRequiredOrInput::GetName(void) const
{
    return this->Name;
}


const mtsInterfaceProvidedOrOutput * mtsInterfaceRequiredOrInput::GetConnectedInterface(void) const {
    return this->InterfaceProvidedOrOutput;
}


mtsRequiredType mtsInterfaceRequiredOrInput::IsRequired(void) const
{
    return this->Required;
}
