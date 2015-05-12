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
    SortedClassVector::const_iterator iterator = std::lower_bound(m_classes.cbegin(), m_classes.cend(), id, OrderByClassId());
    if (iterator != m_classes.end() && iterator._Ptr->id == id)
    {
        CAMP_ERROR(ClassAlreadyCreated(name));
    }

    // Create the new class
    Class* newClass = new Class(name);

    // Insert it into the sorted vector
    m_classes.emplace(iterator, ClassEntry(id, newClass));

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

    return *m_classes[index].classPtr;
}

//-------------------------------------------------------------------------------------------------
const Class& ClassManager::getById(StringId id) const
{
    SortedClassVector::const_iterator iterator = std::lower_bound(m_classes.cbegin(), m_classes.cend(), id, OrderByClassId());
    if (iterator != m_classes.end() && iterator._Ptr->id == id)
    {
        // Found
        return *iterator._Ptr->classPtr;
    }
    else
    {
        // Not found
        CAMP_ERROR(ClassNotFound(id));
    }
}

//-------------------------------------------------------------------------------------------------
const Class* ClassManager::getByIdSafe(StringId id) const
{
    SortedClassVector::const_iterator iterator = std::lower_bound(m_classes.cbegin(), m_classes.cend(), id, OrderByClassId());
    return (iterator != m_classes.end() && iterator._Ptr->id == id) ? iterator._Ptr->classPtr : nullptr;
}

//-------------------------------------------------------------------------------------------------
bool ClassManager::classExists(StringId id) const
{
    SortedClassVector::const_iterator iterator = std::lower_bound(m_classes.cbegin(), m_classes.cend(), id, OrderByClassId());
    return (iterator != m_classes.end() && iterator._Ptr->id == id);
}

//-------------------------------------------------------------------------------------------------
ClassManager::ClassManager()
{
}

//-------------------------------------------------------------------------------------------------
ClassManager::~ClassManager()
{
    // Notify observers
    const size_t numberOfClasses = m_classes.size();
    for (size_t i = 0; i < numberOfClasses; ++i)
    {
        Class* classPtr = m_classes[i].classPtr;
        notifyClassRemoved(*classPtr);
        delete classPtr;
    }
}

} // namespace detail

} // namespace camp
