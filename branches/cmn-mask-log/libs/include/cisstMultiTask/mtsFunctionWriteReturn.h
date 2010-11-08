/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFunctionVoidReturn.h 1859 2010-10-08 16:17:25Z adeguet1 $

  Author(s): Anton Deguet
  Created on: 2010-09-16

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/



/*!
  \file
  \brief Defines a function object to use a void command (mtsCommandWriteReturn)
*/

#ifndef _mtsFunctionWriteReturn_h
#define _mtsFunctionWriteReturn_h

#include <cisstMultiTask/mtsFunctionBase.h>
#include <cisstMultiTask/mtsCommandWriteReturn.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsFunctionWriteReturn: public mtsFunctionBase {
 protected:
    typedef mtsCommandWriteReturn CommandType;
    CommandType * Command;

    template <typename __argumentType, typename __resultType, bool a, bool b>
    class ConditionalWrap {
    public:
        static mtsExecutionResult Call(mtsCommandWriteReturn * command,
                                       const __argumentType & argument, __resultType & result) {
            const mtsGenericObjectProxyRef<__argumentType> argumentWrapped(argument);
            mtsGenericObjectProxyRef<__resultType> resultWrapped(result);
            return command->Execute(argumentWrapped, resultWrapped);
        }
    };

    template <typename __argumentType, typename __resultType>
    class ConditionalWrap<__argumentType, __resultType, false, true> {
    public:
        static mtsExecutionResult Call(mtsCommandWriteReturn * command,
                                       const __argumentType & argument, __resultType & result) {
            const mtsGenericObjectProxyRef<__argumentType> argumentWrapped(argument);
            return command->Execute(argumentWrapped, result);
        }
    };

    template <typename __argumentType, typename __resultType>
    class ConditionalWrap<__argumentType, __resultType, true, false> {
    public:
        static mtsExecutionResult Call(mtsCommandWriteReturn * command,
                                       const __argumentType & argument, __resultType & result) {
            mtsGenericObjectProxyRef<__resultType> resultWrapped(result);
            return command->Execute(argument, resultWrapped);
        }
    };

    template <typename __argumentType, typename __resultType>
    class ConditionalWrap<__argumentType, __resultType, true, true> {
    public:
        static mtsExecutionResult Call(mtsCommandWriteReturn * command,
                                       const __argumentType & argument, __resultType & result) {
            return command->Execute(argument, result);
        }
    };

 public:
    /*! Default constructor.  Does nothing, use Instantiate before
      using. */
    mtsFunctionWriteReturn(void): Command(0) {}

    /*! Destructor. */
    ~mtsFunctionWriteReturn();

    // documented in base class
    bool Detach(void);

    // documented in base class
    bool IsValid(void) const;

    /*! Bind using a command pointer.  This allows to avoid
      querying by name from an interface.
      \param command Pointer on an existing command
      \result Boolean value, true if the command pointer is not null.
    */
    bool Bind(CommandType * command);

    /*! Overloaded operator to enable more intuitive syntax
      e.g., Command() instead of Command->Execute(). */
    mtsExecutionResult operator()(const mtsGenericObject & argument,
                                  mtsGenericObject & result) const;

	/*! Overloaded operator that accepts different argument types (for write return). */
    template <class __argumentType, class __resultType>
    mtsExecutionResult operator()(const __argumentType & argument, __resultType & result) const {
        mtsExecutionResult executionResult = Command ?
            ConditionalWrap<__argumentType, __resultType,
                            cmnIsDerivedFrom<__argumentType, mtsGenericObject>::YES,
                            cmnIsDerivedFrom<__resultType, mtsGenericObject>::YES>
                           ::Call(Command, argument, result)
          : mtsExecutionResult::NO_INTERFACE;
        return executionResult;
    }

    /*! Access to underlying command object. */
    CommandType * GetCommand(void) const;

    /*! Access to the command argument prototype. */
    const mtsGenericObject * GetArgumentPrototype(void) const;

    /*! Access to the command result prototype. */
    const mtsGenericObject * GetResultPrototype(void) const;

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;
};


#endif // _mtsFunctionWriteReturn_h

