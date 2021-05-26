#if $generator.convert_legacy
int handler${arg_idx} = toluafix_ref_function(tolua_S, ${arg_idx}, 0);
ok &= handler${arg_idx} != 0;
if(ok) {
	#set $param_count = len($param_types)
	#set $param_str = []
	#set $count = 0
	#while $count < $param_count
	#set $param = $param_types[$count]
	#set _ = $param_str.append($param_types[$count].namespaced_name + " param" + str($count))
	#set $count = $count + 1
	#end while
	#set $param_str = ", ".join($param_str)
	#set $count = 0
	${out_value} = [=](${param_str})
	{
		#set $ret_str = ""
		#if $ret_type.name != "void"
			#if $ret_type.is_enum
			#set $ret_str = "(${ntype.to_string($generator)})ret"
		int ret = {};
			#else
			#set $ret_str = "ret"
		${ntype.to_string($generator)} ret = {};
			#end if
		#end if
		toluafix_get_function_by_refid(tolua_S, handler${arg_idx});
		if (!lua_isfunction(tolua_S, -1)) {
			lua_pop(tolua_S, 1);
			return ${ret_str};
		}
		#while $count < $param_count
		#set $param_type = $param_types[$count]
		${param_type.from_native({
				"generator": $generator,
				"in_value": "param" + str($count),
				"out_value": "param" + str($count),
				"type_name": $param_type.namespaced_name.replace("*", ""),
				"ntype": $param_type.get_whole_name($generator),
				"class_name": $class_name,
				"level": 2,
				"scriptname": $generator.scriptname_from_native($param_type.namespaced_name, $param_type.namespace_name)})};
		#set $count = $count + 1
		#end while
		#if $ret_type.name != "void"
		lua_call(tolua_S, ${param_count}, 1);
		bool ok = true;
		${ret_type.to_native({
			"generator": $generator,
			"in_value": "argv[" + str(1) + "]",
			"out_value": "ret",
			"arg_idx": -1,
			"class_name": $class_name,
			"lua_namespaced_class_name": $class_name,
			"func_name": $func_name,
			"level": 2,
			"arg":$ret_type,
			"ntype": $ret_type.namespaced_name.replace("*", ""),
			"scriptname": $generator.scriptname_from_native($ret_type.namespaced_name, $ret_type.namespace_name)})};
		lua_pop(tolua_S, 1);
		if (!ok)
			luaL_error(tolua_S, "invalid return value in handler (%s)", LUA_FNAME);
		#else
		lua_call(tolua_S, ${param_count}, 0);
		#end if
		#if $ret_type.name != "void"
		return ${ret_str};
		#end if
	};
}
#else
ok &= luaval_to_native(tolua_S, ${arg_idx}, &${out_value}, LUA_FNAME);
int handler${arg_idx} = query_luafunction_handler(tolua_S, ${arg_idx}, LUA_FNAME);
ok &= handler${arg_idx} != 0
#end if