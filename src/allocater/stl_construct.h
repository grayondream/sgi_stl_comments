/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */
 
 
/*
如果函数构造或者析构函数是trivial则意味着编译器只为对象分配相关的内存，而并不做具体的构造或者析构工作，否则会生成默认的构造函数或者扩展现有的构造函数，保证对一些特殊数据的初始化工作，如何判断，构造函数或者析构函数是trivial，如果输入如下四种情况则为non-trivial，反之为trivial:
1 带有default constructor的类成员：当当前类拥有一个成员变量，而该变量是一个拥有显式声明的default constructor的类时，编译器为了初始化该类会自动声明一个default constructor，并在自动生成的default constructor中调用该类的构造函数。
   1.1 如果当前类并未声明任何构造函数，则编译器声明一个default constructor，并在该构造函数中初始化成员类；
   1.2 如果当前类包含多个已经声明的构造函数，则编译器会扩张已经存在的每一个构造函数，并在用户代码之前调用类成员的default constructor;
2 如果当前类中包含多个类成员变量，则编译器会按照各个类成员声明的顺序在自动生成的构造函数中调用类成员的default constructor或者扩张已经存在的constructor;
3 带有default constructor的基类：如果当前类继承体系中，其祖先类中存在一个包含default constructor的类，则为了初始化该类编译器会生成default constructor并插入相关初始化代码，如果存在则选择扩张constructor;
4 带有virtual function的类：当前类包含一个虚函数或者继承自包含虚函数的类，即类最终包含一个虚函数表指针。编译器生成的default constuctor或者扩张constructor的目的是初始化虚函数表和虚函数指针，基本行为和上面的类似;
当前类虚继承自另一个类：虚继承涉及到了比较复杂的机制，他会在构造函数参数列表中生成一个bool类型变量，只有在顶层子类中，才会通过bool类型量实例化一次虚基类中的对象，以此实现虚基类机制。
*/



//定义了全局函数construct() 和 destroy(),负责对象的构造和析构
//他们隶属于STL标准规范
#ifndef __SGI_STL_INTERNAL_CONSTRUCT_H
#define __SGI_STL_INTERNAL_CONSTRUCT_H

#include <new.h>

__STL_BEGIN_NAMESPACE


/*
 * @brief 对象构造工具，对于trivial的构造函数的调用与否由编译器决定，因此不用像destroy那样定义多个版本
 */
template <class T1, class T2>
inline void 
construct(T1* p, const T2& value)
{  
	new (p) T1(value);
}

/*
 * @brief 原始版本接受一个指针，显示调用对象的析构函数
 */ 
template <class T>
inline void destroy(T* pointer) 
{  			
    pointer->~T();
}

/*
 * @brief 针对一组元素进行析构，如果该对象的析构函数是non-trivial的，则直接调用析构函数
 */
template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) 
{
	for ( ; first < last; ++first)
		destroy(&*first);
}

/*
 * @brief 针对一组元素进行析构，如果对象的析构函数是trivial的，则什么都不用做
 */
template <class ForwardIterator> 
inline void 
__destroy_aux(ForwardIterator, ForwardIterator, __true_type) {}


/*
 * @brief 对一组元素进行析构，在此判断该对象的析构函数是否为trivial的
 */
template <class ForwardIterator, class T>
inline void 
__destroy(ForwardIterator first, ForwardIterator last, T*) 
{
	typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
	__destroy_aux(first, last, trivial_destructor());
}


/* 
 * 针对一组对象进行析构，接受两个迭代器，此函数设法找出元素的数值型别，进而利用 __type_traits<>求取最适当措施
 */
template <class ForwardIterator>
inline void 
destroy(ForwardIterator first, ForwardIterator last) 
{
	__destroy(first, last, value_type(first));
}


/*
 * @brief 针对迭代器为char* 和 wchar_t* 的特化版，什么都不用做
 */
inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_CONSTRUCT_H */

// Local Variables:
// mode:C++
// End:
