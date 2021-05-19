## ===== static function implementation template
#set func_cobj_type = $generator.scriptname_from_native($namespaced_class_name, $namespace_name)
#set func_lua_name = func_cobj_type + ':' + $func_name
int ${signature_name}(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "${func_cobj_type}";
	constexpr auto LUA_FNAME = "${func_lua_name}";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
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
				#set $arg_array += ["arg"+str($count)]
				#set $count = $count + 1
		#end while
		#if $arg_idx >= 0
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		#end if
			#set $arg_list = ", ".join($arg_array)
		#if $ret_type.name != "void"
			#if $ret_type.is_enum
		auto ret = ${namespaced_class_name}::${func_name}($arg_list);
			#else
		auto ret = ${namespaced_class_name}::${func_name}($arg_list);
		#end if
		${ret_type.from_native({"generator": $generator,
								"in_value": "ret",
								"out_value": "ret",
								"type_name": $ret_type.namespaced_name.replace("*", ""),
								"ntype": $ret_type.get_whole_name($generator),
								"class_name": $class_name,
								"level": 2,
								"scriptname": $generator.scriptname_from_native($ret_type.namespaced_name, $ret_type.namespace_name)})};
		return 1;
		#else
		${namespaced_class_name}::${func_name}($arg_list);
		lua_settop(tolua_S, 1);
		return 1;
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
