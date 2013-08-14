
#ifndef __MSLEEP_H_
#define __MSLEEP_H_

#include <QtLua/Function>

class MSleep: public QtLua::Function
{
public:
    MSleep();
    ~MSleep();

    QtLua::Value::List meta_call( QtLua::State & ls, const QtLua::Value::List & args );
    QtLua::String get_description() const;
    QtLua::String get_help() const;
};


//static class Fcn : public QtLua::Function
//{
//  QtLua::Value::List meta_call(QtLua::State &ls, const QtLua::Value::List &args)
//  {
//    // This function excepts at least 1 Number argument 
//    meta_call_check_args(args, 1, 0, QtLua::Value::TNumber);
//
//    return QtLua::Value::List();
//  }
//
//  QtLua::String get_description() const
//  {
//    return "Useless function";
//  }
//
//  QtLua::String get_help() const
//  {
//    return "Use this function to perform no operation.";
//  }
//
//} fcn;
//
//fcn.register_(*state, "fcn");
//
//}




#endif



