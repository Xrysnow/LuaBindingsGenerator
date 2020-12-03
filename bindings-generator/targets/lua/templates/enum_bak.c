#set fields = $current_class.fields
#set parents = $current_class.parents
#set fullname = $generator.scriptname_from_native($current_class.namespaced_class_name, $current_class.namespace_name)

int lua_register_${generator.prefix}_${current_class.class_name}(lua_State* tolua_S)
{
    #for p in parents
    tolua_module(tolua_S, "${p.class_name}", 0);
    tolua_beginmodule(tolua_S, "${p.class_name}");
    #end for

    tolua_module(tolua_S, "${current_class.class_name}", 0);
    tolua_beginmodule(tolua_S,"${current_class.class_name}");
    #for m in fields
        tolua_constant(tolua_S, "${m['name']}", ${fullname}::${m['name']});
    #end for
    tolua_endmodule(tolua_S);

    #for p in parents
    tolua_endmodule(tolua_S);
    #end for
    return 1;
}
