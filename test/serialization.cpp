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

#include "serialization.hpp"
#include <camp-xml/rapidxml.hpp>
#include <boost/test/unit_test.hpp>

#include <iostream>
#include "rapidxml_print.hpp"

using namespace SerializationTest;
using namespace rapidxml;

//-----------------------------------------------------------------------------
//                         Tests for camp serialization
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_SUITE(SERIALIZATION)

template<typename T>
std::string get_xml_string(T& obj)
{
    xml_document<> doc;
    xml_node<>* decl = doc.allocate_node(node_declaration);
    decl->append_attribute(doc.allocate_attribute("version", "1.0"));
    decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
    doc.append_node(decl);

    xml_node<>* root = doc.allocate_node(node_element, "data");
    doc.append_node(root);

    auto& meta = camp::classByType<T>();

    char* obj_node_name = doc.allocate_string(meta.name());
    xml_node<>* obj_node = doc.allocate_node(node_element, obj_node_name);
    root->append_node(obj_node);

    // serialize it, excluding properties tagged with "noxml"
    camp::xml::serialize(obj, obj_node, "noxml");

    std::string result;
    rapidxml::print(std::back_inserter(result), doc);
    return result;
}

template<typename T>
void deserialize(T* obj, std::string const& xml)
{
    auto& meta = camp::classByType<T>();
    xml_document<> doc;
    std::string buf = xml;
    doc.parse<0>(&buf[0]);
    auto* root = doc.first_node("data");
    auto* obj_node = root->first_node(meta.name());
    camp::xml::deserialize(*obj, obj_node, "noxml");
}

//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(XmlSerializationWithVector)
{
    Person john(35, "John", American);
    john.children.push_back(Person(12, "Marie", French));
    john.children.push_back(Person(7, "Rose", English));
    john.children.push_back(Person(2, "Giovanni", Italian));

    std::string original_serialized_str = get_xml_string(john);

    Person john2;
    deserialize(&john2, original_serialized_str);

    std::string second_serialized_str = get_xml_string(john2);

    BOOST_CHECK_EQUAL(original_serialized_str, second_serialized_str);
}

BOOST_AUTO_TEST_CASE(XmlSerializationWithSmartPtrVector)
{
    SmartPerson john(35, "John", American);
    john.children.push_back(new Person(12, "Marie", French));
    john.children.push_back(new Person(7, "Rose", English));
    john.children.push_back(new Person(2, "Giovanni", Italian));

    std::string original_serialized_str = get_xml_string(john);

    SmartPerson john2;
    deserialize(&john2, original_serialized_str);

    std::string second_serialized_str = get_xml_string(john2);

    std::cout << second_serialized_str << std::endl;

    BOOST_CHECK_EQUAL(original_serialized_str, second_serialized_str);
}

BOOST_AUTO_TEST_SUITE_END()
