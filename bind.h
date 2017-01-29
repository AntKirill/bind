#ifndef bind_h
#define bind_h

#include <functional>
#include <iostream>
#include <string>

template<int N>
struct place_holder {};
constexpr place_holder<0> _1{};
constexpr place_holder<1> _2{};
constexpr place_holder<2> _3{};
constexpr place_holder<3> _4{};
constexpr place_holder<4> _5{};
constexpr place_holder<5> _6{};
constexpr place_holder<6> _7{};

template<typename F, typename ... Args>
struct bind_t {

    template<typename Ff, typename... Args_>
    bind_t(Ff &&func, Args_ &&... args) : func(std::forward<Ff>(func)), base_args(std::forward<Args_>(args)...) { }

    template<typename... New_args> auto operator()(New_args&& ... new_args) {
        return call_func(typename seq_getter<std::tuple_size<tpl>::value>::type(), std::forward<New_args> (new_args)...);
    }

private:
    typename std::decay<F>::type func;

    typedef std::tuple<std::decay_t<Args>...> tpl;

    tpl base_args;

    template<int... S> struct seq {};

    template<int S, int... Tail> struct seq_getter {
        typedef typename seq_getter<S - 1, S - 1, Tail ...>::type type;
    };

    template<int... Tail> struct seq_getter<0, Tail...> {
        typedef seq<Tail...> type;
    };

    template <typename Base_args, typename ... New_args> decltype(auto) go(Base_args &base_arg, New_args &... new_args) {
        return base_arg;
    };

    template <int K, typename ... New_args> decltype(auto) go(place_holder<K> &plh, New_args &... new_args) {
        return std::get<K>(std::forward_as_tuple(new_args...));
    };

    template <typename Ff, typename ... Base_args, typename ...New_args>
    decltype(auto) go(bind_t<Ff, Base_args...> &other_bind, New_args &... new_args) {
        return other_bind(new_args ...);
    };

    template <typename ... New_args, int ... K> auto call_func(const seq<K...> & s, New_args&& ... new_args) {
        return func(go(std::get<K>(base_args), new_args...)...);
    };

};

template<typename F, typename ... Args>
bind_t<F, Args...> bind(F&& func, Args&& ... args) {
    return bind_t<F, Args...>(std::forward<F>(func), std::forward<Args>(args)...);
};

#endif