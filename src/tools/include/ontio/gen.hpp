#pragma once

#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/Builtins.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"
#include <functional>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <regex>
#include <sstream>

namespace ontio { namespace cdt {

struct generation_utils {
   std::string contract_name;

   inline void set_contract_name( const std::string& cn ) { contract_name = cn; }
   inline std::string get_contract_name()const { return contract_name; }

   static inline std::string get_action_name( const clang::CXXMethodDecl* decl ) {
      return decl->getNameAsString();
   }

   static inline bool is_ontio_contract( const clang::CXXMethodDecl* decl, const std::string& cn ) {
      std::string name = "";
      if (name.empty()) {
         name = decl->getParent()->getName().str();
      }
      return name == cn;
   }

   inline bool is_template_specialization( const clang::QualType& type, const std::vector<std::string>& names ) { 
      auto check = [&](const clang::Type* pt) {
         if (auto tst = llvm::dyn_cast<clang::TemplateSpecializationType>(pt)) {
            if (auto rt = llvm::dyn_cast<clang::RecordType>(tst->desugar())) {
               if ( names.empty() ) {
                  return true;
               } else {
		  for ( auto name : names ) {
                     if ( rt->getDecl()->getName().str() == name ) {
                        return true;
                     }
		  }
               }
            }
         }
        return false;
      };
      bool is_specialization = false;
      if (auto pt = llvm::dyn_cast<clang::ElaboratedType>(type.getTypePtr())) {
         is_specialization = check(pt->desugar().getTypePtr());
      }
      else
         is_specialization = check(type.getTypePtr());

      return is_specialization;
   }

   inline clang::TemplateArgument get_template_argument( const clang::QualType& type, int index = 0 ) {
      auto ret = [&](const clang::Type* t) {
         auto tst = llvm::dyn_cast<clang::TemplateSpecializationType>(t);
         if (tst) {
            auto arg = tst->getArg(index);
            return arg;
         }
         std::cout << "Internal error, wrong type of template specialization\n";
         return tst->getArg(index);
      };
      if (auto pt = llvm::dyn_cast<clang::ElaboratedType>(type.getTypePtr()))
         return ret(pt->desugar().getTypePtr());
      return ret(type.getTypePtr());
   }

   std::string get_base_type_name( const clang::QualType& type ) {
      clang::QualType newType = type;
      std::string type_str = newType.getNonReferenceType().getAsString();
      return get_base_type_name(type_str);
   }

   std::string get_base_type_name( const std::string& type_str ) {
      int i = type_str.length()-1;
      int template_nested = 0;
      for (; i > 0; i--) {
         if (type_str[i] == '>') ++template_nested;
         if (type_str[i] == '<') --template_nested;
         if (type_str[i] == ':' && template_nested == 0) {
            return type_str.substr(i+1);
         }
         if (type_str[i] == ' ' && template_nested == 0) {
            static const std::string valid_prefixs[] = {"long", "signed", "unsigned" };
            bool valid = false;
            for (auto &s : valid_prefixs) {
               if (i >= s.length() && type_str.substr(i - s.length(), s.length()) == s &&
                   (i - s.length() == 0 || type_str[i - s.length() - 1] == ' ')) {
                  valid = true;
                  i -= s.length();
                  break;
               }
            }
            if (valid) continue;
            return type_str.substr(i+1);
         }
      }
      return type_str;
   }

   std::string _translate_type( const clang::QualType& type ) {
      clang::QualType newType = type;
      std::string type_str = newType.getNonReferenceType().getAsString();
      return _translate_type(get_base_type_name(type_str));
   }

   std::string _translate_type( const std::string& t ) {
      static std::map<std::string, std::string> translation_table =
      {
         {"unsigned __int128", "uint128"},
         {"__int128", "int128"},
         {"uint128_t", "uint128"},
         {"int128_t", "int128"},

         {"unsigned long long", "uint64"},
         {"long long", "int64"},
         {"uint64_t", "uint64"},
         {"int64_t", "int64"},

         {"unsigned long", "uint32"},
         {"long", "int32"},
         {"unsigned int", "uint32"},
         {"int", "int32"},
         {"uint32_t", "uint32"},
         {"int32_t", "int32"},

         {"unsigned short", "uint16"},
         {"short", "int16"},
         {"uint16_t", "uint16"},
         {"int16_t", "int16"},

         {"unsigned char", "uint8"},
         {"signed char", "int8"},
         {"char", "int8"},
         {"uint8_t", "uint8"},
         {"int8_t", "int8"},
	 {"_Bool", "bool"},

         {"unsigned_int", "varuint32"},
         {"signed_int",   "varint32"},
      };
      
      auto ret = translation_table[t];

      if (ret == "")
         return t;
      return ret;
   }

   inline std::string replace_in_name( std::string name ) {
      std::string ret = name;
      std::replace(ret.begin(), ret.end(), '<', '_');
      std::replace(ret.begin(), ret.end(), '>', '_');
      std::replace(ret.begin(), ret.end(), ',', '_');
      std::replace(ret.begin(), ret.end(), ' ', '_');
      std::replace(ret.begin(), ret.end(), ':', '_');
      return ret;
   }

   inline std::string get_template_name( const clang::QualType& type ) {
      auto pt = llvm::dyn_cast<clang::ElaboratedType>(type.getTypePtr());
      auto tst = llvm::dyn_cast<clang::TemplateSpecializationType>(pt ? pt->desugar().getTypePtr() : type.getTypePtr());
      std::string ret = tst->getTemplateName().getAsTemplateDecl()->getName().str()+"_";
      for (int i=0; i < tst->getNumArgs(); ++i) {
         auto arg = get_template_argument(type,i);
         if (arg.getAsExpr()) {
            auto ce = llvm::dyn_cast<clang::CastExpr>(arg.getAsExpr());
            if (ce) { 
               auto il = llvm::dyn_cast<clang::IntegerLiteral>(ce->getSubExpr());
               ret += std::to_string(il->getValue().getLimitedValue());
               if ( i < tst->getNumArgs()-1 )
                  ret += "_";
            }
         }
         else {
            ret += _translate_type(get_template_argument( type, i ).getAsType());
            if ( i < tst->getNumArgs()-1 )
               ret += "_";
         }
      }
      return _translate_type(replace_in_name(ret));
   }

   inline std::string translate_type( const clang::QualType& type ) {
      if ( is_template_specialization( type, {"vector", "set", "deque", "list"} ) ) {
         auto t =translate_type(get_template_argument( type ).getAsType());
         return t=="int8" ? "bytes" : t+"[]";
      }
      else if ( is_template_specialization( type, {"array"} )) {
         auto t0 = translate_type(get_template_argument( type ).getAsType());
         auto arg = get_template_argument( type, 1).getAsExpr();
         return t0 =="uint8" ? "bytes[20]" : t0+"[]";
      }
      else if ( is_template_specialization( type, {"optional"} ) )
         return translate_type(get_template_argument( type ).getAsType())+"?";
      else if ( is_template_specialization( type, {"map"} )) {
         auto t0 = translate_type(get_template_argument( type ).getAsType());
         auto t1 = translate_type(get_template_argument( type, 1).getAsType());
         return replace_in_name("pair_" + t0 + "_" + t1 + "[]");
      }
      else if ( is_template_specialization( type, {"pair"} )) {
         auto t0 = translate_type(get_template_argument( type ).getAsType());
         auto t1 = translate_type(get_template_argument( type, 1).getAsType());
         return replace_in_name("pair_" + t0 + "_" + t1);
      }
      else if ( is_template_specialization( type, {"tuple"} )) {
         auto pt = llvm::dyn_cast<clang::ElaboratedType>(type.getTypePtr());
         auto tst = llvm::dyn_cast<clang::TemplateSpecializationType>( pt ? pt->desugar().getTypePtr() : type.getTypePtr() );
         std::string ret = "tuple_";
         for (int i=0; i < tst->getNumArgs(); ++i) {
            ret += _translate_type(get_template_argument( type, i ).getAsType());
            if ( i < tst->getNumArgs()-1 )
               ret += "_";
         }
         return replace_in_name(ret);
      }
      else if ( is_template_specialization( type, {} )) {
         auto pt = llvm::dyn_cast<clang::ElaboratedType>(type.getTypePtr());
         auto tst = llvm::dyn_cast<clang::TemplateSpecializationType>(pt ? pt->desugar().getTypePtr() : type.getTypePtr() );
         std::string ret = tst->getTemplateName().getAsTemplateDecl()->getName().str()+"_";
         for (int i=0; i < tst->getNumArgs(); ++i) {
            auto arg = get_template_argument(type,i);
            if (auto ce = arg.getKind() == clang::TemplateArgument::ArgKind::Expression
                  ? llvm::dyn_cast<clang::CastExpr>(arg.getAsExpr()) : nullptr) {
               auto il = llvm::dyn_cast<clang::IntegerLiteral>(ce->getSubExpr());
               ret += std::to_string(il->getValue().getLimitedValue());
               if ( i < tst->getNumArgs()-1 )
                  ret += "_";
            }
            else {
               ret += translate_type(get_template_argument( type, i ).getAsType());
               if ( i < tst->getNumArgs()-1 )
                  ret += "_";
            }
         }
         return _translate_type(replace_in_name(ret));
      }
      return _translate_type( type );
   }

   inline bool is_builtin_type( const std::string& t ) {
      static const std::set<std::string> builtins =
      {
         "bool",
         "int8",
         "uint8",
         "int16",
         "uint16",
         "int32",
         "uint32",
         "int64",
         "uint64",
         "int128",
         "uint128",
         "varint32",
         "varuint32",
         "name",
         "bytes",
         "string",
	 "address",
         "asset",
	 "key"
      };

      return builtins.count(_translate_type(t)) >= 1;
   }

   inline bool is_builtin_type( const clang::QualType& t ) {
      std::string nt = translate_type(t);
      return is_builtin_type(nt);
   }

   inline bool is_cxx_record( const clang::QualType& t ) {
      return t.getTypePtr()->isRecordType();
   }

   inline std::string get_type( const clang::QualType& t ) {
      return translate_type(t);
   }

   inline std::string get_type_alias_string( const clang::QualType& t ) {
      if (auto dt = llvm::dyn_cast<clang::TypedefType>(t.getTypePtr()))
         return get_type(dt->desugar());
      return get_type(t);
   }

   inline std::vector<clang::QualType> get_type_alias( const clang::QualType& t ) {
      if (auto dt = llvm::dyn_cast<clang::TypedefType>(t.getTypePtr()))
         return {dt->desugar()};
      return {};
   }

   inline bool is_aliasing( const clang::QualType& t ) {
      if (is_builtin_type(t))
         return false;
      if (get_base_type_name(t).find("<") != std::string::npos) return false;
      return get_base_type_name(t).compare(get_type_alias_string(t)) != 0;
   }
};
}} // ns ontio::cdt
