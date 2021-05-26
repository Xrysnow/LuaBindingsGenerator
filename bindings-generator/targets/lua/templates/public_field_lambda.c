## ===== member implementation template
#set func_cobj_type = $generator.scriptname_from_native($namespaced_class_name, $namespace_name)
#set field_lua_name = func_cobj_type + '.' + $name
#set getter_lua_name = field_lua_name + ' getter'
#set setter_lua_name = field_lua_name + ' setter'
#set handler_name = func_cobj_type + '_' + $name + '_handler'
#if $generator.convert_legacy
static int ${handler_name} = 0;
#end if
int ${signature_name}_get${name}(lua_State* tolua_S)
{
	constexpr auto LUA_OBJ_TYPE = "${func_cobj_type}";
	constexpr auto LUA_FNAME = "${getter_lua_name}";
	constexpr auto LUA_FIELDNAME = "${field_lua_name}";
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (${namespaced_class_name}*)tolua_tousertype(tolua_S, 1, 0);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
#if $generator.convert_legacy
	if (${handler_name} != 0)
		toluafix_get_function_by_refid(tolua_S, ${handler_name});
	else
		lua_pushnil(tolua_S);
#else
	handler_to_luafunction(tolua_S, query_luafunction_handler(tolua_S, 2, LUA_FIELDNAME));
#end if
	return 1;
}
int ${signature_name}_set${name}(lua_State* tolua_S)
{
	constexpr auto LUA_OBJ_TYPE = "${func_cobj_type}";
	constexpr auto LUA_FNAME = "${setter_lua_name}";
	constexpr auto LUA_FIELDNAME = "${field_lua_name}";
	bool ok = true;
	LUA_CHECK_COBJ_TYPE(tolua_S, LUA_OBJ_TYPE, LUA_FNAME);
	auto cobj = (${namespaced_class_name}*)tolua_tousertype(tolua_S, 1, 0);
	LUA_CHECK_COBJ(tolua_S, cobj, LUA_FNAME);
	const int argc = lua_gettop(tolua_S) - 1;
	if (1 == argc) {
		${ntype.to_string($generator)} arg0;
#if $generator.convert_legacy
		${ntype.to_native({"generator": $generator,
							"arg_idx": 2,
							"out_value": "arg0",
							"lua_namespaced_class_name": $generator.scriptname_from_native($namespaced_class_name, $namespace_name),
							"func_name": "LUA_FNAME",
							"scriptname": $generator.scriptname_from_native($ntype.namespaced_name, $ntype.namespace_name),
							"level": 2,
							"arg":$ntype,
					  })};
		if (ok) {
			if (${handler_name} != 0)
				toluafix_remove_function_by_refid(tolua_S, ${handler_name});
			${handler_name} = handler0;
		}
#else
		ok &= luaval_to_native(tolua_S, 2, &arg0, LUA_FIELDNAME);
#end if
		LUA_CHECK_PARAMETER(tolua_S, ok, LUA_FNAME);
		cobj->$pretty_name = arg0;
		return 0;
	}
	LUA_PARAMETER_ERROR(tolua_S, LUA_FNAME, argc, "1");
}
