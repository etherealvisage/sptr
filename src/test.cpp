#include <iostream>
#include <vector>

#include "sptr.h"

std::vector<int> events;

class TestObject {
private:
    int id;
    std::vector<sptr<TestObject>> srefs;
    std::vector<wptr<TestObject>> wrefs;
public:
    TestObject(int id) : id(id) {
        events.push_back(id);
    }
    ~TestObject() {
        events.push_back(-id);
    }

    void addRef(sptr<TestObject> ref) {
        srefs.push_back(ref);
    }
    void addRef(wptr<TestObject> ref) {
        wrefs.push_back(ref);
    }

    void testfunc() {
        std::cout << "testing object " << id << std::endl;
    }
};

void expect(const std::vector<int> &expected) {
    if(events != expected) {
        std::cout << "Test failed" << std::endl;
        std::cout << "\thad:";
        for(auto event : events) std::cout << " " << event;
        std::cout << std::endl;
        std::cout << "\texpected:";
        for(auto event : expected) std::cout << " " << event;
        std::cout << std::endl;
    }
}

int main() {
    {
        events.clear();

        sptr<TestObject> to = sptr<TestObject>::make(new TestObject(1));
    } expect({1,-1});

    {
        events.clear();

        sptr<TestObject> t = sptr<TestObject>::make(new TestObject(1));
        sptr<TestObject> t2 = sptr<TestObject>::make(new TestObject(2));
        t->addRef(t2);
        t2 = sptr<TestObject>::make(new TestObject(3));
        t->addRef(t2);
    } expect({1,2,3,-1,-2,-3});


    {
        events.clear();

        sptr<TestObject> t = sptr<TestObject>::make(new TestObject(1));
        sptr<TestObject> t2 = sptr<TestObject>::make(new TestObject(2));
        t->addRef(t2);
        t2 = sptr<TestObject>::make(new TestObject(3));
        t->addRef(t2);
        t2->addRef(t);
    } expect({1,2,3});

    {
        events.clear();

        sptr<TestObject> t = sptr<TestObject>::make(new TestObject(1));
        sptr<TestObject> t2 = sptr<TestObject>::make(new TestObject(2));
        t->addRef(t2);
        t2 = sptr<TestObject>::make(new TestObject(3));
        t->addRef(t2);

        wptr<TestObject> w = t;

        w.lock();

        {
            sptr<TestObject> t4 = sptr<TestObject>::make(new TestObject(4));
            w = t4;

            if(!w.lock()) {
                std::cout << "failed!" << std::endl;
            }
        }
        if(w.lock()) {
            std::cout << "failed!" << std::endl;
        }

    } expect({1,2,3,4,-4,-1,-2,-3});

    return 0;
}
