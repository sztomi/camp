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
void ClassBuilderBase::addBase(Class& baseClass, int offset)
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
        Class::SortedPropertyVector& targetPropertiesByIndex = m_target->m_propertiesByIndex;
        Class::SortedPropertyVector& targetPropertiesById = m_target->m_propertiesById;
        Class::SortedPropertyVector& baseProperties = baseClass.m_propertiesById;
        const size_t numberOfProperties = baseProperties.size();
        for (size_t i = 0; i < numberOfProperties; ++i)
        {
            Class::PropertyEntry& basePropertyEntry = baseProperties[i];

            // Replace any property that already exists with the same ID
            const uint32_t id = basePropertyEntry.id;
            Class::SortedPropertyVector::iterator iterator = std::lower_bound(targetPropertiesById.begin(), targetPropertiesById.end(), id, Class::OrderByPropertyId());
            if (iterator != targetPropertiesById.end() && iterator->id == id)
            {
                // Found, so just replace property
                std::find_if(targetPropertiesByIndex.begin(), targetPropertiesByIndex.end(), [id](const Class::PropertyEntry& other) { return (other.id == id); })->propertyPtr = basePropertyEntry.propertyPtr;
                iterator->propertyPtr = basePropertyEntry.propertyPtr;
            }
            else
            {
                // Not found, insert new property
                targetPropertiesByIndex.emplace_back(Class::PropertyEntry(id, basePropertyEntry.propertyPtr));
                targetPropertiesById.emplace(iterator, Class::PropertyEntry(id, basePropertyEntry.propertyPtr));
            }
        }
    }

    { // Copy all functions of the base class into the current class
        Class::SortedFunctionVector& targetFunctions = m_target->m_functions;
        Class::SortedFunctionVector& baseFunctions = baseClass.m_functions;
        const size_t numberOfFunctions = baseFunctions.size();
        for (size_t i = 0; i < numberOfFunctions; ++i)
        {
            Class::FunctionEntry& baseFunctionEntry = baseFunctions[i];

            // Replace any function that already exists with the same ID
            const uint32_t id = baseFunctionEntry.id;
            Class::SortedFunctionVector::iterator iterator = std::lower_bound(targetFunctions.begin(), targetFunctions.end(), id, Class::OrderByFunctionId());
            if (iterator != targetFunctions.end() && iterator->id == id)
            {
                // Found, so just replace function
                iterator->functionPtr = baseFunctionEntry.functionPtr;
            }
            else
            {
                // Not found, insert new function
                targetFunctions.emplace(iterator, Class::FunctionEntry(id, baseFunctionEntry.functionPtr));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
void ClassBuilderBase::addConstructor(Constructor* constructor)
{
    m_target->m_constructors.push_back(constructor);
}

//-------------------------------------------------------------------------------------------------
void ClassBuilderBase::addProperty(Property* property)
{
    const uint32_t id = property->id();
    Class::PropertyPtr propertyPtr(property);

    // Replace any property that already exists with the same ID
    { // Sorted by index
        // Retrieve the class' properties sorted by ID
        Class::SortedPropertyVector& properties = m_target->m_propertiesByIndex;

        Class::SortedPropertyVector::iterator iterator = std::find_if(properties.begin(), properties.end(), [id](const Class::PropertyEntry& other) { return (other.id == id); });
        if (iterator != properties.end())
        {
            // Found, so just replace property
            iterator->propertyPtr = propertyPtr;
        }
        else
        {
            // Not found, insert new property
            properties.emplace_back(Class::PropertyEntry(id, propertyPtr));
        }
    }
    { // Sorted by ID
        // Retrieve the class' properties sorted by ID
        Class::SortedPropertyVector& properties = m_target->m_propertiesById;

        Class::SortedPropertyVector::iterator iterator = std::lower_bound(properties.begin(), properties.end(), id, Class::OrderByPropertyId());
        if (iterator != properties.end() && iterator->id == id)
        {
            // Found, so just replace property
            iterator->propertyPtr = propertyPtr;
        }
        else
        {
            // Not found, insert new property
            properties.emplace(iterator, Class::PropertyEntry(id, propertyPtr));
        }
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
    Class::SortedFunctionVector::iterator iterator = std::lower_bound(functions.begin(), functions.end(), id, Class::OrderByFunctionId());
    if (iterator != functions.end() && iterator->id == id)
    {
        // Found, so just replace function
        iterator->functionPtr = Class::FunctionPtr(function);
    }
    else
    {
        // Not found, insert new function
        functions.emplace(iterator, Class::FunctionEntry(id, Class::FunctionPtr(function)));
    }

    m_currentTagHolder = m_currentFunction = function;
    m_currentProperty = nullptr;
}

//-------------------------------------------------------------------------------------------------
void ClassBuilderBase::addTag(const char* name, const detail::Getter<Value>& value)
{
    assert(m_currentTagHolder);

    // Retrieve the tag holders tags sorted by ID
    const StringId id(name);
    TagHolder::SortedTagVector& tags = m_currentTagHolder->m_tags;

    // Replace any tag that already exists with the same ID
    TagHolder::SortedTagVector::iterator iterator = std::lower_bound(tags.begin(), tags.end(), id, TagHolder::OrderByTagId());
    if (iterator != tags.end() && iterator->id == id)
    {
        // Found, so just replace tag value
        // -> Should not happen for efficiency reasons
        assert(false);
        iterator->value = value;
    }
    else
    {
        // Not found, insert new tag
        tags.emplace(iterator, TagHolder::TagEntry(id, name, value));
    }
}


} // namespace camp
