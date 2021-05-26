## ===== static function implementation template
#set func_cobj_type = $generator.scriptname_from_native($namespaced_class_name, $namespace_name)
#if $generator.use_dot_sfunction
#set func_lua_name = func_cobj_type + '.' + $reg_name
#else
#set func_lua_name = func_cobj_type + ':' + $reg_name
#end if
int ${signature_name}(lua_State* tolua_S)
{
#if len($arguments) > 0
	bool ok = true;
#end if
	constexpr auto LUA_FNAME = "${func_lua_name}";
#if $generator.use_dot_sfunction
	const int argc = lua_gettop(tolua_S);
#else
	constexpr auto LUA_OBJ_TYPE = "${func_cobj_type}";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
#end if
#if len($arguments) >= $min_args
	#set arg_count = len($arguments)
	#set arg_idx = $min_args
	#while $arg_idx <= $arg_count
	if (argc == ${arg_idx}) {
		#set arg_list = ""
		#set arg_array = []
		#set $count = 0
		#while $count < $arg_idx
			#set $arg = $arguments[$count]
		${arg.to_string($generator)} arg${count};
			#set $count = $count + 1
		#end while
		#set $count = 0
		#while $count < $arg_idx
			#set $arg = $arguments[$count]
			#if $generator.use_dot_sfunction
				#set $arg_lua_idx = $count+1
			#else
				#set $arg_lua_idx = $count+2
			#end if
		${arg.to_native({"generator": $generator,
						 "in_value": "argv[" + str(count) + "]",
						 "out_value": "arg" + str(count),
						 "arg_idx": $arg_lua_idx,
						 "class_name": $class_name,
						 "lua_namespaced_class_name": $generator.scriptname_from_native($namespaced_class_name, $namespace_name),
						 "func_name": "LUA_FNAME",
						 "level": 2,
						 "arg": $arg,
						 "ntype": $arg.namespaced_name.replace("*", ""),
						 "scriptname": $generator.scriptname_from_native($arg.namespaced_name, $arg.namespace_name)})};
			#set $arg_array += ["arg"+str($count)]
			#set $count = $count + 1
		#end while
		#if $arg_idx > 0
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		#end if
		#set $arg_list = ", ".join($arg_array)
		#set func_invoke = "%s::%s(%s)" % ($namespaced_class_name, $func_name, $arg_list)
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
			#if $generator.use_dot_sfunction
		return 0;
			#else
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
