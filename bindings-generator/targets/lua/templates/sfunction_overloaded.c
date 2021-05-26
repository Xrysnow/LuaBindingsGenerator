## ===== static function implementation template - for overloaded functions
#set func_cobj_type = $generator.scriptname_from_native($namespaced_class_name, $namespace_name)
#if $generator.use_dot_sfunction
#set func_lua_name = func_cobj_type + '.' + $implementations[0].reg_name
#else
#set func_lua_name = func_cobj_type + ':' + $implementations[0].reg_name
#end if
int ${signature_name}(lua_State* tolua_S)
{
	bool ok = true;
	constexpr auto LUA_FNAME = "${func_lua_name}";
#if $generator.use_dot_sfunction
	const int argc = lua_gettop(tolua_S);
#else
	constexpr auto LUA_OBJ_TYPE = "${func_cobj_type}";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
#end if
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
				#if $generator.use_dot_sfunction
					#set $arg_lua_idx = $count+1
				#else
					#set $arg_lua_idx = $count+2
				#end if
			${arg.to_string($generator)} arg${count};
			${arg.to_native({"generator": $generator,
							 "in_value": "argv[" + str(count) + "]",
							 "out_value": "arg" + str(count),
							 "arg_idx": $arg_lua_idx,
							 "class_name": $class_name,
							 "lua_namespaced_class_name": $generator.scriptname_from_native($namespaced_class_name, $namespace_name),
							 "func_name": "LUA_FNAME",
							 "level": 3,
							 "arg":$arg,
							 "ntype": $arg.namespaced_name.replace("*", ""),
							 "scriptname": $generator.scriptname_from_native($arg.namespaced_name, $arg.namespace_name)})};
				#set $arg_array += ["arg"+str(count)]
				#set $count = $count + 1
				#if $arg_idx > 0
			if (!ok) { break; }
				#end if
			#end while
			#set $arg_list = ", ".join($arg_array)
			#set func_invoke = "%s::%s(%s)" % ($namespaced_class_name, $func.func_name, $arg_list)
			#if str($func.ret_type) != "void"
#if $generator.convert_legacy
			auto ret = ${func_invoke};
			${func.ret_type.from_native({"generator": $generator,
										 "in_value": "ret",
										 "out_value": "jsret",
										 "type_name": $func.ret_type.namespaced_name.replace("*", ""),
										 "ntype": $func.ret_type.get_whole_name($generator),
										 "class_name": $class_name,
										 "level": 3,
										 "scriptname": $generator.scriptname_from_native($func.ret_type.namespaced_name, $func.ret_type.namespace_name)})};
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
	} while (0);
	#set $arg_idx = $arg_idx + 1
	ok = true;
	#end while
	#end if
	#end for
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "${func.min_args}");
}
