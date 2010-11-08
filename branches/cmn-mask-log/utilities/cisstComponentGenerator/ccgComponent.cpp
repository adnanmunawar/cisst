/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: Component.cpp 1030 2010-06-11 jkriss1 $ */

#include <string>
#include <ccgComponent.h>

#include <iostream>
#include <fstream>


ccgComponent::ccgComponent(void)
{
}


std::string ccgComponent::WriteStateTableData(void)
{
    std::string s = "StateTable.AddData(";
    ccgStateTableData data = StateTable.Data.front();
    StateTable.Data.pop_front();

    s += data.Name + ", \"" + data.Name + "\");";

    return s;
}


std::string ccgComponent::WriteInterfaceProvided(void)
{
    ccgInterfaceProvided interf = InterfacesProvided.front();
    std::string n = interf.Name.substr(1, interf.Name.length()-2);

    std::string s = "    interfaceProvided = AddInterfaceProvided(" + interf.Name + ");\n";

    size_t size = interf.Commands.size();
    for (size_t i = 0; i < size; i++) {
        ccgCommand command = interf.Commands.front();

        s += "    interfaceProvided->AddCommand";
        if (command.Type == "Void" || command.Type == "Write" || command.Type == "QualifiedRead") {
            s += command.Type + "(&" + Name + "::" + command.Func + ", this, " + command.Name;

            if (command.Type == "Write" && command.Arg1 != "void") {
                s += ", " + command.Arg1;
            }

            s += ");\n";
        } else if (command.Type == "Read") {
            if (command.Arg2 == "void")
                s += command.Type + "(&" + Name + "::" + command.Func + ", " + command.Arg1 + ", " + command.Name + ");\n";
            else
                s += command.Type + "(&" + command.Arg2 + "::" + command.Func + ", " + command.Arg1 + ", " + command.Name + ");\n";
        } else if (command.Type == "ReadState")
            s += command.Type + "(StateTable, " + command.Func + ", " + command.Name + ");\n";

        interf.Commands.pop_front();
    }

    size = interf.Events.size();
    for (size_t i = 0; i < size; i++) {
        ccgEvent e = interf.Events.front();

        s += "    interfaceProvided->AddEvent";
        s += e.Type + "(" + e.Function + ", " + e.Name;
        if (e.Type == "Write")
            s += ", " + e.Arg1 + ");\n";
        else
            s += ");\n";

        interf.Events.pop_front();
    }

    InterfacesProvided.pop_front();
    return s;
}


std::string ccgComponent::WriteInterfaceRequired(void)
{
    ccgInterfaceRequired interf = InterfacesRequired.front();
    std::string n = interf.Name.substr(1, interf.Name.length() - 2);

    std::string s = "    interfaceRequired = AddInterfaceRequired(" + interf.Name + ");\n";

    size_t size = interf.Functions.size();
    for (size_t i = 0; i < size; i++) {
        ccgFunction function = interf.Functions.front();
        s += "    interfaceRequired->AddFunction";
        s += "(" + function.Name + ", " + interf.Type + "." + function.Type + ");\n";
        interf.Functions.pop_front();
    }

    size = interf.Handlers.size();
    for (size_t i = 0; i < size; i++) {
        ccgEventHandler handler = interf.Handlers.front();
        s += "    interfaceRequired->AddEventHandler" + handler.Type;
        s += "(&" + Name + "::" + handler.Function + ", this, " + handler.Name;
        if (handler.Arg != "void")
            s += ", " + handler.Arg;
        s += ");\n";
        interf.Handlers.pop_front();
    }

    InterfacesRequired.pop_front();
    return s;
}


void ccgComponent::GenerateCode(std::string filename)
{
    std::ofstream file;

    std::cout << filename << std::endl;

    file.open(filename.c_str());

    file << "/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */" << std::endl;
    file << "/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */" << std::endl;
    file << "/* File " << GetName() << " generated by cisstComponentGenerator, do not change */\n" << std::endl;

    file << "#include <" << GetName() << ".h>\n" << std::endl;

    file << "void " << GetName() << "::InitComponent(void) {\n" << std::endl;
    size_t size = GetStateTableSize();
    if (size > 0)
        file << "    // state table variables" << std::endl;
    for (size_t i = 0; i < size; i++)
        file << "    " << WriteStateTableData() << std::endl;

    size = GetNumberOfInterfacesProvided();

    if (size > 0) {
        file << "    // provided interfaces" << std::endl
             << "    mtsInterfaceProvided * interfaceProvided;" << std::endl;
    }

    for (size_t i = 0; i < size; i++)
        file << WriteInterfaceProvided() << std::endl;

    size = GetNumberOfInterfacesRequired();

    if (size > 0) {
        file << "    // required interfaces" << std::endl
             << "    mtsInterfaceRequired * interfaceRequired;" << std::endl;
    }

    for (size_t i = 0; i < size; i++)
        file << WriteInterfaceRequired() << std::endl;

    file << "\n};" << std::endl;

    file.close();
}
