#pragma once

#include <string>
#include <vector>
#include <regex>
#include "runtime/common_base.h"

namespace h7l { namespace runtime {

using String = std::string;
using CString = const std::string&;
template <typename T>
using List = std::vector<T>;

struct SimpleArrayParser{

    static inline bool getInt(CString str, int& val){
        try{
            val = std::stoi(str);
        }catch (std::invalid_argument&){
            return false;
        }catch (std::out_of_range&){
            return false;
        }catch (...) {
            return false;
        }
        return true;
    }
    static inline bool getFloat(CString str, float& val){
        try{
            val = std::stof(str);
        }catch (std::invalid_argument&){
            return false;
        }catch (std::out_of_range&){
            return false;
        }catch (...) {
            return false;
        }
        return true;
    }
    static inline bool getDouble(CString str, double& val){
        try{
            val = std::stod(str);
        }catch (std::invalid_argument&){
            return false;
        }catch (std::out_of_range&){
            return false;
        }catch (...) {
            return false;
        }
        return true;
    }
    struct Item{
        int start;
        int end;
        std::vector<String> elements;
        std::vector<Item> children;

        template<typename T, typename F>
        bool getElementsTo(std::vector<T>& vec, F func){
            if(elements.empty()){
                for(auto& c: children){
                    if(!c.getElementsTo(vec, func)){
                        return false;
                    }
                }
            }else{
                for(auto& e: elements){
                    T v;
                    if(func(e, v)){
                        vec.push_back(v);
                    }else{
                        return false;
                    }
                }
            }
            return true;
        }
        int getChildrenCnt()const{
            return children.size();
        }
        bool getShapes(List<int>& shapes){
            if(elements.empty()){
                if(!children.empty()){
                    shapes.push_back(children.size());
                    return children[0].getShapes(shapes);
                }else{
                    return false;
                }
            }else{
                shapes.push_back(elements.size());
                return elements.size() > 0;
            }
        }
    };
    static inline std::vector<String> split(CString pat, CString text){

        std::regex reg(pat);
        std::sregex_token_iterator first{text.begin(),
                                         text.end(), reg, -1}, last;
        return {first, last};
    }
    static inline String buildFeaStr0(int dimC){
        String feaStr;
        {
            for(int i = 0 ; i < dimC ; ++i){
                feaStr.append("]");
            }
            feaStr.append(",");
            for(int i = 0 ; i < dimC ; ++i){
                feaStr.append("[");
            }
        }
        return feaStr;
    }
    static String removeBlank(CString str0){
        String str;
        for(int i = 0 ; i < (int)str0.length(); ++i){
            if(str0.data()[i] == '\n' || str0.data()[i] == ' '){
                continue;
            }
#ifdef _WIN32
            if(str0.data()[i] == '\r' || str0.data()[i + 1] == '\n'){
                continue;
            }
#endif
            str.append(String(1, str0.data()[i]));
        }
        return str;
    }
    static inline int getArrayDimCnt(CString str){
        int dimC = 0;
        {
            int idx = str.find("[");
            while (idx >= 0) {
                ++ dimC;
                int next = str.find("[", idx + 1);
                if(next != idx + 1){
                    break;
                }
                idx = next;
            }
        }
        return dimC;
    }
    static Item parse(CString str,int dimC){
        Item root;
        root.start = str.find("[");
        root.end = str.rfind("]");
        if(dimC > 1){
            auto dim0Str = buildFeaStr0(dimC - 1);
            List<std::pair<int,int>> ps;
            int index = 1;
            while (true) {
                int idx = str.find(dim0Str, index);
                if(idx >= 0){
                    ps.emplace_back(index, idx + dimC - 1);
                    index = idx + dimC;
                }else{
                    break;
                }
            }
            for(auto& p : ps){
                auto subS = str.substr(p.first, p.second - p.first);
                auto item1 = parse(subS, dimC - 1);
                root.children.push_back(item1);
            }
            {   auto& p = ps.back();
                auto subS = str.substr(p.second + 1, str.length() - p.second - 2);
                auto item1 = parse(subS, dimC - 1);
                root.children.push_back(item1);
            }
        }else{
            auto subS = str.substr(1, str.length() - 2);
            root.elements = split(",", subS);
        }
        return root;
    }
    static bool parseInt(CString str,int dimC, List<int>& shapes,List<int>& out){
        return parseTo(str, dimC, shapes, out, [](CString s, int& v){
            return getInt(s,v);
        });
    }
    static bool parseFloat(CString str,int dimC, List<int>& shapes,List<float>& out){
        return parseTo(str, dimC, shapes, out, [](CString s, float& v){
            return getFloat(s,v);
        });
    }
    static bool parseDouble(CString str,int dimC, List<int>& shapes,List<double>& out){
        return parseTo(str, dimC, shapes, out, [](CString s, double& v){
            return getDouble(s,v);
        });
    }
    template<typename T, typename F>
    static bool parseTo(CString str,int dimC, List<int>& shapes,List<T>& out, F f){
        auto item = parse(str, dimC);
        if(item.getShapes(shapes)){
            auto ret = item.getElementsTo(out, f);
            if(ret){
                int cnt = 1;
                for(auto& s : shapes){
                    cnt *= s;
                }
                if(cnt == (int)out.size()){
                    return true;
                }
            }
        }
        return false;
    }
};

}}
