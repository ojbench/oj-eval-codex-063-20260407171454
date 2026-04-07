# Problem 063 - MemoryRiver

**ACMOJ Problem ID**: 1337

## Table of Contents

- [Problem 063 - MemoryRiver](#problem-063-memoryriver)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
    - [Background](#background)
  - [Assignment Description](#assignment-description)
    - [Grade Composition](#grade-composition)
  - [Assignment Requirements](#assignment-requirements)
    - [Input Format](#input-format)
    - [Output Format](#output-format)
    - [Samples](#samples)
    - [Data Constraints](#data-constraints)
  - [Per-Testcase Resource Limits](#per-testcase-resource-limits)
  - [Test Data](#test-data)
  - [Submission Requirements](#submission-requirements)
    - [OJ Git Repository Compilation Process](#oj-git-repository-compilation-process)
    - [Git Configuration Requirements](#git-configuration-requirements)
    - [Submission Guidelines](#submission-guidelines)
    - [Evaluation Notes](#evaluation-notes)
    - [Academic Integrity](#academic-integrity)

## Introduction

### Background

Implement file-based storage class with space reclamation. Read/write objects to binary file with index management.

## Assignment Description

### Grade Composition

| Grading Component | Percentage |
| :--: | :--: |
| Pass **1337. MemoryRiver** | 80% |
| Code Review | 20% |

Here are several points that need clarification:

- In the Code Review, we will **strictly examine your code style and repository organization structure, etc.**

- This assignment provides some sample data for testing, stored in the `/workspace/data/063/data_test/` directory. Note that these are not the test cases on the Online Judge. Passing all local test cases does not guarantee that you will pass the OJ tests.

- Besides the provided sample data, we also encourage you to design your own test data based on your program logic to assist debugging.

## Assignment Requirements

### Problem Description

## Description

### 本题中，需要实现一个包含空间回收的文件读写类

> Where the north wind meets the sea
>
> There's a river full of memory
>
> Sleep, my darling, safe and sound
>
> For in this river all is found

—— *All is Found*

文件存储操作是编程的重要部分，其分为ASCII文件存储和二进制文件存储。文件其实是一块白纸，交由你创作出块状、链状、树状的画作，以及本题中需要实现的`MemoryRiver`类。

`MemoryRiver`类是为了方便对文件的操作而封装了一些操作。该类的功能有

* 与一个文件名（string）绑定
* 往文件中写入一个`int`或`T`类型对象
* 往文件中读取一个`int`或`T`类型对象（T的大小大于int）
* 删除并回收一块空间。

请实现一个类模板`MemoryRiver.hpp`，来实现文件的读写。

该类模板的声明为

```c++
template<class T, int info_len = 2>
class MemoryRiver;
```

其中，`T`是需要存储的原子对象的类，`info_len`是存储文件头部预留出来的`int`的个数（1_base）。

`MemoryRiver`的存储策略为在文件首预留出 `info_len` 个`int`作为存储一些参数的空间，这些`int`在`initialise()`函数种被初始化为0。

请完成`MemoryRiver.hpp`内的代码：

### MemoryRiver.hpp

```c++
#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T, int info_len = 2>
class MemoryRiver {
private:
    /* your code here */
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);
public:
    MemoryRiver() = default;

    MemoryRiver(const string& file_name) : file_name(file_name) {}

    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::out);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

	//读出第n个int的值赋给tmp，1_base
    void get_info(int &tmp, int n) {
        if (n > info_len) return;
        /* your code here */
    }

	//将tmp写入第n个int的位置，1_base
    void write_info(int tmp, int n) {
        if (n > info_len) return;
        /* your code here */
    }
    
	//在文件合适位置写入类对象t，并返回写入的位置索引index
    //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
    //位置索引index可以取为对象写入的起始位置
    int write(T &t) {
        /* your code here */
    }
    
    //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
    void update(T &t, const int index) {
        /* your code here */
    }
	
    //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
    void read(T &t, const int index) {
        /* your code here */
    }
	
    //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
    void Delete(int index) {
        /* your code here */
    }
};


#endif //BPT_MEMORYRIVER_HPP
```

tips:

二进制文件的读写(可参考https://en.cppreference.com/w/cpp/io/basic_fstream 等资料)

```c++
//T类型的对象t的二进制文件读写
file.write(reinterpret_cast<char *>(&t), sizeof(T));
file.read(reinterpret_cast<char *>(&t), sizeof(T));
//将指针移动到[文件头指针+offset]处
file.seekp(offset);
```

可以使用文件（多）链表的思想来实现空间回收。

也可以用类似内存池的思想，多开一个文件进行管理。

可以隐式的增加文件头部的info_len的长度供自己使用，但是需要在get_info和write_info时作一个转换的映射避免冲突

本题未对原子化操作做出要求，但在大作业中推荐在每个函数调用前打开文件，调用后关闭文件。

**评测的文件数量限制为20，但最好不要超过2个文件**

$2\le infolen$

### Input Format

See the problem description above.

### Output Format

See the problem description above.

### Samples

No sample data provided for this problem.

### Data Constraints

See the problem description for constraints.

## Per-Testcase Resource Limits

- **Time Limit (per test case)**: 5000 ms
- **Memory Limit (per test case)**: 50 MiB
- **Disk Usage**: No disk usage is permitted.

## Test Data

The test data for this problem is located at `/workspace/data/063/data_test/`.

## Submission Requirements

### OJ Git Repository Compilation Process

For Git compilation, we will first clone the repository using a command similar to:
```bash
git clone <repo_url> . --depth 1 --recurse-submodules --shallow-submodules --no-local
```

Then we check if there is a `CMakeLists.txt` file. If it exists, we run (if not, a warning message will be displayed):
```bash
cmake .
```

Finally, we check if there is any of `GNUmakefile`/`makefile`/`Makefile` (if cmake was run previously, this will be the generated Makefile). If it exists, we run (if not, a warning message will be displayed):
```bash
make
```

After this process is complete, we will use the `code` file in the project root directory as the compilation result.

A `CMakeLists.txt` file is provided in the project. You can use or modify it as needed. The local environment has gcc-13 and g++-13 available.

### Git Configuration Requirements

**IMPORTANT**: You must create a `.gitignore` file in your project root directory to avoid OJ evaluation conflicts.

The `.gitignore` file should include at least the following entries:

```gitignore
CMakeFiles/
CMakeCache.txt
```

### Submission Guidelines

- The submitted code must be able to compile successfully through the above compilation process
- The compiled executable file name must be `code`
- The program needs to be able to read data from standard input and write results to standard output
- Please ensure the code runs correctly within the given time and space limits
- **You must use C++ or C language** to implement this assignment

### Evaluation Notes

- The evaluation system will test your program using the provided test data
- The program output must exactly match the expected output (including format)
- Exceeding time or memory limits will be judged as the corresponding error type
- Please pay attention to the overall time performance of your code and the time complexity of each part of your algorithm.

### Academic Integrity

If any violations are found during evaluation or code review (including but not limited to using unconventional methods to pass test cases), your final score may be significantly reduced or become **0 points**.
