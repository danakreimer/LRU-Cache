#include <iostream>
#include <stdio.h>
#include <string.h>
#include <list>
#include <functional>
#include <unordered_map>
#include <fstream>

#ifndef EX2_EX2_H
#define EX2_EX2_H

using namespace std;

template<class T>
class CacheManager {
private:
    unordered_map<string, pair<T, list<string>::iterator>> cacheMap;
    unsigned int size;
    list<string> orderKeys;

    bool doesFileExist(const std::string &filename) {
        ifstream file;
        file.open(filename);
        bool doesExist = file.good();
        file.close();
        return doesExist;
    }

    void write(string key, T obj) {
        ofstream file;
        string fileName = T::class_name + key;
        file.open(fileName, ios::binary);
        if (file.is_open()) {
            file.write((char *) &obj, sizeof(obj));
        } else {
            throw "could not open file";
        }

        file.close();
    }

    T read(string key) {
        ifstream file;
        string fileName = T::class_name + key;
        T obj;

        file.open(fileName, ios::binary);
        if (file.is_open()) {
            file.read((char *) &obj, sizeof(obj));
        } else {
            throw "could not open file";
        }

        file.close();
        return obj;
    }

public:
    CacheManager(int _size) {
        this->size = _size;
    }

    void insert(string key, T obj) {
        if (cacheMap.count(key) > 0) {
            orderKeys.erase(cacheMap[key].second);
            orderKeys.push_front(key);
        } else {
            orderKeys.push_front(key);
            if (orderKeys.size() > this->size) {
                string keyToDelete = orderKeys.back();
                cacheMap.erase(keyToDelete);
                orderKeys.pop_back();
            }
        }

        cacheMap[key] = make_pair(obj, orderKeys.begin());
        write(key, obj);
    }

    T get(string key) {
        T obj;
        string filename = T::class_name + key;
        if (cacheMap.count(key) > 0) {
            obj = cacheMap[key].first;
            orderKeys.erase(cacheMap[key].second);
            orderKeys.push_front(key);
        } else if (doesFileExist(filename)) {
            obj = read(key);
            orderKeys.push_front(key);
            if (orderKeys.size() > this->size) {
                string keyToDelete = orderKeys.back();
                cacheMap.erase(keyToDelete);
                orderKeys.pop_back();
            }
        } else {
            throw "object doesnt exist in cache or files";
        }

        cacheMap[key] = make_pair(obj, orderKeys.begin());
        return obj;
    }

    void foreach(const function<void(T &)> &func) {
        for (string key : orderKeys) {
            func(cacheMap[key].first);
        }
    }
};

#endif //EX2_EX2_H
