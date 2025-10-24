#include "runtime/utils/ConstantPool.h"


namespace h7l { namespace runtime {

// 故意产生哈希冲突的测试字符串
static inline std::vector<std::string> generateCollidingStrings(int count) {
    std::vector<std::string> result;

    // 这些字符串经过设计会产生哈希冲突
    result.push_back("Aa");
    result.push_back("BB");

    // 更多可能产生冲突的字符串
    for (int i = 0; i < count - 2; ++i) {
        result.push_back("Test_" + std::to_string(i));
    }

    return result;
}
static void testHashCollisions() {
    std::cout << "=== 哈希冲突测试 ===" << std::endl;

    ConstantPool pool;

    // 测试1：故意产生冲突的字符串
    auto colliding_strings = generateCollidingStrings(10);

    std::cout << "\n测试故意冲突的字符串:" << std::endl;
    for (const auto& str : colliding_strings) {
        size_t index = pool.addString(str);
        std::cout << "添加字符串: '" << str << "' -> 索引: " << index << std::endl;
    }

    // 测试2：大量随机字符串
    std::cout << "\n测试大量随机字符串:" << std::endl;
    const int LARGE_TEST_COUNT = 1000;
    std::vector<size_t> indices;

    for (int i = 0; i < LARGE_TEST_COUNT; ++i) {
        std::string random_str = "RandomString_" + std::to_string(rand() % 100);
        indices.push_back(pool.addString(random_str));
    }

    // 测试3：重复字符串
    std::cout << "\n测试重复字符串:" << std::endl;
    size_t first_index = pool.addString("DuplicateString");
    size_t second_index = pool.addString("DuplicateString");
    std::cout << "第一次添加索引: " << first_index << std::endl;
    std::cout << "第二次添加索引: " << second_index << std::endl;
    std::cout << "是否相同: " << (first_index == second_index ? "是" : "否") << std::endl;

    // 显示统计信息
    pool.getStats();
}

}}

// ==================== 主函数 ====================
void main_test_const_pool2() {
    using namespace h7l::runtime;
    std::cout << "=== 改进的高性能常量池：哈希冲突处理 ===" << std::endl;

    // 运行哈希冲突测试
    testHashCollisions();

    // 基本功能测试
    std::cout << "\n\n=== 基本功能测试 ===" << std::endl;

    ConstantPool pool;

    // 添加各种类型的常量
    pool.addNil();
    pool.addBoolean(true);
    pool.addBoolean(false);
    pool.addInteger(42);
    pool.addDouble(3.14159);
    pool.addString("Hello, World!");
    pool.addString("Hello, World!"); // 重复
    pool.addString("Different String");

    std::cout << "基本测试完成" << std::endl;
    pool.getStats();
}
