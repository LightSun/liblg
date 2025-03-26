#include "structs.h"

using namespace h7l;

namespace h7l {
// 常见类型映射表（64位系统）
static const std::unordered_map<std::string, TypeInfo> sBase_type_map = {
    {"char",    {1, 1}},
    {"uchar",   {1, 1}},
    {"short",   {2, 2}},
    {"int",     {4, 4}},
    {"uint",    {4, 4}},
    {"float",   {4, 4}},
    {"double",  {8, 8}},
    {"long",    {8, 8}},
    {"void*",   {8, 8}}
};

static bool parse_array_type(const std::string& type_name, std::string& base_type,
                             size_t& count) {
    size_t bracket_pos = type_name.find('[');
    if (bracket_pos == std::string::npos) return false;

    size_t end_pos = type_name.find(']', bracket_pos);
    if (end_pos == std::string::npos || end_pos != type_name.length()-1) return false;

    base_type = type_name.substr(0, bracket_pos);
    std::string count_str = type_name.substr(bracket_pos+1, end_pos-bracket_pos-1);

    try {
        count = stoul(count_str);
    } catch (...) {
        return false;
    }
    return true;
}
}

StructLayout h7l::compute_struct_layout(const std::vector<std::string>& member_types,
                                std::function<TypeInfo(const std::string&, int)> func){
    StructLayout layout;
    size_t current_offset = 0;
    size_t max_alignment = 0;

    for (const auto& type_name : member_types) {
        TypeInfo type_info;
        std::string base_type;
        size_t array_count = 0;

        // 处理数组类型
        if (parse_array_type(type_name, base_type, array_count)) {
            auto it = sBase_type_map.find(base_type);
            TypeInfo baseTi;
            if (it == sBase_type_map.end()) {
                baseTi = func(base_type, array_count);
            }else{
                baseTi = it->second;
            }
            type_info.size = baseTi.size * array_count;
            type_info.alignment = baseTi.alignment;
        }
        // 处理普通类型-NonArr
        else {
            auto it = sBase_type_map.find(type_name);
            if (it == sBase_type_map.end()) {
                type_info = func(type_name, 1);
            }else{
                type_info = it->second;
            }
        }

        // 计算填充字节（当前偏移按对齐要求对齐）
        size_t padding = (type_info.alignment - (current_offset % type_info.alignment)) % type_info.alignment;

        // 记录成员布局信息
        layout.members.push_back({
            current_offset + padding,
            type_info.size,
            type_info.alignment,
            padding
        });

        // 更新偏移和最大对齐值
        current_offset += padding + type_info.size;
        max_alignment = std::max(max_alignment, type_info.alignment);
    }

    // 计算结构体总大小（包含末尾填充）
    layout.alignment = max_alignment;
    size_t total_used = current_offset;
    size_t final_padding = (max_alignment - (total_used % max_alignment)) % max_alignment;

    layout.total_size = total_used + final_padding;
    layout.final_padding = final_padding;

    return layout;
}
