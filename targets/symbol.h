// $Id: symbol.h,v 1.7 2015/05/20 09:41:37 ist175714 Exp $ -*- c++ -*-
#ifndef __PWN_SEMANTICS_SYMBOL_H__
#define __PWN_SEMANTICS_SYMBOL_H__

#include <string>
#include <cdk/basic_type.h>

namespace pwn {

    class symbol {
      basic_type *_type;
      std::string _name;
      long _value; // hack!
      bool _is_defined;
      bool _is_var;
      std::vector<basic_type*> _args;
      int _offset;
      

    public:
      inline symbol(basic_type *type, const std::string &name, long value, bool defined = false) :
          _type(type), _name(name), _value(value), _is_defined(defined), _is_var(true), _offset(0) {
      }

      virtual ~symbol() {
        delete _type;
      }

      inline basic_type *type() const {
        return _type;
      }
      inline const std::string &name() const {
        return _name;
      }
      inline long value() const {
        return _value;
      }
      inline long value(long v) {
        return _value = v;
      }

      inline bool is_defined() const {
        return _is_defined;	
      }
      
      inline bool is_defined(bool value) {
        return _is_defined = value;	
      }
      
      inline std::vector<basic_type*> args(std::vector<basic_type*> args) {
        return _args = args;	
      }
      inline std::vector<basic_type*> args() const {
        return _args;	
      }

      inline int offset(int offset) {
        return _offset = offset;	
      }

      inline int offset() const {
        return _offset;	
      }
      
      inline bool is_var() const {
        return _is_var;	
      }
      
      inline bool is_var(bool value) {
        return _is_var = value;	
      }

    };

} // pwn

#endif
