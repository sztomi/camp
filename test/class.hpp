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

namespace ClassTest
{
    struct MyTempClass
    {
    };

    struct MyUndeclaredClass
    {
    };

    struct MyClass
    {
        void func() {}
        int prop;
    };

    struct MyClass2
    {
    };

    struct Base
    {
        CAMP_RTTI();
    };

    struct Derived : Base
    {
        CAMP_RTTI();
    };

    struct DerivedNoRtti : Base
    {
    };

    struct Derived2NoRtti : Derived
    {
    };

    void declare()
    {
        camp::Class::declare<MyClass>()
            .property("prop", &MyClass::prop)
            .function("func", &MyClass::func);

        camp::Class::declare<MyClass2>();

        camp::Class::declare<Base>();

        camp::Class::declare<Derived>()
            .base<Base>();

        camp::Class::declare<DerivedNoRtti>()
            .base<Base>();

        camp::Class::declare<Derived2NoRtti>()
            .base<Derived>();
    }
}

CAMP_TYPE(ClassTest::MyUndeclaredClass /* never declared */)
CAMP_TYPE(ClassTest::MyTempClass /* declared in a test */)
CAMP_AUTO_TYPE(ClassTest::MyClass, &ClassTest::declare)
CAMP_AUTO_TYPE(ClassTest::MyClass2, &ClassTest::declare)
CAMP_AUTO_TYPE(ClassTest::Base, &ClassTest::declare)
CAMP_AUTO_TYPE(ClassTest::Derived, &ClassTest::declare)
CAMP_AUTO_TYPE(ClassTest::DerivedNoRtti, &ClassTest::declare)
CAMP_AUTO_TYPE(ClassTest::Derived2NoRtti, &ClassTest::declare)
