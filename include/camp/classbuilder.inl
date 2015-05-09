/****************************************************************************
**
** Copyright (C) 2009-2014 TEGESO/TEGESOFT and/or its subsidiary(-ies) and mother company.
** Contact: Tegesoft Information (contact@tegesoft.com)
**
** This file is part of the CAMP library.
**
** The MIT License (MIT)
**
** Copyright (C) 2009-2014 TEGESO/TEGESOFT and/or its subsidiary(-ies) and mother company.
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


namespace camp
{
//-------------------------------------------------------------------------------------------------
template <typename T>
ClassBuilder<T>::ClassBuilder(Class& target)
    : m_target(&target)
    , m_currentTagHolder(m_target)
    , m_currentProperty(nullptr)
    , m_currentFunction(nullptr)
{
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename U>
ClassBuilder<T>& ClassBuilder<T>::base()
{
    // Retrieve the base metaclass
    const Class& baseClass = classByType<U>();

    #ifdef _DEBUG
    {
        // First make sure that the base class is not already a base of the current class
        const uint32_t baseId = baseClass.id();
        Class::BaseList::const_iterator endBase = m_target->m_bases.end();
        for (Class::BaseList::const_iterator it = m_target->m_bases.begin(); it != endBase; ++it)
        {
            assert(it->base->id() != baseId);
        }
    }
    #endif

    // Compute the offset to apply for pointer conversions
    T* asDerived = reinterpret_cast<T*>(1);
    U* asBase = static_cast<U*>(asDerived);
    int offset = static_cast<int>(reinterpret_cast<char*>(asBase) - reinterpret_cast<char*>(asDerived));

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

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename F>
ClassBuilder<T>& ClassBuilder<T>::property(const char* name, F accessor)
{
    // Find the factory which will be able to construct a camp::Property from an accessor of type F
    typedef detail::PropertyFactory1<T, F> Factory;

    // Construct and add the metaproperty
    return addProperty(Factory::get(name, accessor));
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename F1, typename F2>
ClassBuilder<T>& ClassBuilder<T>::property(const char* name, F1 accessor1, F2 accessor2)
{
    // Find the factory which will be able to construct a camp::Property from accessors of type F1 and F2
    typedef detail::PropertyFactory2<T, F1, F2> Factory;

    // Construct and add the metaproperty
    return addProperty(Factory::get(name, accessor1, accessor2));
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename F1, typename F2, typename F3>
ClassBuilder<T>& ClassBuilder<T>::property(const char* name, F1 accessor1, F2 accessor2, F3 accessor3)
{
    // Find the factory which will be able to construct a camp::Property from accessors of type F1, F2 and F3
    typedef detail::PropertyFactory3<T, F1, F2, F3> Factory;

    // Construct and add the metaproperty
    return addProperty(Factory::get(name, accessor1, accessor2, accessor3));
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename F>
ClassBuilder<T>& ClassBuilder<T>::function(const char* name, F function)
{
    // Get a uniform function type from F, whatever it really is
    typedef typename boost::function_types::function_type<F>::type Signature;

    // Construct and add the metafunction
    return addFunction(new detail::FunctionImpl<Signature>(name, function));
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename F>
ClassBuilder<T>& ClassBuilder<T>::function(const char* name, boost::function<F> function)
{
    return addFunction(new detail::FunctionImpl<F>(name, function));
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename F1, typename F2>
ClassBuilder<T>& ClassBuilder<T>::function(const char* name, F1 function1, F2 function2)
{
    // Get uniform function types from F1 and F2, whatever they really are
    typedef typename boost::function_types::function_type<F1>::type Signature1;
    typedef typename boost::function_types::function_type<F2>::type Signature2;

    // Construct and add the metafunction
    return addFunction(new detail::FunctionImpl<Signature1, Signature2>(name, function1, function2));
}

//-------------------------------------------------------------------------------------------------
template <typename T>
ClassBuilder<T>& ClassBuilder<T>::tag(const Value& id)
{
    return tag(id, Value::nothing);
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename U>
ClassBuilder<T>& ClassBuilder<T>::tag(const Value& id, const U& value)
{
    // Make sure we have a valid tag holder, and the tag doesn't already exists
    assert(m_currentTagHolder && !m_currentTagHolder->hasTag(id));

    // For the special case of Getter<Value>, the ambiguity between both constructors
    // cannot be automatically solved, so let's do it manually
    typedef typename boost::mpl::if_c<detail::FunctionTraits<U>::isFunction, boost::function<Value (T&)>, Value>::type Type;

    // Add the new tag (override if already exists)
    m_currentTagHolder->m_tags[id] = detail::Getter<Value>(Type(value));

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
ClassBuilder<T>& ClassBuilder<T>::readable(bool value)
{
    // Make sure we have a valid property
    assert(m_currentProperty != nullptr);

    m_currentProperty->m_readable = detail::Getter<bool>(value);

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename F>
ClassBuilder<T>& ClassBuilder<T>::readable(F function)
{
    // Make sure we have a valid property
    assert(m_currentProperty != nullptr);

    m_currentProperty->m_readable = detail::Getter<bool>(boost::function<bool (T&)>(function));

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
ClassBuilder<T>& ClassBuilder<T>::writable(bool value)
{
    // Make sure we have a valid property
    assert(m_currentProperty != nullptr);

    m_currentProperty->m_writable = detail::Getter<bool>(value);

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename F>
ClassBuilder<T>& ClassBuilder<T>::writable(F function)
{
    // Make sure we have a valid property
    assert(m_currentProperty != nullptr);

    m_currentProperty->m_writable = detail::Getter<bool>(boost::function<bool (T&)>(function));

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
ClassBuilder<T>& ClassBuilder<T>::callable(bool value)
{
    // Make sure we have a valid function
    assert(m_currentFunction != nullptr);

    m_currentFunction->m_callable = detail::Getter<bool>(value);

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename F>
ClassBuilder<T>& ClassBuilder<T>::callable(F function)
{
    // Make sure we have a valid function
    assert(m_currentFunction != nullptr);

    m_currentFunction->m_callable = detail::Getter<bool>(boost::function<bool (T&)>(function));

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
ClassBuilder<T>& ClassBuilder<T>::constructor0()
{
    Constructor* constructor = new detail::ConstructorImpl0<T>;
    m_target->m_constructors.push_back(Class::ConstructorPtr(constructor));

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename A0>
ClassBuilder<T>& ClassBuilder<T>::constructor1()
{
    Constructor* constructor = new detail::ConstructorImpl1<T, A0>;
    m_target->m_constructors.push_back(Class::ConstructorPtr(constructor));

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename A0, typename A1>
ClassBuilder<T>& ClassBuilder<T>::constructor2()
{
    Constructor* constructor = new detail::ConstructorImpl2<T, A0, A1>;
    m_target->m_constructors.push_back(Class::ConstructorPtr(constructor));

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename A0, typename A1, typename A2>
ClassBuilder<T>& ClassBuilder<T>::constructor3()
{
    Constructor* constructor = new detail::ConstructorImpl3<T, A0, A1, A2>;
    m_target->m_constructors.push_back(Class::ConstructorPtr(constructor));

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename A0, typename A1, typename A2, typename A3>
ClassBuilder<T>& ClassBuilder<T>::constructor4()
{
    Constructor* constructor = new detail::ConstructorImpl4<T, A0, A1, A2, A3>;
    m_target->m_constructors.push_back(Class::ConstructorPtr(constructor));

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <typename A0, typename A1, typename A2, typename A3, typename A4>
ClassBuilder<T>& ClassBuilder<T>::constructor5()
{
    Constructor* constructor = new detail::ConstructorImpl5<T, A0, A1, A2, A3, A4>;
    m_target->m_constructors.push_back(Class::ConstructorPtr(constructor));

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
template <template <typename> class U>
ClassBuilder<T>& ClassBuilder<T>::external()
{
    // Create an instance of the mapper
    U<T> mapper;

    // Retrieve the properties
    std::size_t propertyCount = mapper.propertyCount();
    for (std::size_t i = 0; i < propertyCount; ++i)
        addProperty(mapper.property(i));

    // Retrieve the functions
    std::size_t functionCount = mapper.functionCount();
    for (std::size_t i = 0; i < functionCount; ++i)
        addFunction(mapper.function(i));

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
Class& ClassBuilder<T>::getClass()
{
    return *m_target;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
ClassBuilder<T>& ClassBuilder<T>::addProperty(Property* property)
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
        properties.insert(iterator, Class::PropertyPtr(property));
    }

    m_currentTagHolder = m_currentProperty = property;
    m_currentFunction = nullptr;

    return *this;
}

//-------------------------------------------------------------------------------------------------
template <typename T>
ClassBuilder<T>& ClassBuilder<T>::addFunction(Function* function)
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
        functions.insert(iterator, Class::FunctionPtr(function));
    }

    m_currentTagHolder = m_currentFunction = function;
    m_currentProperty = nullptr;

    return *this;
}

} // namespace camp
