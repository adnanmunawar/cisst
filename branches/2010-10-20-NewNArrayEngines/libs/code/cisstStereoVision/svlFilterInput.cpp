/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

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

#include <cisstStereoVision/svlFilterInput.h>
#include <cisstStereoVision/svlFilterBase.h>
#include <cisstStereoVision/svlStreamManager.h>
#include <cisstStereoVision/svlStreamBranchSource.h>


svlFilterInput::svlFilterInput(svlFilterBase* filter, bool trunk, const std::string &name) :
    BaseType(name),
    Filter(filter),
    Trunk(trunk),
    Connected(false),
    Connection(0),
    ConnectedFilter(0),
    Type(svlTypeInvalid),
    Buffer(0),
    Timestamp(-1.0)
{
}

svlFilterInput::~svlFilterInput()
{
    if (Buffer) delete Buffer;
}

bool svlFilterInput::IsTrunk(void) const
{
    return Trunk;
}

svlStreamType svlFilterInput::GetType(void) const
{
    return Type;
}

svlFilterBase* svlFilterInput::GetFilter(void)
{
    return Filter;
}

svlFilterBase* svlFilterInput::GetConnectedFilter(void)
{
    return ConnectedFilter;
}

int svlFilterInput::AddType(svlStreamType type)
{
    if (!Filter || Filter->IsInitialized()) return SVL_FAIL;

    unsigned int size = SupportedTypes.size();
    SupportedTypes.resize(size + 1);
    SupportedTypes[size] = type;
    return SVL_OK;
}

bool svlFilterInput::IsConnected(void) const
{
    return Connected;
}

bool svlFilterInput::IsTypeSupported(svlStreamType type)
{
    const unsigned int size = SupportedTypes.size();
    for (unsigned int i = 0; i < size; i ++) {
        if (SupportedTypes[i] == type) return true;
    }
    return false;
}

svlFilterOutput* svlFilterInput::GetConnection(void)
{
    return Connection;
}

int svlFilterInput::DisconnectInternal(void)
{
    // TO DO
    return SVL_FAIL;
}

int svlFilterInput::PushSample(const svlSample* sample)
{
    if (!sample || !Filter || Trunk || Connected) return SVL_FAIL;

    svlStreamType type = sample->GetType();
    if (Type != svlTypeInvalid && Type != type) return SVL_FAIL;

    if (Filter->AutoType) {
        // Automatic setup
        if (!IsTypeSupported(type)) return SVL_FAIL;
        Type = type;
    }
    else {
        // Manual setup
        Type = type;
        if (Filter->UpdateTypes(*this, Type) != SVL_OK) return SVL_FAIL;
    }

    if (!Buffer) Buffer = new svlBufferSample(Type);

    // Store timestamp
    Timestamp = sample->GetTimestamp();

    return Buffer->Push(sample);
}

svlSample* svlFilterInput::PullSample(bool waitfornew, double timeout)
{
    if (!Filter || !Filter->IsInitialized() || !Buffer) return 0;
    return Buffer->Pull(waitfornew, timeout);
}

double svlFilterInput::GetTimestamp(void)
{
    return Timestamp;
}