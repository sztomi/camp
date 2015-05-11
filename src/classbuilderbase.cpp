/****************************************************************************
**
** This file is part of the CAMP library.
**
** The MIT License (MIT)
**
** Copyright (C) 2009-2014 TEGESO/TEGESOFT and/or its subsidiary(-ies) and mother company.
** Contact: Tegesoft Information (contact@tegesoft.com)
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
** 
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
** 
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
**
****************************************************************************/


#include <camp/classbuilderbase.hpp>
#include <camp/property.hpp>
#include <camp/function.hpp>
#include <camp/class.hpp>


namespace camp
{
//-------------------------------------------------------------------------------------------------
ClassBuilderBase::ClassBuilderBase(Class& target)
    : m_target(&target)
    , m_currentTagHolder(m_target)
    , m_currentProperty(nullptr)
    , m_currentFunction(nullptr)
{
}

//-------------------------------------------------------------------------------------------------
Class& ClassBuilderBase::getClass()
{
    return *m_target;
}

//-------------------------------------------------------------------------------------------------
void ClassBuilderBase::addBase(const Class& baseClass, int offset)
{
    #ifdef _DEBUG
    {
        // First make sure that the base class is not already a base of the current class
        const uint32_t baseId = baseClass.id();
        const Class::BaseVector& bases = m_target->m_bases;
        const size_t numberOfBases = bases.size();
        for (size_t i = 0; i < numberOfBases; ++i)
        {
            assert(bases[i].base->id() != baseId);
        }
    }
    #endif

    // Add the base metaclass to the bases of the current class
    Class::BaseInfo baseInfos;
    baseInfos.base = &baseClass;
    baseInfos.offset = offset;
    m_target->m_bases.push_back(baseInfos);

    { // Copy all properties of the base class into the current class
        Class::SortedPropertyVector& targetProperties = m_target->m_properties;
        const Class::SortedPropertyVector& baseProperties = baseClass.m_properties;
        const size_t numberOfProperties = baseProperties.size();
        for (size_t i = 0; i < numberOfProperties; ++i)
        {
            const Class::PropertyPtr& propertyPtr = baseProperties[i];
            Class::SortedPropertyVector::const_iterator iterator = std::lower_bound(targetProperties.cbegin(), targetProperties.cend(), propertyPtr->id(), Class::OrderByPropertyId());
            targetProperties.insert(iterator, propertyPtr);
        }
    }

    { // Copy all functions of the base class into the current class
        Class::SortedFunctionVector& targetFunctions = m_target->m_functions;
        const Class::SortedFunctionVector& baseFunctions = baseClass.m_functions;
        const size_t numberOfFunctions = baseFunctions.size();
        for (size_t i = 0; i < numberOfFunctions; ++i)
        {
            const Class::FunctionPtr& functionPtr = baseFunctions[i];
            Class::SortedFunctionVector::const_iterator iterator = std::lower_bound(targetFunctions.cbegin(), targetFunctions.cend(), functionPtr->id(), Class::OrderByFunctionId());
            targetFunctions.insert(iterator, functionPtr);
        }
    }
}

//-------------------------------------------------------------------------------------------------
void ClassBuilderBase::addProperty(Property* property)
{
    // Retrieve the class' properties sorted by ID
    Class::SortedPropertyVector& properties = m_target->m_properties;

    // Replace any property that already exists with the same ID
    const uint32_t id = property->id();
    Class::SortedPropertyVector::const_iterator iterator = std::lower_bound(properties.cbegin(), properties.cend(), id, Class::OrderByPropertyId());
    if (iterator != properties.end() && (*iterator._Ptr)->id() == id)
    {
        // Found, so just replace property
        *iterator._Ptr = Class::PropertyPtr(property);
    }
    else
    {
        // Not found, insert new property
        properties.emplace(iterator, Class::PropertyPtr(property));
    }

    m_currentTagHolder = m_currentProperty = property;
    m_currentFunction = nullptr;
}

//-------------------------------------------------------------------------------------------------
void ClassBuilderBase::addFunction(Function* function)
{
    // Retrieve the class' functions sorted by ID
    Class::SortedFunctionVector& functions = m_target->m_functions;

    // Replace any function that already exists with the same ID
    const uint32_t id = function->id();
    Class::SortedFunctionVector::const_iterator iterator = std::lower_bound(functions.cbegin(), functions.cend(), id, Class::OrderByFunctionId());
    if (iterator != functions.end() && (*iterator._Ptr)->id() == id)
    {
        // Found, so just replace function
        *iterator._Ptr = Class::FunctionPtr(function);
    }
    else
    {
        // Not found, insert new function
        functions.emplace(iterator, Class::FunctionPtr(function));
    }

    m_currentTagHolder = m_currentFunction = function;
    m_currentProperty = nullptr;
}

} // namespace camp
