## ===== static function implementation template - for overloaded functions
#set func_cobj_type = $generator.scriptname_from_native($namespaced_class_name, $namespace_name)
#set func_lua_name = func_cobj_type + ':' + $implementations[0].func_name
int ${signature_name}(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_OBJ_TYPE = "${func_cobj_type}";
	constexpr auto LUA_FNAME = "${func_lua_name}";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	#for func in $implementations
	#if len($func.arguments) >= $func.min_args
	#set arg_count = len($func.arguments)
	#set arg_idx = $func.min_args
	#while $arg_idx <= $arg_count
	do {
		if (argc == ${arg_idx}) {
			#set arg_list = ""
			#set arg_array = []
			#set count = 0
			#while $count < $arg_idx
			#set $arg = $func.arguments[$count]
			${arg.to_string($generator)} arg${count};
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
			#if $arg_idx >= 0
			if (!ok) { break; }
			#end if
			#end while
			#set $arg_list = ", ".join($arg_array)
			#if str($func.ret_type) != "void"
				#if $func.ret_type.is_enum
			auto ret = ${namespaced_class_name}::${func.func_name}($arg_list);
				#else
			auto ret = ${namespaced_class_name}::${func.func_name}($arg_list);
				#end if
			${func.ret_type.from_native({"generator": $generator,
										 "in_value": "ret",
										 "out_value": "jsret",
										 "type_name": $func.ret_type.namespaced_name.replace("*", ""),
										 "ntype": $func.ret_type.get_whole_name($generator),
										 "class_name": $class_name,
										 "level": 2,
										 "scriptname": $generator.scriptname_from_native($func.ret_type.namespaced_name, $func.ret_type.namespace_name)})};
			return 1;
			#else
			${namespaced_class_name}::${func.func_name}($arg_list);
			lua_settop(tolua_S, 1);
			return 1;
			#end if
		}
	} while (0);
	#set $arg_idx = $arg_idx + 1
	ok = true;
	#end while
	#end if
	#end for
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "${func.min_args}");
}
