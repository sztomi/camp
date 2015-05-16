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


#include <camp/tagholder.hpp>


namespace camp
{
//-------------------------------------------------------------------------------------------------
TagHolder::~TagHolder()
{
}

//-------------------------------------------------------------------------------------------------
std::size_t TagHolder::tagCount() const
{
    return m_tags.size();
}

//-------------------------------------------------------------------------------------------------
const TagHolder::TagEntry& TagHolder::getTagEntryByIndex(std::size_t index) const
{
    // Make sure that the index is not out of range
    if (index >= m_tags.size())
        CAMP_ERROR(OutOfRange(index, m_tags.size()));

    return m_tags[index];
}

//-------------------------------------------------------------------------------------------------
bool TagHolder::hasTag(StringId id) const
{
    SortedTagVector::const_iterator iterator = std::lower_bound(m_tags.cbegin(), m_tags.cend(), id, OrderByTagId());
    return (iterator != m_tags.end() && (*iterator._Ptr).id == id);
}

//-------------------------------------------------------------------------------------------------
const Value& TagHolder::tag(StringId id) const
{
    SortedTagVector::const_iterator iterator = std::lower_bound(m_tags.cbegin(), m_tags.cend(), id, OrderByTagId());
    return (iterator != m_tags.end() && (*iterator._Ptr).id == id) ? (*iterator._Ptr).value.get() : Value::nothing;
}

//-------------------------------------------------------------------------------------------------
Value TagHolder::tag(StringId id, const UserObject& object) const
{
    SortedTagVector::const_iterator iterator = std::lower_bound(m_tags.cbegin(), m_tags.cend(), id, OrderByTagId());
    return (iterator != m_tags.end() && (*iterator._Ptr).id == id) ? (*iterator._Ptr).value.get(object) : Value::nothing;
}

//-------------------------------------------------------------------------------------------------
TagHolder::TagHolder()
{
}

} // namespace camp
