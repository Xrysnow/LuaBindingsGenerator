## ===== instance function implementation template - for overloaded functions
#set func_cobj_type = $generator.scriptname_from_native($namespaced_class_name, $namespace_name)
#set func_lua_name = func_cobj_type + ':' + $implementations[0].func_name
int ${signature_name}(lua_State* tolua_S)
{
	bool ok = true;
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
#for func in $implementations
#if len($func.arguments) >= $func.min_args
	#set arg_count = len($func.arguments)
	#set arg_idx = $func.min_args
	#while $arg_idx <= $arg_count
	#set arg_list = ""
	#set arg_array = []
	do {
		#if $func.min_args >= 0
		if (argc == ${arg_idx}) {
			#set $count = 0
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
		#end if
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
			#if $func.ret_type.name != "void"
				#if $func.ret_type.is_enum
			auto ret = cobj->${func.func_name}($arg_list);
				#else
			auto ret = cobj->${func.func_name}($arg_list);
				#end if
			${func.ret_type.from_native({"generator": $generator,
													  "in_value": "ret",
													  "out_value": "ret",
													  "type_name": $func.ret_type.namespaced_name.replace("*", ""),
													  "ntype": $func.ret_type.get_whole_name($generator),
													  "class_name": $class_name,
													  "level": 2,
													  "scriptname": $generator.scriptname_from_native($func.ret_type.namespaced_name, $func.ret_type.namespace_name)})};
			return 1;
			#else
			cobj->${func.func_name}($arg_list);
			lua_settop(tolua_S, 1);
			return 1;
			#end if
		#end if
		}
	} while (false);
	#set $arg_idx = $arg_idx + 1
	ok = true;
	#end while
#end if
#end for
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "${func.min_args}");
}
