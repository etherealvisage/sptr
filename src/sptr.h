#ifndef SPTR_H
#define SPTR_H

template<typename T>
class sptr_helper {
private:
    T *m_target;
    int m_scount;
    int m_wcount;
public:
    sptr_helper(T *target) : m_target(target), m_scount(1), m_wcount(0) {}

    T *target() const { return m_target; }

    // need to do atomic increment here to be threadsafe
    void incs() { if(m_scount > 0) m_scount ++; }
    void incw() { m_wcount ++ ; }

    void decs() { m_scount --; check(); }
    void decw() { m_wcount --; check(); }

    bool alive() {
        return m_scount > 0;
    }

    bool check() {
        if(m_scount < 0) {
            std::cout << "negative scount!" << std::endl;
        }
        if(m_wcount < 0) {
            std::cout << "negative wcount!" << std::endl;
        }
        if(m_target && m_scount == 0) {
            delete m_target;
            m_target = 0;
        }
        if(m_scount == 0 && m_wcount == 0) return false;
        return true;
    }
};

template<typename T>
class wptr;

template<typename T>
class sptr {
private:
    friend class wptr<T>;
    sptr_helper<T> *m_helper;
public:
    sptr() {
        m_helper = 0;
    }
    sptr(const sptr &other) {
        m_helper = other.m_helper;
        if(m_helper) m_helper->incs();
    }
    sptr(sptr_helper<T> *helper) {
        m_helper = helper;
        if(m_helper) m_helper->incs();
    }
    sptr(T *object) {
        *this = make(object);
    }

    ~sptr() {
        reset();
    }

    void reset() {
        if(m_helper) {
            m_helper->decs();
            check();
        }
    }

    static sptr<T> make(T *object) {
        sptr<T> s;
        s.m_helper = new sptr_helper<T>(object);
        return s;
    }

    T *operator->() const {
        if(m_helper) return m_helper->target();
        else return 0;
    }

    operator T*() const {
        if(m_helper) return m_helper->target();
        else return 0;
    }

    sptr<T> &operator=(const sptr<T> &other) {
        reset();
        m_helper = other.m_helper;
        if(m_helper) m_helper->incs();
        return *this;
    }
private:
    void check() {
        if(m_helper && !m_helper->check()) {
            delete m_helper;
            m_helper = 0;
        }
    }
};

template<typename T>
class wptr {
public:
    sptr_helper<T> *m_helper;
public:
    wptr() {
        m_helper = 0;
    }
    wptr(const wptr &other) {
        m_helper = other.m_helper;
        if(m_helper) m_helper->incw();
    }

    wptr(const sptr<T> &ptr) {
        m_helper = ptr.m_helper;
        m_helper->incw();
    }

    ~wptr() {
        reset();
    }

    void reset() {
        if(m_helper) {
            m_helper->decw();
            check();
        }
    }

    sptr<T> lock() const {
        if(!m_helper->alive()) return sptr<T>();

        return sptr<T>(m_helper);
    }

    wptr<T> &operator=(const wptr<T> &other) {
        reset();
        m_helper = other.m_helper;
        if(m_helper) m_helper->incw();
        return *this;
    }
private:
    void check() {
        if(m_helper && !m_helper->check()) {
            delete m_helper;
            m_helper = 0;
        }
    }
};

#endif
