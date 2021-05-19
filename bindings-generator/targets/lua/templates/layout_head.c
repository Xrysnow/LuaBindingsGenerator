\#include "${out_file}.hpp"
#if $macro_judgement
$macro_judgement
#end if
#for header in $headers
	#set relative = os.path.relpath(header, $search_path)
	#if not '..' in relative
\#include "${relative.replace(os.path.sep, '/')}"
	#else
\#include "${os.path.basename(header)}"
	#end if
#end for
\#include "scripting/lua-bindings/manual/tolua_fix.h"
\#include "scripting/lua-bindings/manual/LuaBasicConversions.h"
#if $cpp_headers
#for header in $cpp_headers
\#include "${header}"
#end for
#end if
#if $extra_layout_head
${extra_layout_head}
#end if

\#ifndef LUA_CHECK_COBJ_TYPE
	\#ifdef LUA_DEBUG
		\#define LUA_CHECK_COBJ_TYPE(L, TYPE, NAME) if(!tolua_isusertype((L), 1, (TYPE), 0, nullptr)) { return luaL_error((L), "invalid 'cobj' in '%s': '%s', expects '%s'", NAME, tolua_typename((L), 1), (TYPE)); }
	\#else
		\#define LUA_CHECK_COBJ_TYPE(L, TYPE, NAME) (void)(TYPE);
	\#endif
\#endif
\#ifndef LUA_CHECK_COBJ
	\#ifdef LUA_DEBUG
		\#define LUA_CHECK_COBJ(L, COBJ, NAME) if(!(COBJ)) { return luaL_error((L), "invalid 'cobj' in '%s'", NAME); }
	\#else
		\#define LUA_CHECK_COBJ(L, COBJ, NAME)
	\#endif
\#endif
\#ifndef LUA_CHECK_PARAMETER
	\#define LUA_CHECK_PARAMETER(L, OK, NAME) if(!(OK)) { return luaL_error((L), "invalid arguments in '%s'", NAME); }
\#endif
\#ifndef LUA_PARAMETER_ERROR
	\#define LUA_PARAMETER_ERROR(L, NAME, ARGC, EXPECT) return luaL_error((L), "wrong number of arguments in '%s': %d, expects %s", NAME, (ARGC), EXPECT);
\#endif
