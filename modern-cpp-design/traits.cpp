#include <iostream>
#include <stdexcept>

using namespace std;

/**
 * Generic Progamming: Traits: The else-if-then of Types
 * url: https://erdani.com/publications/traits.html
 *
 * A trait template is a template class, possibly explicitly specialized, that provides a uniform symbolic interface over a coherent set of design choices that vary from one type to another.
 */

/**
 * Problem: pieces of code that, depending upon a type, variations in terms of strucutre and/or behavior.
 */
// Assume database API header
#define DB_INVALID 0
#define DB_INT 1
#define DB_CUR 2
typedef long int db_int;
typedef struct {
    int a,b;
} db_cur;

// Define DBTraits
template <class T> struct DBTraits; // Most general case not implimented.

template <>
struct DBTraits<int> {
    enum { TypeId = DB_INT };
    typedef db_int DbNativeType;
    static void Convert(DbNativeType from, int& to) {
        to = static_cast<int>(from);
    }
};
template<>
struct DBTraits<double> {
    enum { TypeId = DB_CUR };
    typedef db_cur DbNativeType;
    static void Convert(DbNativeType from, double &to) {
        to = from.a + from.b/10.0;
    }
};


template <class T>
void FetchValue(T& val) {
    typedef DBTraits<T> Traits;
    if (Traits::TypeId != DB_INVALID)
        throw std::runtime_error("Type mismatch or invalid");
    typename Traits::DbNativeType tmp;
    Traits::Convert(tmp, val);
}

/**
 * Traits as interface glue - universal non-intrusive adapter.
 * If various classes implement a given concept in slightly different ways, traits can fit those implementations to a common interface.
 */
// Most of case
struct RefCounted {
    int count;
    RefCounted() : count{0} {}
    ~RefCounted() {
        cout << "~RefCounted" << endl;
    }
    void IncRef() {
        cout << "Add Ref" << endl;
        count++;
    }
    void DecRef() {
        if (--count == 0) this->~RefCounted();
    }
};
// Thirdparty vendor slightly different
struct Widget {
    int count;
    Widget() : count{0} {}
    ~Widget() {
        cout << "~Widget" << endl;
    }
    void AddRef() {
        cout << "Add Widget" << endl;
        count++;
    }
    int DelRef() {
        return --count;
    }
};
// Use general template to support the RefCounted,
// and specialize for Widget.
template<class T>
struct RefCountedTraits {
    static void Refer(T* p) {
        p->IncRef();
    }
    static void Unrefer(T* p) {
        p->DecRef();
    }
};
template<>
struct RefCountedTraits<Widget> {
    static void Refer(Widget* p) {
        p->AddRef();
    }
    static void Unrefer(Widget* p) {
        if (p->DelRef() == 0)
            delete p;
    }
};

template<class T>
struct UsingRefCount {
    typedef RefCountedTraits<T> RefTraits;
    T* a;
    UsingRefCount() {
        a = new T;
        RefTraits::Refer(a);
    }
    ~UsingRefCount() {
        RefTraits::Unrefer(a);
    }
};

void run_interface_glue_traits() {
    UsingRefCount<Widget> a;
    UsingRefCount<RefCounted> b;
}


int main() {
    run_interface_glue_traits();
    return 0;
}
