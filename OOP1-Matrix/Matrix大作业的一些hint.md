## 一些容易Memory Leak的地方

### 移动构造的指针

使用**移动构造**重载赋值时

```c++
Matrix &operator=(Matrix &&o) noexcept {
    //todo
}
```

由于需要接管o的空间，需要将`*this`的指针指向o的存储空间，此时应该先删除`*this`指向的存储空间里的内容，再让其指向o。

示例：

```c++
Matrix &operator=(Matrix &&o) noexcept {
            //use when "Matrix a; a=b;"
            matrix_column = o.matrix_column;
            matrix_row = o.matrix_row;
            delete[] matrix_storage;
            matrix_storage = o.matrix_storage;
            o.matrix_storage = nullptr;
            return *this;
        }
```



### Initializer_list的异常抛出

使用`Initializer_list`写Matrix的构造函数时

```c++
Matrix(std::initializer_list<std::initializer_list<T>> il) {
    //todo
}
```

由于需要判断`Initializer_list`是否每一行的元素个数都相等，而访问`Initializer_list`只能使用迭代器，因此需要在每个第二重循环开始判断元素个数是否与第一行的相同。而作为构造函数，new数组的操作应该在循环外完成，如果在循环内发现异常，抛出信息，代表此次构造失败，析构函数可能会被跳过。因此需要在抛出语句的上一行delete掉刚刚new的数组。

示例：

```c++
Matrix(std::initializer_list<std::initializer_list<T>> il) {
            size_t il_row_size = il.size();
            size_t il_column_size = (*il.begin()).size();
            matrix_column = il_column_size;
            matrix_row = il_row_size;
            matrix_storage = new T[matrix_row * matrix_column];
            size_t i = 0;
            for (auto iter_row = il.begin(); iter_row != il.end(); iter_row++) {
                if ((*iter_row).size() != il_column_size) {
                    delete[] matrix_storage;
                    throw std::invalid_argument("INVALID INITIALIZER_LIST!");
                }
                size_t j = 0;
                for (auto iter_column = (*iter_row).begin(); iter_column != (*iter_row).end(); iter_column++) {
                    matrix_storage[i * matrix_column + j] = *iter_column;
                    j++;
                }
                i++;
            }
        }
```



### return *this

据说有在应该`return *this`的地方没有return导致Memory Leak的问题，这个问题我没有遇到过所以不是很清楚……但是各位如果修复了上面的问题依旧Memory Leak，可以尝试检查`return *this`。







*Written by Rainy Memory*

