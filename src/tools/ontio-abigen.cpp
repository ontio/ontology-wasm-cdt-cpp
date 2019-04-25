// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/Builtins.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "llvm/Support/FileSystem.h"
#include <iostream>
#include <typeinfo>
#include <ontio/gen.hpp>
#include <ontio/whereami.hpp>
#include <ontio/abi.hpp>

#include <exception>
#include <fstream>
#include <sstream>
#include <memory>
#include <set>
#include <map>
#include <chrono>
#include <ctime>


#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <array>
#include <string> 

#include <jsoncons/json.hpp>

#include "llvm/Support/CommandLine.h"
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace llvm;
using namespace ontio;
using namespace ontio::cdt;
using jsoncons::json;
using jsoncons::ojson;
using std::cout;
using std::endl;
using std::string;
using std::array;

struct abigen_exception : public std::exception {
   virtual const char* what() const throw() {
      return "ontio.abigen fatal error";
   }
} abigen_ex;

DeclarationMatcher function_decl_matcher = cxxMethodDecl().bind("ontio_abis");
StatementMatcher stringLiteral_matcher  = stringLiteral(hasParent(implicitCastExpr(hasParent(callExpr(hasParent(constantExpr()), hasDescendant(declRefExpr(to(functionDecl(hasName("hash_at_compile_time"))))))))), hasAncestor(caseStmt(has(constantExpr(has(callExpr(hasDescendant(declRefExpr(to(functionDecl(hasName("hash_at_compile_time"))))))))), hasDescendant(stringLiteral()), hasAncestor(functionDecl(allOf(hasName("invoke"), hasParent(linkageSpecDecl()),isExternC(),parameterCountIs(0), returns(asString("void")))))))).bind("ontio_abis");

class abigen : public generation_utils {
   public:

   void add_typedef( const clang::QualType& t ) {
      abi_typedef ret;
      ret.new_type_name = get_base_type_name( t );
      auto td = get_type_alias(t);
      if (td.empty())
         return;
      ret.type = translate_type(td[0]);
      if(!is_builtin_type(td[0]))
         add_type(td[0]);
      _abi.typedefs.insert(ret);
   }
   
   void add_tuple(const clang::QualType& type) {
      auto pt = llvm::dyn_cast<clang::ElaboratedType>(type.getTypePtr());
      auto tst = llvm::dyn_cast<clang::TemplateSpecializationType>(pt->desugar().getTypePtr());
      if (!tst)
         throw abigen_ex;
      abi_struct tup;
      tup.name = get_type(type);
      for (int i = 0; i < tst->getNumArgs(); ++i) {
         clang::QualType ftype = get_template_argument(type, i).getAsType();
         add_type(ftype);
         tup.fields.push_back( {"field_"+std::to_string(i),
               translate_type(ftype)} );
      }
      _abi.structs.insert(tup);
   }
   
   void add_pair(const clang::QualType& type) {
      for (int i = 0; i < 2; ++i) {
         clang::QualType ftype = get_template_argument(type, i).getAsType();
         std::string ty = translate_type(ftype);
         add_type(ftype);
      }
      abi_struct pair;
      pair.name = get_type(type);
      pair.fields.push_back( {"first", translate_type(get_template_argument(type).getAsType())} );
      pair.fields.push_back( {"second", translate_type(get_template_argument(type, 1).getAsType())} );   
      add_type(get_template_argument(type).getAsType());
      add_type(get_template_argument(type, 1).getAsType());
      _abi.structs.insert(pair);
   } 

   void add_map(const clang::QualType& type) {
      for (int i = 0; i < 2; ++i) {
         clang::QualType ftype = get_template_argument(type, i).getAsType();
         std::string ty = translate_type(ftype);
         add_type(ftype);
      }
      abi_struct kv;
      std::string name = get_type(type);
      kv.name = name.substr(0, name.length() - 2);
      kv.fields.push_back( {"key", translate_type(get_template_argument(type).getAsType())} );
      kv.fields.push_back( {"value", translate_type(get_template_argument(type, 1).getAsType())} );   
      add_type(get_template_argument(type).getAsType());
      add_type(get_template_argument(type, 1).getAsType());
      _abi.structs.insert(kv);
   }

   void add_struct( const clang::CXXRecordDecl* decl, const std::string& rname="" ) {
      abi_struct ret;
      if ( decl->getNumBases() == 1 ) {
         ret.base = get_type(decl->bases_begin()->getType());
         add_struct(decl->bases_begin()->getType().getTypePtr()->getAsCXXRecordDecl());
      }

      std::string sub_name = "";
      for ( auto field : decl->fields() ) {
      	ret.fields.push_back({field->getName().str(), get_type(field->getType())});
	sub_name += "_" + get_type(field->getType());
	add_type(field->getType());
      }
      if (!rname.empty())
         ret.name = rname;
      else
         ret.name = decl->getName().str();
      _abi.structs.insert(ret);
   }

   void add_action( const clang::CXXMethodDecl* decl ) {
      abi_action new_action;
      new_action.name 	= decl->getNameAsString();

      if(_abi.action_names.find(new_action.name) == _abi.action_names.end())
	 return;

      new_action.retype = get_type(decl->getCallResultType().getNonReferenceType().getUnqualifiedType());

      for (auto param : decl->parameters() ) {
         auto param_type = param->getType().getNonReferenceType().getUnqualifiedType();
         new_action.fields.push_back({param->getNameAsString(), get_type(param_type)});
         add_type(param_type);
      }
      _abi.actions.insert(new_action);
   }

   void add_action_name( const std::string & action_name ) {
      _abi.action_names.insert(action_name);
   }

   void add_variant( const clang::QualType& t ) {
      abi_variant var;
      auto pt = llvm::dyn_cast<clang::ElaboratedType>(t.getTypePtr());
      auto tst = llvm::dyn_cast<clang::TemplateSpecializationType>(pt ? pt->desugar().getTypePtr() : t.getTypePtr());
      var.name = get_type(t);
      for (int i=0; i < tst->getNumArgs(); ++i)
         var.types.push_back(translate_type(get_template_argument( t, i ).getAsType()));
      _abi.variants.insert(var); 
   }

   void add_type( const clang::QualType& type ) {
      if (!is_builtin_type(translate_type(type))) {
         if (is_aliasing(type))
            add_typedef(type);
	 else if (is_template_specialization(type, {"array", "vector", "set", "deque", "list", "optional"})) {
	    auto m_type = get_template_argument(type).getAsType();
            add_type(m_type);
         }
         else if (is_template_specialization(type, {"map"}))
            add_map(type);
         else if (is_template_specialization(type, {"pair"}))
            add_pair(type);
         else if (is_template_specialization(type, {"tuple"}))
            add_tuple(type);
         else if (is_template_specialization(type, {"variant"}))
            add_variant(type);
         else if (is_template_specialization(type, {})) {
            add_struct(type.getTypePtr()->getAsCXXRecordDecl(), get_template_name(type));
         }
	 else if (type.getTypePtr()->isRecordType()) {
            add_struct(type.getTypePtr()->getAsCXXRecordDecl());
	 }
      }
   }

   std::string generate_json_comment() {
      std::stringstream ss;
      ss << "This file was generated with ontio-abigen.";
      ss << " DO NOT EDIT";
      return ss.str();
   }

   ojson action_to_json( const abi_action& s ) {
      ojson o;
      o["name"] = s.name;
      o["retype"] = s.retype;
      o["args"] = ojson::array();
      for ( auto field : s.fields ) {
         ojson f;
         f["name"] = field.name;
         f["type"] = field.type;
         o["args"].push_back(f);
      }
      return o;
   }

   ojson struct_to_json( const abi_struct& s ) {
      ojson o;
      o["name"] = s.name;
      o["base"] = s.base;
      o["fields"] = ojson::array();
      for ( auto field : s.fields ) {
         ojson f;
         f["name"] = field.name;
         f["type"] = field.type;
         o["fields"].push_back(f);
      }
      return o;
   }

   ojson variant_to_json( const abi_variant& v ) {
      ojson o;
      o["name"] = v.name;
      o["types"] = ojson::array();
      for ( auto ty : v.types ) {
         o["types"].push_back( ty );
      }
      return o;
   }

   ojson typedef_to_json( const abi_typedef& t ) {
      ojson o;
      o["new_type_name"] = t.new_type_name;
      o["type"]          = t.type;
      return o;
   }

   ojson to_json() {
      ojson o;
      o["comment"] = generate_json_comment();
      o["CompilerVersion"]     = _abi.version;

      auto remove_suffix = [&]( std::string name ) {
         int i = name.length()-1;
         for (; i >= 0; i--) 
            if ( name[i] != '[' && name[i] != ']' && name[i] != '?' && name[i] != '$' )
               break;
         return name.substr(0,i+1);
      };

      auto validate_struct = [&]( abi_struct as ) {
         if ( is_builtin_type(_translate_type(as.name)) )
            return false;
      };

      o["entrypoint"] 	 = "invoke";
      o["hash"]  	= _abi.filehash;
      
      o["functions"]     = ojson::array();
      for ( auto a : _abi.actions ) {
         o["functions"].push_back(action_to_json( a ));
      }

      o["structs"]     = ojson::array();
      for ( auto s : _abi.structs ) {
         o["structs"].push_back(struct_to_json(s));
      }

      o["types"]       = ojson::array();
      for ( auto t : _abi.typedefs ) {
      	o["types"].push_back(typedef_to_json( t ));
      }

      return o;
   }

   abi& get_abi_ref() { return _abi; }


   private: 
      abi                                   _abi;
};

abigen& get_abigen_ref() {
   static abigen ag;
   return ag;
}

class ontioMethodMatcher : public MatchFinder::MatchCallback {
   public:
      virtual void run( const MatchFinder::MatchResult& res ) {
         if (const clang::CXXMethodDecl* decl = res.Nodes.getNodeAs<clang::CXXMethodDecl>("ontio_abis")->getCanonicalDecl()) {
            if (abigen::is_ontio_contract(decl, get_abigen_ref().get_contract_name())) {
      	       auto name = decl->getNameAsString();
	       /*ignore contract construct and destory method*/
	       if (name == get_abigen_ref().get_contract_name() or name == ('~' + get_abigen_ref().get_contract_name()))
	          return;
               get_abigen_ref().add_action(decl);
               }
            }
         }
};

class ontioAbiMatcher : public MatchFinder::MatchCallback {
   public:
      virtual void run( const MatchFinder::MatchResult& res ) {
         if (const clang::StringLiteral* decl = res.Nodes.getNodeAs<clang::StringLiteral>("ontio_abis")) {
	    std::string action_name = decl->getString();
            get_abigen_ref().add_action_name(action_name);
         }
     }
};

string tohexstring(array<uint8_t, 20> &tmp) {
	std::vector<char> s;
	s.resize(40);
	int index = 0;
	/*here use rbegin to reverse the byte array*/
	for(auto it = tmp.rbegin(); it != tmp.rend(); it++) {
		uint8_t high = *it/16, low = *it%16;
		s[index] = (high<10) ? ('0' + high) : ('a' + high - 10);
		s[index + 1] = (low<10) ? ('0' + low) : ('a' + low - 10);
		index += 2;
	}

	string res;
	res.resize(40);
	memcpy(res.data(), s.data(), 40);
	return res;
}

void generate_filehash(const std::string binaryname) {
	FILE *fp = NULL; 
	string cmd = "openssl dgst -binary -sha256 " + binaryname + " | openssl dgst -binary -ripemd160";
	fp = popen((char *)cmd.data(), "r"); 
	if(!fp) {
		perror("popen"); 
		exit(EXIT_FAILURE);  
	} 
	array<uint8_t, 20> s;
	memset((char*)s.data(), 0, 20);
	fread((char *)s.data(), 21, 1, fp);
	pclose(fp);

	string filehash  = tohexstring(s);
	get_abigen_ref().get_abi_ref().filehash = filehash;
}

int main(int argc, const char **argv) {

   cl::SetVersionPrinter([](llvm::raw_ostream& os) {
        os << "ontio-abigen version " << "v1.0" << "\n";
  });
   cl::OptionCategory cat("ontio-abigen", "generates an abi from C++ project input");

   cl::opt<std::string> abidir(
    "output",
    cl::desc("Set the output filename and fullpath"),
    cl::Required,
    cl::cat(cat));
   cl::opt<std::string> contract_name(
    "contract",
    cl::desc("Set the contract name"),
    cl::Required,
    cl::cat(cat));
   cl::opt<std::string> binary_name(
    "binary",
    cl::desc("Set the binary name"),
    cl::Required,
    cl::cat(cat));
    
   std::vector<std::string> options;
   for (size_t i=0; i < argc; i++) {
      options.push_back(argv[i]);
   }
   bool has_dash_dash = false;
   for (auto opt : options) {
      if ( opt.compare("--") == 0 ) {
         has_dash_dash = true;
         break;
      }
   }
   if (!has_dash_dash)
      options.push_back("--");
   options.push_back("--target=wasm32");
   options.push_back("-nostdlib");
   options.push_back("-ffreestanding");
   options.push_back("-fno-builtin");
   options.push_back("-fno-rtti");
   options.push_back("-fno-exceptions");
   options.push_back("-I${Boost_INCLUDE_DIRS}");
   options.push_back("-DBOOST_DISABLE_ASSERTS");
   options.push_back("-DBOOST_EXCEPTION_DISABLE");
   options.push_back("-Wno-everything");
   options.push_back("-std=c++17");
   options.push_back(std::string("-I")+ontio::cdt::whereami::where()+"/../include/libcxx");
   options.push_back(std::string("-I")+ontio::cdt::whereami::where()+"/../include/libc");
   options.push_back(std::string("-I")+ontio::cdt::whereami::where()+"/../include/boost");
   options.push_back(std::string("-I")+ontio::cdt::whereami::where()+"/../include/ontiolib");
   options.push_back(std::string("-I")+ontio::cdt::whereami::where()+"/../include");

   int size = options.size();
   const char** new_argv = new const char*[size];
   for (size_t i=0; i < size; i++)
      new_argv[i] = options[i].c_str();

   CommonOptionsParser opts( size, new_argv, cat, 0 );

   ClangTool tool( opts.getCompilations(), opts.getSourcePathList());
   get_abigen_ref().set_contract_name(contract_name);
   generate_filehash(binary_name);

   ontioAbiMatcher ontio_abi_matcher;
   MatchFinder finder_stringLiteral;

   ontioMethodMatcher ontio_method_matcher;
   MatchFinder finder_method_decl;

   finder_stringLiteral.addMatcher(stringLiteral_matcher, &ontio_abi_matcher);
   finder_method_decl.addMatcher(function_decl_matcher, &ontio_method_matcher);

   int tool_run = -1;
   try {
      tool_run = tool.run(newFrontendActionFactory(&finder_stringLiteral).get());
      tool_run = tool.run(newFrontendActionFactory(&finder_method_decl).get());
      std::ofstream output(abidir);
      output << pretty_print(get_abigen_ref().to_json());
      output.close();
   } catch (std::exception& ex) {
      std::cout << ex.what() << "\n";
      tool_run = -1;
   } 

   return tool_run;
}
