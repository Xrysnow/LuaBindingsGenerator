import re
from clang import cindex

LUA_KEYWORDS = [
    'and', 'break', 'do', 'else', 'elseif', 'end',
    'false', 'for', 'function', 'goto', 'if', 'in',
    'local', 'nil', 'not', 'or', 'repeat', 'return',
    'then', 'true', 'until', 'while',
]

LUA_TYPE_MAP = {
    "int": "number",
    "unsigned int": "number",
    "short": "number",
    "unsigned short": "number",
    "long": "number",
    "unsigned long": "number",
    "long long": "number",
    "unsigned long long": "number",
    "char": "number",
    "unsigned char": "number",
    "float": "number",
    "double": "number",
    "bool": "boolean",
    "const char*": "string",
    "const char *": "string",
    "char*": "string",
    "char *": "string",
    "cocos2d::Vec2": "vec2_table",
    "cocos2d::Vec3": "vec3_table",
    "cocos2d::Vec4": "vec4_table",
    "cocos2d::Vector": "array_table",
    "cocos2d::Mat4": "mat4_table",
    "cocos2d::Map": "map_table",
    "cocos2d::Point": "point_table",
    "cocos2d::Size": "size_table",
    "cocos2d::Rect": "rect_table",
    "cocos2d::Color3B": "color3b_table",
    "cocos2d::Color4B": "color4b_table",
    "cocos2d::Color4F": "color4f_table",
    "std::string": "string",
    "std::string_view": "string",
}

NS_MAP = {
    "cocos2d::experimental::ui::": "ccexp.",
    "cocos2d::experimental::": "ccexp.",
    "cocos2d::extension::": "cc.",
    "cocos2d::ui::": "ccui.",
    "cocos2d::": "cc.",
    "spine::": "sp.",
    "cocostudio::timeline::": "ccs.",
    "cocostudio::": "ccs.",
    "cocosbuilder::": "cc.",
    "CocosDenshion::": "cc.",
    "cocos2d::tweenfunc::": "cc.",
    "cocos2d::backend::": "ccb.",
    "lstg::": "lstg.",
    "l2d::": "l2d.",
    "xmath::": "xmath.",
    "xmath::collision::": "xmath.",
    "xmath::random::": "xmath.",
    "lstg::symbol::": "lstg_sym.",
    "imgui::": "imgui.",
    "audio::": "audio.",
    "video::": "video.",
    "ImGui::": "imgui.",
    "implot::": "implot.",
    "fairygui::": "fgui.",
}

stl_type_map = {
    'std_function_args': 1000,
    'std::unordered_map': 2,
    'std::unordered_multimap': 2,
    'std::map': 2,
    'std::multimap': 2,
    'std::vector': 1,
    'std::list': 1,
    'std::forward_list': 1,
    'std::priority_queue': 1,
    'std::set': 1,
    'std::multiset': 1,
    'std::unordered_set': 1,
    'std::unordered_multiset': 1,
    'std::stack': 1,
    'std::queue': 1,
    'std::deque': 1,
    'std::array': 1,

    'unordered_map': 2,
    'unordered_multimap': 2,
    'map': 2,
    'multimap': 2,
    'vector': 1,
    'list': 1,
    'forward_list': 1,
    'priority_queue': 1,
    'set': 1,
    'multiset': 1,
    'unordered_set': 1,
    'unordered_multiset': 1,
    'stack': 1,
    'queue': 1,
    'deque': 1,
    'array': 1
}


def find_sub_string_count(s, start, end, substr):
    count = 0
    pos = s.find(substr, start, end)
    if pos != -1:
        next_count = find_sub_string_count(s, pos + 1, end, substr)
        count = next_count + 1
    return count


def split_container_name(name):
    name = name.strip()
    left = name.find('<')
    right = -1

    if left != -1:
        right = name.rfind('>')

    if left == -1 or right == -1:
        return [name]

    first = name[:left]
    results = [first]

    comma = name.find(',', left + 1, right)
    if comma == -1:
        results.append(name[left + 1:right].strip())
        return results

    left += 1
    while comma != -1:
        lt_count = find_sub_string_count(name, left, comma, '<')
        gt_count = find_sub_string_count(name, left, comma, '>')
        if lt_count == gt_count:
            results.append(name[left:comma].strip())
            left = comma + 1
        comma = name.find(',', comma + 1, right)

    if left < right:
        results.append(name[left:right].strip())
    name_len = len(name)
    if right < name_len - 1:
        results.append(name[right + 1:].strip())

    return results


def normalize_type_name_by_sections(sections):
    container_name = sections[0]
    suffix = ''

    index = len(sections) - 1
    while sections[index] == '*' or sections[index] == '&':
        suffix += sections[index]
        index -= 1

    name_for_search = container_name.replace('const ', '').replace('&', '').replace('*', '').strip()
    if name_for_search in stl_type_map:
        normalized_name = container_name + '<' + ', '.join(sections[1:1 + stl_type_map[name_for_search]]) + '>' + suffix
    else:
        normalized_name = container_name + '<' + ', '.join(sections[1:]) + '>'

    return normalized_name


def normalize_std_function_by_sections(sections):
    normalized_name = ''
    if sections[0] == 'std_function_args':
        normalized_name = '(' + ', '.join(sections[1:]) + ')'
    elif sections[0] == 'std::function' or sections[0] == 'function':
        normalized_name = 'std::function<' + sections[1] + ' ' + sections[2] + '>'
    else:
        assert False
    return normalized_name


def normalize_type_str(s, depth=1):
    if s.find('std::function') == 0 or s.find('function') == 0:
        start = s.find('<')
        assert (start > 0)
        sections = [s[:start]]  # std::function
        start += 1
        ret_pos = s.find('(', start)
        sections.append(s[start:ret_pos].strip())  # return type
        end = s.find(')', ret_pos + 1)
        sections.append('std_function_args<' + s[ret_pos + 1:end].strip() + '>')
    else:
        sections = split_container_name(s)
    section_len = len(sections)
    if section_len == 1:
        return sections[0]

    # for section in sections:
    #     print('>' * depth + section)

    if sections[0] == 'const std::basic_string' or sections[0] == 'const basic_string':
        last_section = sections[len(sections) - 1]
        if last_section == '&' or last_section == '*' or last_section.startswith('::'):
            return 'const std::string' + last_section
        else:
            return 'const std::string'

    elif sections[0] == 'std::basic_string' or sections[0] == 'basic_string':
        last_section = sections[len(sections) - 1]
        if last_section == '&' or last_section == '*' or last_section.startswith('::'):
            return 'std::string' + last_section
        else:
            return 'std::string'

    for i in range(1, section_len):
        sections[i] = normalize_type_str(sections[i], depth + 1)

    if sections[0] == 'std::function' or sections[0] == 'function' or sections[0] == 'std_function_args':
        normalized_name = normalize_std_function_by_sections(sections)
    else:
        normalized_name = normalize_type_name_by_sections(sections)
    return normalized_name


def lua_typename_from_natve(script_ns_dict, namespace_class_name: str, is_ret=False, nt=None):
    # script_ns_dict = self.config['conversions']['ns_map']
    ncn = namespace_class_name
    ncn = re.sub('^const ', '', ncn)
    ncn = re.sub('&$', '', ncn)
    arr_type = ['std::array<', 'std::vector<', 'std::set<', 'std::unordered_set<', 'cocos2d::Vector<']
    for ty in arr_type:
        if ncn.startswith(ty):
            # array type
            element_t = ncn[len(ty):-1]
            if element_t in LUA_TYPE_MAP:
                return LUA_TYPE_MAP[element_t] + '[]'
            element_t = lua_typename_from_natve(script_ns_dict, element_t, is_ret, nt)
            if element_t != 'void':
                return element_t + '[]'
            return "array_table"
    map_type = ['std::map<', 'std::unordered_map<', 'cocos2d::Map<']
    for ty in map_type:
        if ncn.startswith(ty):
            # map type
            e = ncn[len(ty):-1]
            e = e.split(',')
            if len(e) == 2:
                e0, e1 = e[0].strip(), e[1].strip()
                if e0 and e1:
                    if e0 in LUA_TYPE_MAP and e1 in LUA_TYPE_MAP:
                        return "table<" + LUA_TYPE_MAP[e0] + "," + LUA_TYPE_MAP[e1] + ">"
                    e0 = lua_typename_from_natve(script_ns_dict, e0, is_ret, nt)
                    e1 = lua_typename_from_natve(script_ns_dict, e1, is_ret, nt)
                    return "table<{}, {}>".format(e0, e1)
            return "map_table"
    map_type2 = ["hlookup::string_map<"]
    for ty in map_type2:
        if ncn.startswith(ty):
            e = ncn[len(ty):-1]
            if e in LUA_TYPE_MAP:
                return "table<string, {}>".format(e)
            e = lua_typename_from_natve(script_ns_dict, e, is_ret, nt)
            return "table<string, {}>".format(e)
    if ncn.startswith("std::string"):
        return "string"
    if ncn.startswith("std::basic_string<"):
        return "string"
    if ncn.startswith("std::pair<"):
        return "table"
    if ncn.startswith("std::tuple<"):
        return "table"
    if ncn.startswith("std::function<"):
        if nt is not None and nt.is_function:
            ret_str = ""
            param_strs = []
            if str(nt.ret_type) != "void":
                ret_str = lua_typename_from_natve(script_ns_dict, nt.ret_type.namespaced_name)
                ret_str = ":" + ret_str
            for i in range(len(nt.param_types)):
                param = nt.param_types[i]
                param_str = lua_typename_from_natve(script_ns_dict, param.namespaced_name)
                if len(nt.param_types) == 1 and param_str == 'void':
                    break
                param_strs.append("arg" + str(i) + ":" + param_str)
            return "fun(" + ",".join(param_strs) + ")" + ret_str
        return "function"
    if ncn in LUA_TYPE_MAP:
        return LUA_TYPE_MAP[ncn]
    for (k, v) in script_ns_dict.items():
        if k in ncn:
            if is_ret:
                return ncn.replace("*", "").replace("const ", "").replace(k, v)
            else:
                return ncn.replace("*", "").replace("const ", "").replace(k, v)
    return ncn.replace("*", "").replace("const ", "")
