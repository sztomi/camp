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
#include <camp/classbuilder.hpp>
#include <memory>

namespace SerializationTest
{
	enum Nationality
	{
	    American,
	    English,
	    French,
	    Italian
	};

	struct Person
	{
	    Person(int age_ = 0, std::string name_ = "", Nationality nationality_ = American)
	        : sick(false)
	        , age(age_)
	        , name(name_)
	        , nationality(nationality_)
	    {
	    }

	    bool sick;
	    int age;
	    std::string name;
	    Nationality nationality;
	    std::vector<Person> children;
	};

	struct SmartPerson
	{
	    SmartPerson(int age_ = 0, std::string name_ = "", Nationality nationality_ = American)
	        : sick(false)
	        , age(age_)
	        , name(name_)
	        , nationality(nationality_)
	    {
	    }

	    bool sick;
	    int age;
	    std::string name;
	    Nationality nationality;
	    std::vector<Person*> children;
	};

	void registerNationality()
	{
	    camp::Enum::declare<Nationality>()
	        .value("American", American)
	        .value("English", English)
	        .value("French", French)
	        .value("Italian", Italian);
	}

	void registerPerson()
	{
	    camp::Class::declare<Person>()
	        .constructor0()
	        .property("sick", &Person::sick).tag("noxml")
	        .property("age", &Person::age)
	        .property("name", &Person::name)
	        .property("nationality", &Person::nationality)
	        .property("children", &Person::children);
	}

	void registerSmartPerson()
	{
	    camp::Class::declare<SmartPerson>()
	        .constructor0()
	        .property("sick", &SmartPerson::sick).tag("noxml")
	        .property("age", &SmartPerson::age)
	        .property("name", &SmartPerson::name)
	        .property("nationality", &SmartPerson::nationality)
	        .property("children", &SmartPerson::children);
	}
}

CAMP_AUTO_TYPE(SerializationTest::Nationality, &SerializationTest::registerNationality);
CAMP_AUTO_TYPE(SerializationTest::Person, &SerializationTest::registerPerson);
CAMP_AUTO_TYPE(SerializationTest::SmartPerson, &SerializationTest::registerSmartPerson);
