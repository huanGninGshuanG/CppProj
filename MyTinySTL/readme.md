# 记录
主要阅读了开源库[MyTinySTL](https://github.com/Alinshans/MyTinySTL)
## 迭代器
* iterator种类
```c++
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
```
* iterator_traits相关内容
  1. 首先定义迭代器模板
  ```c++
    template <class Category, class T, class Distance = ptrdiff_t,
    class Pointer = T*, class Reference = T&>
    struct iterator {
        typedef Category                             iterator_category;
        typedef T                                    value_type;
        typedef Pointer                              pointer;
        typedef Reference                            reference;
        typedef Distance                             difference_type;
    };
  ```
  2. 判断一个类型是否是迭代器类型
  ```c++
    template <class T>
    struct has_iterator_cat {
    private:
        struct two { char a; char b; };
        template <class U> static two test(...);
        template <class U> static char test(typename U::iterator_category* = 0);
    public:
        static const bool value = sizeof(test<T>(0)) == sizeof(char);
    }; 
  ```
  这里利用了函数的重载，如果`U::iterator_category`是一个类型，就能重载到第二个`test`函数，同时这里不需要实现`test`函数
  3. iterator_traits的实现
  首先`iterator_traits_helper`实现了完成了具体的实现，如果第二个模板参数类型为true就定义了`iterator`结构体中的五个类型。
  ```c++
  template <class Iterator, bool> struct iterator_traits_impl {};
    
  template <class Iterator>
    struct iterator_traits_impl<Iterator, true>{
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type        value_type;
        typedef typename Iterator::pointer           pointer;
        typedef typename Iterator::reference         reference;
        typedef typename Iterator::difference_type   difference_type;
    };
  ```
  ```c++
  template <class Iterator, bool>
    struct iterator_traits_helper {};
    
    template <class Iterator>
    struct iterator_traits_helper<Iterator, true>
    : public iterator_traits_impl<Iterator,
        std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
        std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>{
    };
  ```
  ```c++
  template <class Iterator>
    struct iterator_traits
    : public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};
  ```
  同时`iterator_traits`还对指针完成了偏特化版本。
  4. 通过萃取完成静态多态
  ```c++
  template <class InputIterator>
  typename iterator_traits<InputIterator>::difference_type
  distance(InputIterator first, InputIterator last){
    return distance_dispatch(first, last, iterator_category(first));
  }
  
  template <class RandomIter>
  typename iterator_traits<RandomIter>::difference_type
  distance_dispatch(RandomIter first, RandomIter last,
    random_access_iterator_tag) {
    return last - first;
  }
  ```
  上面只列举出了`random_access_iterator_tag`的版本。