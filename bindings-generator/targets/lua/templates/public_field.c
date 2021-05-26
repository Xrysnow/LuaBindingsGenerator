## ===== member implementation template
#set func_cobj_type = $generator.scriptname_from_native($namespaced_class_name, $namespace_name)
#set getter_lua_name = func_cobj_type + '.' + $name + ' getter'
#set setter_lua_name = func_cobj_type + '.' + $name + ' setter'
int ${signature_name}_get${name}(lua_State* tolua_S)
{
	constexpr auto LUA_OBJ_TYPE = "${func_cobj_type}";
	constexpr auto LUA_FNAME = "${getter_lua_name}";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (${namespaced_class_name}*)tolua_tousertype(tolua_S, 1, 0);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
#if $generator.convert_legacy
	#if $ntype.is_object and not $ntype.object_can_convert($generator, False)
		#set field_ntype = $ntype.get_whole_name($generator) + "*"
	#else
		#set field_ntype = $ntype.get_whole_name($generator)
	#end if
	${ntype.from_native({"generator": $generator,
						 "type_name": $ntype.namespaced_name.replace("*", ""),
						 "ntype": $field_ntype,
						 "level": 1,
						 "scriptname": $generator.scriptname_from_native($ntype.namespaced_name, $ntype.namespace_name),
						 "in_value":"cobj->" + $pretty_name,
						 })};
#else
	native_to_luaval(tolua_S, cobj->$name);
#end if
	return 1;
}
int ${signature_name}_set${name}(lua_State* tolua_S)
{
	constexpr auto LUA_OBJ_TYPE = "${func_cobj_type}";
	constexpr auto LUA_FNAME = "${setter_lua_name}";
	bool ok = true;
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (${namespaced_class_name}*)tolua_tousertype(tolua_S, 1, 0);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (1 == argc) {
#if $generator.convert_legacy
		${ntype.to_string($generator)} arg0;
		${ntype.to_native({"generator": $generator,
							"arg_idx": 2,
							"out_value": "arg0",
							"lua_namespaced_class_name": $generator.scriptname_from_native($namespaced_class_name, $namespace_name),
							"func_name": "LUA_FNAME",
							"scriptname": $generator.scriptname_from_native($ntype.namespaced_name, $ntype.namespace_name),
							"level": 2,
							"arg":$ntype,
					  })};
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		cobj->$pretty_name = arg0;
#else
		ok &= luaval_to_native(tolua_S, 2, &cobj->$name, LUA_FNAME);
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
#end if
		return 0;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
