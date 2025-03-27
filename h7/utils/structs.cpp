#include "structs.h"

using namespace h7l;

namespace h7l {
// 常见类型映射表（64位系统）
static const std::unordered_map<std::string, TypeInfo> sBase_type_map = {
    {"char",    {sizeof(char), sizeof(char)}},
    {"uchar",   {sizeof(unsigned char), sizeof(unsigned char)}},
    {"short",   {sizeof(short), sizeof(short)}},
    {"int",     {sizeof(int), sizeof(int)}},
    {"uint",    {sizeof(uint), sizeof(uint)}},
    {"float",   {sizeof(float), sizeof(float)}},
    {"double",  {sizeof(double), sizeof(double)}},
    {"long",    {sizeof(int64_t), sizeof(int64_t)}},
    {"void*",   {sizeof(void*), sizeof(void*)}}
};

static inline int parse_dim(const std::string& type_name, int startIdx){
    size_t bracket_pos = type_name.find('[', startIdx);
    if (bracket_pos == std::string::npos) return -1;

    size_t end_pos = type_name.find(']', bracket_pos);
    if (end_pos == std::string::npos || end_pos != type_name.length()-1) return -1;
    std::string count_str = type_name.substr(bracket_pos+1, end_pos-bracket_pos-1);
    try {
        return std::stoi(count_str);
    } catch (...) {
        return -1;
    }
    return -1;
}

static bool parse_array_type(const std::string& type_name, std::string& base_type,
                             size_t& count, std::vector<int>& shapes) {
    size_t bracket_pos = type_name.find('[');
    if (bracket_pos == std::string::npos) return false;
    base_type = type_name.substr(0, bracket_pos);
    //
    int startIdx = 0;
    while (true) {
        int dim = parse_dim(type_name, startIdx);
        if(dim > 0){
            shapes.push_back(dim);
        }else{
            break;
        }
    }
    size_t cnt = 1;
    for(auto& sp : shapes){
        cnt *= sp;
    }
    count = cnt;
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
        std::vector<int> shapes;

        // 处理数组类型
        if (parse_array_type(type_name, base_type, array_count, shapes)) {
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
            padding,
            shapes
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
StructLayout h7l::compute_struct_layout2(const std::vector<MemberInfo0>& infos,
                        std::function<TypeInfo(const std::string&, int)> func){
    StructLayout layout;
    size_t current_offset = 0;
    size_t max_alignment = 0;
    for (const auto& info : infos) {
        TypeInfo base_info;
        {
            auto it = sBase_type_map.find(info.baseType);
            if (it == sBase_type_map.end()) {
                base_info = func(info.baseType, 1);
            }else{
                base_info = it->second;
            }
        }
        TypeInfo type_info;
        if(!info.shapes.empty()){
            type_info.size = base_info.size * info.getElementCount();
            type_info.alignment = base_info.alignment;
        }else{
            type_info = base_info;
        }

        // 计算填充字节（当前偏移按对齐要求对齐）
        size_t padding = (type_info.alignment - (current_offset % type_info.alignment)) % type_info.alignment;

        // 记录成员布局信息
        layout.members.push_back({
            current_offset + padding,
            type_info.size,
            type_info.alignment,
            padding,
            info.shapes
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
