//
// Created by 黄凝霜 on 2024/2/8.
//

#pragma once
#include <future>
#include <thread>
#include <vector>
#include <iostream>
#include <atomic>
#include <ThreadPool/MTQueue.h>

namespace tp {
    class ThreadPool {
    private:
        using task_t = std::function<void(size_t)>;
        tp::MTQueue<task_t> m_tasks;
        std::vector<std::thread> m_threads;

        void init_thread(size_t idx) {
            m_threads[idx] = std::thread([this, idx]() {
                while(true) {
                    task_t task = std::move(m_tasks.pop());
                    if(!task) break;
                    task(idx);
                }
            });
        }
    public:
        explicit ThreadPool(size_t n=std::thread::hardware_concurrency()) {
            m_threads.resize(n);
            for(size_t i=0; i<n; i++) {
                init_thread(i);
            }
        }

        template<class F, class ...Params>
        auto push(F &&f, Params&& ...ps) -> std::future<decltype(f(0, ps...))> {
            auto task = std::make_shared<std::packaged_task<decltype(f(0, ps...))(int)>>([&](size_t idx) {
                return f(idx, std::forward<Params>(ps)...);
            });
            auto fret = task->get_future();
            m_tasks.push([task](size_t idx) {
                (*task)(idx);
            });
            return fret;
        }

        template<class F>
        auto push(F &&f) -> std::future<decltype(f(0))> {
            auto task = std::make_shared<std::packaged_task<decltype(f(0))(int)>>([&](int idx) {
                return f(idx);
            });
            auto fret = task->get_future();
            m_tasks.push([task](int idx) {
                (*task)(idx);
            });
            return fret;
        }

        ~ThreadPool() {
            for(size_t i=0; i<m_threads.size(); i++) {
                m_tasks.push(nullptr);
            }
            for(auto &t : m_threads) {
                if(t.joinable()) t.join();
            }
        }
    };
}
