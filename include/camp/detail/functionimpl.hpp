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


#pragma once


#include <camp/args.hpp>
#include <camp/function.hpp>
#include <camp/detail/callhelper.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/assign/list_of.hpp>


namespace camp
{
namespace detail
{
using boost::assign::list_of;

/**
 * \brief Helper function which converts an argument to a C++ type
 *
 * The main purpose of this function is to convert any BadType error to
 * a BadArgument one.
 *
 * \param args List of arguments
 * \param index Index of the argument to convert
 * \param function Name of the calling function, must be valid
 *
 * \return Value of args[index] converted to T
 *
 * \thrown BadArgument conversion triggered a BadType error
 */
template <typename T>
inline typename std::remove_reference<T>::type convertArg(const Args& args, std::size_t index, const char* function)
{
    try
    {
        return args[index].to<typename std::remove_reference<T>::type>();
    }
    catch (const BadType&)
    {
        CAMP_ERROR(BadArgument(args[index].type(), mapType<T>(), index, function));
    }
}


/**
 * \class camp::detail::FunctionImpl
 *
 * \brief Typed specialization of camp::Function.
 *
 * The FunctionImpl class is a template which is specialized
 * according to the underlying function prototype.
 */

template <typename F1, typename F2 = void> class FunctionImpl;

/*
 * Specialization of FunctionImpl for functions taking no argument
 */
template <typename C, typename R>
class FunctionImpl<R (C)> : public Function
{
public:

    /**
     * \brief Constructor
     */
    FunctionImpl(const char* name, boost::function<R (C)> function)
        : Function(name, mapType<R>())
        , m_function(function)
    {
    }

protected:

    /**
     * \see Function::execute
     */
    virtual Value execute(const UserObject& object, const Args&) const override
    {
        return CallHelper<R, C>::call(m_function, object.get<C>());
    }

private:

    boost::function<R (C)> m_function; ///< Object containing the actual function to call
};

/*
 * Specialization of FunctionImpl for functions taking 1 argument
 */
template <typename C, typename R, typename A0>
class FunctionImpl<R (C, A0)> : public Function
{
public:

    /**
     * \brief Constructor
     */
    FunctionImpl(const char* name, boost::function<R (C, A0)> function)
        : Function(name, mapType<R>(), list_of(mapType<A0>()))
        , m_function(function)
    {
    }

protected:

    /**
     * \see Function::execute
     */
    virtual Value execute(const UserObject& object, const Args& args) const override
    {
        return CallHelper<R, C>::call(m_function, object.get<C>()
                                      , convertArg<A0>(args, 0, name()));
    }

private:

    boost::function<R (C, A0)> m_function; ///< Object containing the actual function to call
};

/*
 * Specialization of FunctionImpl for functions taking 2 arguments
 */
template <typename C, typename R, typename A0, typename A1>
class FunctionImpl<R (C, A0, A1)> : public Function
{
public:

    /**
     * \brief Constructor
     */
    FunctionImpl(const char* name, boost::function<R (C, A0, A1)> function)
        : Function(name, mapType<R>(), list_of(mapType<A0>())(mapType<A1>()))
        , m_function(function)
    {
    }

protected:

    /**
     * \see Function::execute
     */
    virtual Value execute(const UserObject& object, const Args& args) const override
    {
        return CallHelper<R, C>::call(m_function, object.get<C>()
                                      , convertArg<A0>(args, 0, name())
                                      , convertArg<A1>(args, 1, name()));
    }

private:

    boost::function<R (C, A0, A1)> m_function; ///< Object containing the actual function to call
};

/*
 * Specialization of FunctionImpl for functions taking 3 arguments
 */
template <typename C, typename R, typename A0, typename A1, typename A2>
class FunctionImpl<R (C, A0, A1, A2)> : public Function
{
public:

    /**
     * \brief Constructor
     */
    FunctionImpl(const char* name, boost::function<R (C, A0, A1, A2)> function)
        : Function(name, mapType<R>(), list_of(mapType<A0>())(mapType<A1>())(mapType<A2>()))
        , m_function(function)
    {
    }

protected:

    /**
     * \see Function::execute
     */
    virtual Value execute(const UserObject& object, const Args& args) const override
    {
        return CallHelper<R, C>::call(m_function, object.get<C>()
                                      , convertArg<A0>(args, 0, name())
                                      , convertArg<A1>(args, 1, name())
                                      , convertArg<A2>(args, 2, name()));
    }

private:

    boost::function<R (C, A0, A1, A2)> m_function; ///< Object containing the actual function to call
};

/*
 * Specialization of FunctionImpl for functions taking 4 arguments
 */
template <typename C, typename R, typename A0, typename A1, typename A2, typename A3>
class FunctionImpl<R (C, A0, A1, A2, A3)> : public Function
{
public:

    /**
     * \brief Constructor
     */
    FunctionImpl(const char* name, boost::function<R (C, A0, A1, A2, A3)> function)
        : Function(name, mapType<R>(), list_of(mapType<A0>())(mapType<A1>())(mapType<A2>())(mapType<A3>()))
        , m_function(function)
    {
    }

protected:

    /**
     * \see Function::execute
     */
    virtual Value execute(const UserObject& object, const Args& args) const override
    {
        return CallHelper<R, C>::call(m_function, object.get<C>()
                                      , convertArg<A0>(args, 0, name())
                                      , convertArg<A1>(args, 1, name())
                                      , convertArg<A2>(args, 2, name())
                                      , convertArg<A3>(args, 3, name()));
    }

private:

    boost::function<R (C, A0, A1, A2, A3)> m_function; ///< Object containing the actual function to call
};

/*
 * Specialization of FunctionImpl for functions taking 5 arguments
 */
template <typename C, typename R, typename A0, typename A1, typename A2, typename A3, typename A4>
class FunctionImpl<R (C, A0, A1, A2, A3, A4)> : public Function
{
public:

    /**
     * \brief Constructor
     */
    FunctionImpl(const char* name, boost::function<R (C, A0, A1, A2, A3, A4)> function)
        : Function(name, mapType<R>(), list_of(mapType<A0>())(mapType<A1>())(mapType<A2>())(mapType<A3>())(mapType<A4>()))
        , m_function(function)
    {
    }

protected:

    /**
     * \see Function::execute
     */
    virtual Value execute(const UserObject& object, const Args& args) const override
    {
        return CallHelper<R, C>::call(m_function, object.get<C>()
                                      , convertArg<A0>(args, 0, name())
                                      , convertArg<A1>(args, 1, name())
                                      , convertArg<A2>(args, 2, name())
                                      , convertArg<A3>(args, 3, name())
                                      , convertArg<A4>(args, 4, name()));
    }

private:

    boost::function<R (C, A0, A1, A2, A3, A4)> m_function; ///< Object containing the actual function to call
};


/*
 * Specialization of FunctionImpl for composed functions taking no argument
 */
template <typename C, typename N, typename M, typename R>
class FunctionImpl<R (N), M (C)> : public Function
{
public:

    /**
     * \brief Constructor
     */
    template <typename F1, typename F2>
    FunctionImpl(const char* name, F1 function, F2 accessor)
        : Function(name, mapType<R>())
        , m_function(boost::bind(function, boost::bind(accessor, _1)))
    {
    }

protected:

    /**
     * \see Function::execute
     */
    virtual Value execute(const UserObject& object, const Args&) const override
    {
        return CallHelper<R, C>::call(m_function, object.get<C>());
    }

private:

    boost::function<R (C)> m_function; ///< Object containing the actual function to call
};

/*
 * Specialization of FunctionImpl for composed functions taking 1 argument
 */
template <typename C, typename N, typename M, typename R, typename A0>
class FunctionImpl<R (N, A0), M (C)> : public Function
{
public:

    /**
     * \brief Constructor
     */
    template <typename F1, typename F2>
    FunctionImpl(const char* name, F1 function, F2 accessor)
        : Function(name, mapType<R>(), list_of(mapType<A0>()))
        , m_function(boost::bind(function, boost::bind(accessor, _1), _2))
    {
    }

protected:

    /**
     * \see Function::execute
     */
    virtual Value execute(const UserObject& object, const Args& args) const override
    {
        return CallHelper<R, C>::call(m_function, object.get<C>()
                                      , convertArg<A0>(args, 0, name()));

    }

private:

    boost::function<R (C, A0)> m_function; ///< Object containing the actual function to call
};

/*
 * Specialization of FunctionImpl for composed functions taking 2 arguments
 */
template <typename C, typename N, typename M, typename R, typename A0, typename A1>
class FunctionImpl<R (N, A0, A1), M (C)> : public Function
{
public:

    /**
     * \brief Constructor
     */
    template <typename F1, typename F2>
    FunctionImpl(const char* name, F1 function, F2 accessor)
        : Function(name, mapType<R>(), list_of(mapType<A0>())(mapType<A1>()))
        , m_function(boost::bind(function, boost::bind(accessor, _1), _2, _3))
    {
    }

protected:

    /**
     * \see Function::execute
     */
    virtual Value execute(const UserObject& object, const Args& args) const override
    {
        return CallHelper<R, C>::call(m_function, object.get<C>()
                                      , convertArg<A0>(args, 0, name())
                                      , convertArg<A1>(args, 1, name()));

    }

private:

    boost::function<R (C, A0, A1)> m_function; ///< Object containing the actual function to call
};

/*
 * Specialization of FunctionImpl for composed functions taking 3 arguments
 */
template <typename C, typename N, typename M, typename R, typename A0, typename A1, typename A2>
class FunctionImpl<R (N, A0, A1, A2), M (C)> : public Function
{
public:

    /**
     * \brief Constructor
     */
    template <typename F1, typename F2>
    FunctionImpl(const char* name, F1 function, F2 accessor)
        : Function(name, mapType<R>(), list_of(mapType<A0>())(mapType<A1>())(mapType<A2>()))
        , m_function(boost::bind(function, boost::bind(accessor, _1), _2, _3, _4))
    {
    }

protected:

    /**
     * \see Function::execute
     */
    virtual Value execute(const UserObject& object, const Args& args) const override
    {
        return CallHelper<R, C>::call(m_function, object.get<C>()
                                   , convertArg<A0>(args, 0, name())
                                   , convertArg<A1>(args, 1, name())
                                   , convertArg<A2>(args, 2, name()));

    }

private:

    boost::function<R (C, A0, A1, A2)> m_function; ///< Object containing the actual function to call
};

/*
 * Specialization of FunctionImpl for composed functions taking 4 arguments
 */
template <typename C, typename N, typename M, typename R, typename A0, typename A1, typename A2, typename A3>
class FunctionImpl<R (N, A0, A1, A2, A3), M (C)> : public Function
{
public:

    /**
     * \brief Constructor
     */
    template <typename F1, typename F2>
    FunctionImpl(const char* name, F1 function, F2 accessor)
        : Function(name, mapType<R>(), list_of(mapType<A0>())(mapType<A1>())(mapType<A2>())(mapType<A3>()))
        , m_function(boost::bind(function, boost::bind(accessor, _1), _2, _3, _4, _5))
    {
    }

protected:

    /**
     * \see Function::execute
     */
    virtual Value execute(const UserObject& object, const Args& args) const override
    {
        return CallHelper<R, C>::call(m_function, object.get<C>()
                                      , convertArg<A0>(args, 0, name())
                                      , convertArg<A1>(args, 1, name())
                                      , convertArg<A2>(args, 2, name())
                                      , convertArg<A3>(args, 3, name()));

    }

private:

    boost::function<R (C, A0, A1, A2, A3)> m_function; ///< Object containing the actual function to call
};

/*
 * Specialization of FunctionImpl for composed functions taking 5 arguments
 */
template <typename C, typename N, typename M, typename R, typename A0, typename A1, typename A2, typename A3, typename A4>
class FunctionImpl<R (N, A0, A1, A2, A3, A4), M (C)> : public Function
{
public:

    /**
     * \brief Constructor
     */
    template <typename F1, typename F2>
    FunctionImpl(const char* name, F1 function, F2 accessor)
        : Function(name, mapType<R>(), list_of(mapType<A0>())(mapType<A1>())(mapType<A2>())(mapType<A3>())(mapType<A4>()))
        , m_function(boost::bind(function, boost::bind(accessor, _1), _2, _3, _4, _5, _6))
    {
    }

protected:

    /**
     * \see Function::execute
     */
    virtual Value execute(const UserObject& object, const Args& args) const override
    {
        return CallHelper<R, C>::call(m_function, object.get<C>()
                                      , convertArg<A0>(args, 0, name())
                                      , convertArg<A1>(args, 1, name())
                                      , convertArg<A2>(args, 2, name())
                                      , convertArg<A3>(args, 3, name())
                                      , convertArg<A4>(args, 4, name()));

    }

private:

    boost::function<R (C, A0, A1, A2, A3, A4)> m_function; ///< Object containing the actual function to call
};

} // namespace detail

} // namespace camp
