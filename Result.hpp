/*
MIT License

Copyright (c) 2024 Cenxuan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once
#ifndef __TRESULT_RESULT_HPP__
#define __TRESULT_RESULT_HPP__
#include <type_traits>
#include <functional>
#include <algorithm>
#include <cstring>

template<typename _Type>
class Option{
protected:
    unsigned char has_val_;
    _Type val_;

    explicit Option(unsigned char _has_val,_Type&& _val)
    noexcept(std::is_nothrow_move_constructible_v<_Type>)
    requires(std::is_move_constructible_v<_Type>)
    :has_val_(has_val_)
    ,val_(std::move(_val)){

    }


    explicit Option(unsigned char _has_val,_Type const& _val)
    noexcept(std::is_nothrow_copy_constructible_v<_Type>)
    requires(std::is_copy_constructible_v<_Type>)
    :has_val_(has_val_)
    ,val_(_val){

    }

    // default constructor for _Type
    explicit Option(unsigned char _has_val)
    noexcept(std::is_nothrow_default_constructible_v<_Type>)
    requires(std::is_default_constructible_v<_Type>)
    :has_val_(has_val_)
    ,val_(){

    }

    // default constructor
    explicit Option()
    noexcept(std::is_nothrow_default_constructible_v<_Type>)
    requires(std::is_default_constructible_v<_Type>)
    :has_val_(0)
    ,val_(){

    }

public:

    // move constrcutor
    explicit Option(Option&& _other)
    noexcept(std::is_nothrow_move_constructible_v<_Type>)
    requires(std::is_move_constructible_v<_Type>)
    :has_val_(_other.has_val_)
    ,val_(std::move(_other.val_))// call move constructor
    {
        _other.has_val_ = 0;
    }


    // copy constrcutor
    // no copyable
    explicit Option(Option const& _other)=delete;
    Option& operator=(Option const& _other) =delete;
    Option& operator=(Option&& _other) = delete;


    // copy from other Option Create a new Option
    static Option copy_from(Option const& _other)
    noexcept(noexcept(Option(_other.has_val_,_other.val_)))
    requires(std::is_copy_constructible_v<_Type>) 
    {
        return Option(_other.has_val_,_other.val_);
    }

    // move from other Option using left value reference
    static Option move_from(Option& _other)
    noexcept(noexcept(Option(_other.has_val_,std::move(_other.val_))))
    requires(std::is_move_constructible_v<_Type>)
    {
        return Option(_other.has_val_,std::move(_other.val_));
    }
    
    // move from other Option using rvalue reference
    static Option move_from(Option&& _other)
    noexcept(noexcept(Option(_other.has_val_,std::forward<Option>(_other.val_))))
    requires(std::is_move_constructible_v<_Type>)
    {
        return Option(_other.has_val_,std::forward<Option>(_other.val_));
    }

    // create a new Option using rvalue reference
    static Option Some(_Type&& _val)
    noexcept(noexcept(Option(1,std::forward<_Type>(_val))))
    requires(std::is_move_constructible_v<_Type>)
    {
        return Option(1,std::forward<_Type>(_val));
    }

    // create a new Option using lvalue reference
    // this function can only be used for types that is copyable but nonmoveable
    // if need to craete an Option for moveable types by using its copy constructor
    // plz use the copy_from function instead
    static Option Some(_Type const& _val)
    noexcept(noexcept(Option(1,_val)))
    requires(!std::is_move_constructible_v<_Type> && std::is_copy_constructible_v<_Type>)
    {
        return Option(1,_val);
    }

    // use defualt constructor to create a "Some" Option
    static Option Some_default()
    noexcept(noexcept(Option(1)))
    {
        return Option(1);
    }

    // use copy constructor to create a "Some" Option
    static Option Some_copy_from(_Type const& _val)
    noexcept(noexcept(Option(1,_val)))
    {
        return Option(1,_val);
    }

    // use default constructor to create a "None" Option
    static Option None()
    noexcept(noexcept(Option(0)))
    requires(std::is_default_constructible_v<_Type>)
    {
        return Option(0);
    }

    // this function is enable for those types that cannot be construct by default constructor
    static Option None()
    noexcept(noexcept(Option(0,std::move(std::declval<_Type&>()))))
    requires(!std::is_default_constructible_v<_Type>)
    {
        unsigned char _fake[sizeof(_Type)]{0};
        _Type& _temp = (*(_Type*)(_fake));
        return Option(0,std::move(_temp));
    }


    bool is_some()const noexcept {
        return has_val_==1;
    }
    bool is_none()const noexcept {
        return has_val_==0;
    }
    _Type& unwrap() noexcept{
        return val_;
    }
    _Type const& unwrap()const noexcept{
        return val_;
    }
    _Type& unwrap_or(_Type const& _val) noexcept{
        if(has_val_) return val_;
        else return const_cast<_Type&>(_val);
    }
    _Type const& unwrap_or(_Type const& _val)const noexcept{
        if(has_val_) return val_;
        else return _val;
    }

};

template<unsigned long long int _1, unsigned long long int _2>
constexpr unsigned long long int __Res_Max = _1>_2?_1:_2;

template<typename _Type_1,typename _Type_2>
class Switch{
protected:
    unsigned char is_first_ = 1;
    void* data_ = nullptr;

    explicit Switch()
    noexcept(std::is_nothrow_default_constructible_v<_Type_1> && noexcept(new _Type_1()))
    requires(std::is_default_constructible_v<_Type_1>)
    :data_((void*)new _Type_1())
    ,is_first_(1){}

    explicit Switch(_Type_1 const& _val)
    noexcept(std::is_nothrow_copy_constructible_v<_Type_1>)
    requires(std::is_copy_constructible_v<_Type_1>)
    :data_((void*)new _Type_1(_val))
    ,is_first_(1){}

    explicit Switch(_Type_1 && _val)
    noexcept(std::is_nothrow_move_constructible_v<_Type_1>)
    requires(std::is_move_constructible_v<_Type_1>)
    :data_((void*)new _Type_1(std::forward<_Type_1>(_val)))
    ,is_first_(1){}

    explicit Switch(_Type_2 const& _val)
    noexcept(std::is_nothrow_copy_constructible_v<_Type_2>)
    requires(std::is_copy_constructible_v<_Type_2>)
    :data_((void*)new _Type_2(_val))
    ,is_first_(1){}

    explicit Switch(_Type_2 && _val)
    noexcept(std::is_nothrow_move_constructible_v<_Type_2>)
    requires(std::is_move_constructible_v<_Type_2>)
    :data_((void*)new _Type_2(std::forward<_Type_2>(_val)))
    ,is_first_(1){}

public:
    explicit Switch(Switch && _other) noexcept
    :data_(_other.data_)
    ,is_first_(_other.is_first_){
        _other.is_first_ = 1;
        _other.data_ = nullptr;
    }

    // copy constrcutor
    // no copyable
    explicit Switch(Switch const& _other)=delete;
    Switch& operator=(Switch const& _other) =delete;
    Switch& operator=(Switch&& _other) = delete;

    // default constructor for _Type_1
    static Switch First()
    noexcept(Switch())
    requires(std::is_default_constructible_v<_Type_1>)
    {
        return Switch();
    }

    static Switch First(_Type_1&& _val)
    noexcept(noexcept(Switch(std::forward<_Type_1>(_val))))
    requires(std::is_move_constructible_v<_Type_1>)
    {
        return Switch(std::forward<_Type_1>(_val));
    }

    static Switch First(_Type_1 const& _val)
    noexcept(noexcept(Switch(_val)))
    requires(std::is_copy_constructible_v<_Type_1> && !std::is_move_constructible_v<_Type_1>)
    {
        return Switch(_val);
    }

    static Switch Second()
    noexcept(Switch())
    requires(std::is_default_constructible_v<_Type_2>)
    {
        return Switch(_Type_2{});
    }

    static Switch Second(_Type_2&& _val)
    noexcept(noexcept(Switch(std::forward<_Type_2>(_val))))
    requires(std::is_move_constructible_v<_Type_2>)
    {
        return Switch(std::forward<_Type_2>(_val));
    }

    static Switch Second(_Type_2 const& _val)
    noexcept(noexcept(Switch(_val)))
    requires(std::is_copy_constructible_v<_Type_2> && !std::is_move_constructible_v<_Type_2>)
    {
        return Switch(_val);
    }

    static Switch copy_from(Switch const& _other)
    noexcept(noexcept(Switch(_other)))
    {
        return Switch(_other);
    }
    static Switch move_from(Switch && _other)
    noexcept(noexcept(Switch(std::forward<Switch>(_other))))
    {
        return Switch(std::forward<Switch>(_other));
    }
    static Switch move_from(Switch const& _other)
    noexcept(noexcept(Switch(std::move(_other))))
    {
        return Switch(std::move(_other));
    }

    bool is_first()noexcept{
        return is_first_==1;
    }
    bool is_second()noexcept{
        return is_first_ !=0;
    }
    _Type_1& get_first()noexcept{
        return *(static_cast<_Type_1*>(data_));
    }
    _Type_2& get_second()noexcept{
        return *(static_cast<_Type_2*>(data_));
    }


};


#endif