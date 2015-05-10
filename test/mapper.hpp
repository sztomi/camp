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


#pragma once


#include <camp/camptype.hpp>
#include <camp/class.hpp>
#include <camp/simpleproperty.hpp>
#include <camp/function.hpp>
#include <map>
#include <string>

namespace MapperTest
{
    struct MyClass
    {
        enum
        {
            propertyCount = 5,
            functionCount = 3
        };

        static const char* property(std::size_t index)
        {
            static const char* names[] = {"prop0", "prop1", "prop2", "prop3", "prop4"};
            return names[index];
        }

        static const char* function(std::size_t index)
        {
            static const char* names[] = {"func0", "func1", "func2"};
            return names[index];
        }

        MyClass()
        {
            m_props[camp::StringId(property(0))] = 0;
            m_props[camp::StringId(property(1))] = 10;
            m_props[camp::StringId(property(2))] = 20;
            m_props[camp::StringId(property(3))] = 30;
            m_props[camp::StringId(property(4))] = 40;
        }

        int& prop(camp::StringId id)
        {
            return m_props[id];
        }

        const char* func(const char* name)
        {
            static std::string result;
            result = name;
            result += "_called";
            return result.c_str();
        }

        std::map<uint32_t, int> m_props;
    };

    template <typename T>
    struct MyMapper
    {
        std::size_t propertyCount()
        {
            return T::propertyCount;
        }

        camp::Property* property(std::size_t index)
        {
            return new MyProperty(T::property(index));
        }

        std::size_t functionCount()
        {
            return T::functionCount;
        }

        camp::Function* function(std::size_t index)
        {
            return new MyFunction(T::function(index));
        }

        struct MyProperty : public camp::SimpleProperty
        {
        public:

            MyProperty(const char* name)
                : camp::SimpleProperty(name, camp::intType)
            {
            }

            virtual camp::Value getValue(const camp::UserObject& object) const
            {
                return object.get<T>().prop(id());
            }

            virtual void setValue(const camp::UserObject& object, const camp::Value& value) const
            {
                object.get<T>().prop(id()) = value.to<int>();
            }
        };

        class MyFunction : public camp::Function
        {
        public:

            MyFunction(const char* name)
                : camp::Function(name, camp::stringType)
            {
            }

            virtual camp::Value execute(const camp::UserObject& object, const camp::Args&) const
            {
                T& t = object.get<T>();
                return t.func(name());
            }
        };
    };

    void declare()
    {
        camp::Class::declare<MyClass>()
            .external<MyMapper>();
    }
}

CAMP_AUTO_TYPE(MapperTest::MyClass, &MapperTest::declare)
