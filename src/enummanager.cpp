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


#include <camp/detail/enummanager.hpp>
#include <camp/enum.hpp>
#include <camp/errors.hpp>


namespace camp
{
namespace detail
{

//-------------------------------------------------------------------------------------------------
EnumManager& EnumManager::instance()
{
    static EnumManager manager;
    return manager;
}

//-------------------------------------------------------------------------------------------------
Enum& EnumManager::addClass(const char* name)
{
    // First make sure that the enum doesn't already exist
    const StringId id(name);
    SortedEnumVector::const_iterator iterator = std::lower_bound(m_enums.cbegin(), m_enums.cend(), id, OrderByEnumId());
    if (iterator != m_enums.end() && iterator._Ptr->id == id)
    {
        CAMP_ERROR(EnumAlreadyCreated(name));
    }

    // Create the new class
    Enum* newEnum = new Enum(name);

    // Insert it into the sorted vector
    m_enums.emplace(iterator, EnumEntry(id, newEnum));

    // Notify observers
    notifyEnumAdded(*newEnum);

    // Done
    return *newEnum;
}

//-------------------------------------------------------------------------------------------------
std::size_t EnumManager::count() const
{
    return m_enums.size();
}

//-------------------------------------------------------------------------------------------------
const Enum& EnumManager::getByIndex(std::size_t index) const
{
    // Make sure that the index is not out of range
    if (index >= m_enums.size())
        CAMP_ERROR(OutOfRange(index, m_enums.size()));

    return *m_enums[index].enumPtr;
}

//-------------------------------------------------------------------------------------------------
const Enum& EnumManager::getById(StringId id) const
{
    SortedEnumVector::const_iterator iterator = std::lower_bound(m_enums.cbegin(), m_enums.cend(), id, OrderByEnumId());
    if (iterator != m_enums.end() && iterator._Ptr->id == id)
    {
        // Found
        return *iterator._Ptr->enumPtr;
    }
    else
    {
        // Not found
        CAMP_ERROR(EnumNotFound(id));
    }
}

//-------------------------------------------------------------------------------------------------
const Enum* EnumManager::getByIdSafe(StringId id) const
{
    SortedEnumVector::const_iterator iterator = std::lower_bound(m_enums.cbegin(), m_enums.cend(), id, OrderByEnumId());
    return (iterator != m_enums.end() && iterator._Ptr->id == id) ? iterator._Ptr->enumPtr : nullptr;
}

//-------------------------------------------------------------------------------------------------
bool EnumManager::enumExists(StringId id) const
{
    SortedEnumVector::const_iterator iterator = std::lower_bound(m_enums.cbegin(), m_enums.cend(), id, OrderByEnumId());
    return (iterator != m_enums.end() && iterator._Ptr->id == id);
}

//-------------------------------------------------------------------------------------------------
EnumManager::EnumManager()
{
}

//-------------------------------------------------------------------------------------------------
EnumManager::~EnumManager()
{
    // Notify observers
    const size_t numberOfEnums = m_enums.size();
    for (size_t i = 0; i < numberOfEnums; ++i)
    {
        Enum* enumPtr = m_enums[i].enumPtr;
        notifyEnumRemoved(*enumPtr);
        delete enumPtr;
    }
}

} // namespace detail

} // namespace camp
