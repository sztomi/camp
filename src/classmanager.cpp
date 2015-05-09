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


#include <camp/detail/classmanager.hpp>
#include <camp/class.hpp>
#include <camp/errors.hpp>
#include <camp/observer.hpp>


namespace camp
{
namespace detail
{
//-------------------------------------------------------------------------------------------------
ClassManager& ClassManager::instance()
{
    return inst();
}

//-------------------------------------------------------------------------------------------------
Class& ClassManager::addClass(const char* name)
{
    // First make sure that the class doesn't already exist
    const StringId id(name);
    if (m_classes.find(id) != m_classes.end())
    {
        CAMP_ERROR(ClassAlreadyCreated(name));
    }

    // Create the new class
    Class* newClass = new Class(name);

    // Insert it into the table
    m_classes.insert(std::make_pair(id, newClass));

    // Notify observers
    notifyClassAdded(*newClass);

    // Done
    return *newClass;
}

//-------------------------------------------------------------------------------------------------
std::size_t ClassManager::count() const
{
    return m_classes.size();
}

//-------------------------------------------------------------------------------------------------
const Class& ClassManager::getByIndex(std::size_t index) const
{
    // Make sure that the index is not out of range
    if (index >= m_classes.size())
        CAMP_ERROR(OutOfRange(index, m_classes.size()));

    ClassTable::const_iterator it = m_classes.begin();
    std::advance(it, index);

    return *it->second;
}

//-------------------------------------------------------------------------------------------------
const Class& ClassManager::getById(StringId id) const
{
    ClassTable::const_iterator it = m_classes.find(id);
    if (it == m_classes.end())
        CAMP_ERROR(ClassNotFound(id));

    return *it->second;
}

//-------------------------------------------------------------------------------------------------
const Class* ClassManager::getByIdSafe(StringId id) const
{
    ClassTable::const_iterator it = m_classes.find(id);
    return (it == m_classes.end()) ? nullptr : it->second;
}

//-------------------------------------------------------------------------------------------------
bool ClassManager::classExists(StringId id) const
{
    return (m_classes.find(id) != m_classes.end());
}

//-------------------------------------------------------------------------------------------------
ClassManager::ClassManager()
{
}

//-------------------------------------------------------------------------------------------------
ClassManager::~ClassManager()
{
    // Notify observers
    for (ClassTable::const_iterator it = m_classes.begin(); it != m_classes.end(); ++it)
    {
        Class* classPtr = it->second;
        notifyClassRemoved(*classPtr);
        delete classPtr;
    }
}

} // namespace detail

} // namespace camp
