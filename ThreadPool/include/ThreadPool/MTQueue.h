//
// Created by 黄凝霜 on 2024/2/8.
//

#pragma once
#include <queue>
#include <shared_mutex>
#include <condition_variable>

namespace tp {
    template<typename T>
    class MTQueue {
    private:
        std::queue<T> m_q;
        std::condition_variable m_cv;
        mutable std::mutex m_mtx;
    public:
        void push(T const &val) {
            std::unique_lock grd(m_mtx);
            m_q.push(std::move(val));
            m_cv.notify_one();
        }

        T pop() {
            std::unique_lock grd(m_mtx);
            m_cv.wait(grd, [this] { return !m_q.empty(); });
            T ret = std::move(m_q.front());
            m_q.pop();
            return ret;
        }

        void clear() {
            std::unique_lock grd(m_mtx);
            while(!m_q.empty()) {
                m_q.pop();
            }
        }

        bool empty() const {
            std::unique_lock grd(m_mtx);
            return m_q.empty();
        }

        size_t size() const {
            std::unique_lock grd(m_mtx);
            return m_q.size();
        }
    };
}
