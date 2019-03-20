#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <memory>

// Define some policy classes for Creator policy
template <class T>
struct OpNewCreator {
    static T* Create() {
        std::cout << "create use NEW operator" << std::endl;
        return new T;
    }
protected:
    ~OpNewCreator() {}
};

template <class T>
struct OpMallocCreator {
    static T* Create() {
        std::cout << "create use MALLOC operator" << std::endl;
        T* v = (T*) std::malloc(sizeof(T));
        return v;
    }
protected:
    ~OpMallocCreator() {}
};

template <class T>
struct OtherCreator {
    OtherCreator() : v_(0) {};
    T* clone() {
        T* tmp = new T;
        std::copy(v_, v_ + sizeof(T), tmp);
        return tmp;
    }
    T* Create() {
        std::cout << "create use OTHER operator" << std::endl;
        return (v_) ? clone() : new T;
    }
protected:
    ~OtherCreator() {}
private:
    T* v_;
};

// In library code, define hosts (host classes).
// Such a class will either contain or inherit one of the policy classes.
struct Widget {
    std::string name;
    Widget() : name("") {
        std::cout << "create anonymous Widget" << std::endl;
    }
    Widget(const std::string& x) : name(x) {
        std::cout << "create Widget " << x << std::endl;
    }
    ~Widget() { std::cout << "dtor Widget " << name << std::endl; }
};

template <class CreationPolicy>
class WidgetManager : public CreationPolicy {
    std::unique_ptr<Widget> w_;
public:
    WidgetManager() : w_(CreationPolicy().Create()) {
        std::cout << "Simple Widget Manager use Creator policy" << std::endl;
    }
};

// Policy classes with Template template parameters
template <template <class> class CreationPolicy>
class WidgetManagerTTP : public CreationPolicy<Widget> {
    std::unique_ptr<Widget> w_;
public:
    WidgetManagerTTP() : w_(CreationPolicy<Widget>().Create()) {
        std::cout << "Template template parameters Widget Manager use Creator policy" << std::endl;
    }
};

void usingPolicy() {
    // In application code, the client chooses the desired policy.
    {
        typedef WidgetManager<OpNewCreator<Widget>> MyWidgetMngr;
        MyWidgetMngr a;
    }
    {
        typedef WidgetManagerTTP<OtherCreator> MyWidgetMngrTTP;
        MyWidgetMngrTTP b;
    }
}


int main() {
    usingPolicy();
    return 0;
}


/*
 *

* Policy classes with template member function: being better supported by older compilers, but harder to talk about, define, implement and use.

* Destructor of policy classes

WidgetManagerTTP<OtherCreator> wm;
OtherCreator<Widget>* p = &wm; // dubious, but legal
delete p;

Solution: define nonvirtual protected destructor for policy classes, no size or speed overhead.
struct OpNewCreator {
    template <class T>
    static T* Create() {
        return new T;
    }
protected:
    ~OpNewCreator() {}
}



*/
