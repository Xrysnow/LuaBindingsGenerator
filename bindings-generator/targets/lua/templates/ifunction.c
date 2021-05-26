## ===== instance function implementation template
#set func_cobj_type = $generator.scriptname_from_native($namespaced_class_name, $namespace_name)
#set func_lua_name = func_cobj_type + ':' + $reg_name
int ${signature_name}(lua_State* tolua_S)
{
#if len($arguments) > 0
	bool ok = true;
#end if
	constexpr auto LUA_OBJ_TYPE = "${func_cobj_type}";
#if not $is_constructor
	constexpr auto LUA_FNAME = "${func_lua_name}";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (${namespaced_class_name}*)tolua_tousertype(tolua_S, 1, nullptr);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
#else
	constexpr auto LUA_FNAME = "${func_cobj_type} constructor";
#end if
	const int argc = lua_gettop(tolua_S) - 1;
#if len($arguments) >= $min_args
	#set arg_count = len($arguments)
	#set arg_idx = $min_args
	#while $arg_idx <= $arg_count
	if (argc == ${arg_idx}) {
		#set $count = 0
		#while $count < $arg_idx
			#set $arg = $arguments[$count]
		${arg.to_string($generator)} arg${count};
			#set $count = $count + 1
		#end while
		#set $count = 0
		#set arg_list = ""
		#set arg_array = []
		#while $count < $arg_idx
			#set $arg = $arguments[$count]
		${arg.to_native({"generator": $generator,
									  "in_value": "argv[" + str(count) + "]",
									  "out_value": "arg" + str(count),
									  "arg_idx": $count+2,
									  "class_name": $class_name,
									  "lua_namespaced_class_name": $generator.scriptname_from_native($namespaced_class_name, $namespace_name),
									  "func_name": "LUA_FNAME",
									  "level": 2,
									  "arg":$arg,
									  "ntype": $arg.namespaced_name.replace("*", ""),
									  "scriptname": $generator.scriptname_from_native($arg.namespaced_name, $arg.namespace_name)})};
			#set $arg_array += ["arg"+str(count)]
			#set $count = $count + 1
		#end while
		#if $arg_idx > 0
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		#end if
		#set $arg_list = ", ".join($arg_array)
		#if $is_constructor
		auto cobj = new ${namespaced_class_name}($arg_list);
#if not $generator.script_control_cpp
	#if $is_ref_class
		cobj->autorelease();
		int ID = (int)cobj->_ID ;
		int* luaID = &cobj->_luaID ;
		toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj, LUA_OBJ_TYPE);
	#else
		tolua_pushusertype(tolua_S, (void*)cobj, LUA_OBJ_TYPE);
		tolua_register_gc(tolua_S, lua_gettop(tolua_S));
	#end if
#else
		tolua_pushusertype(tolua_S, (void*)cobj, LUA_OBJ_TYPE);
		tolua_register_gc(tolua_S, lua_gettop(tolua_S));
#end if
		return 1;
		#else
			#set func_invoke = "cobj->%s(%s)" % ($func_name, $arg_list)
			#if $ret_type.name != "void"
#if $generator.convert_legacy
		auto ret = ${func_invoke};
		${ret_type.from_native({"generator": $generator,
								"in_value": "ret",
								"out_value": "ret",
								"type_name": $ret_type.namespaced_name.replace("*", ""),
								"ntype": $ret_type.get_whole_name($generator),
								"class_name": $class_name,
								"level": 2,
								"scriptname": $generator.scriptname_from_native($ret_type.namespaced_name, $ret_type.namespace_name)})};
#else
		native_to_luaval(tolua_S, ${func_invoke});
#end if
		return 1;
			#else
		${func_invoke};
		lua_settop(tolua_S, 1);
		return 1;
			#end if
		#end if
	}
		#set $arg_idx = $arg_idx + 1
	#end while
#end if
#if $min_args != $arg_count
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "${min_args} to ${arg_count}");
#else
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "${min_args}");
#end if
}
