//
// Created by 黄凝霜 on 2024/2/8.
//

#pragma once
#include <future>
#include <thread>
#include <vector>
#include <iostream>
#include <atomic>
#include <stdexcept>
#include <ThreadPool/MTQueue.h>

namespace tp {
    class ThreadPool {
    private:
        using task_t = std::function<void(size_t)>;
        tp::MTQueue<task_t> m_tasks;
        std::vector<std::thread> m_threads;

        std::atomic<bool> m_stop{false};
        std::atomic<bool> m_wait{true}; // stop时是否等待已提交的任务执行完毕
        std::atomic<int> m_waiting{0};

        void init_thread(size_t idx) {
            m_threads[idx] = std::thread([this, idx]() {
                while(true) {
                    ++m_waiting;
                    task_t task = std::move(m_tasks.pop());
                    --m_waiting;
                    if(!task || (m_stop && !m_wait)) break;
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

        int waitingCnt() {return m_waiting;}

        void stop(bool isWait=false) {
            if(m_stop) return;
            m_stop.store(true);
            m_wait.store(isWait);
            if(!isWait) {
                // 清空m_tasks的所有未被消费的任务
                m_tasks.clear();
            }
        }

        template<class F, class ...Params>
        auto push(F &&f, Params&& ...ps) -> std::future<decltype(f(0, ps...))> {
            if(m_stop) throw ;
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
            if(m_stop) throw ;
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
            if(!m_stop || m_wait) {
                for(size_t i=0; i<m_threads.size(); i++) {
                    m_tasks.push(nullptr);
                }
            }
            for(auto &t : m_threads) {
                if(t.joinable()) t.join();
            }
        }
    };
}
