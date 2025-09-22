#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
//#include <cctype>
#include <functional>

namespace h7l {

// 类型信息结构体，包含类型大小和对齐要求
struct TypeInfo {
    size_t size;
    size_t alignment;
};

struct MemberInfo0{
    std::string baseType;
    std::vector<int> shapes;

    int getElementCount()const{
        int cnt = 1;
        for(auto& shape : shapes){
            cnt *= shape;
        }
        return cnt;
    }
};

struct MemberLayout {
    size_t offset;
    size_t size;
    size_t alignment;
    size_t padding_before;
    std::vector<int> shapes;
};

struct StructLayout {
    std::vector<MemberLayout> members;
    size_t total_size;
    size_t alignment;
    size_t final_padding;
};

static void print_layout(const StructLayout& layout) {
    std::cout << "结构体总大小：" << layout.total_size << " 字节\n";
    std::cout << "结构体对齐要求：" << layout.alignment << " 字节\n";
    std::cout << "末尾填充：" << layout.final_padding << " 字节\n\n";

    for (size_t i = 0; i < layout.members.size(); ++i) {
        const auto& m = layout.members[i];
        std::cout << "成员 " << i+1 << ":\n"
             << "  类型大小：" << m.size << " 字节\n"
             << "  对齐要求：" << m.alignment << " 字节\n"
             << "  偏移地址：" << m.offset << "\n"
             << "  前部填充：" << m.padding_before << " 字节\n"
             << std::endl;
    }
}

//extra func to parse type.
StructLayout compute_struct_layout(const std::vector<std::string>& member_types,
                                   std::function<TypeInfo(const std::string&, int)> func);
StructLayout compute_struct_layout2(const std::vector<MemberInfo0>& infos,
                                   std::function<TypeInfo(const std::string&, int)> func);


}
