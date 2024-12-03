/**
 * @file track_target.hh
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief C语言中的跟踪目标结构体 tracked_targets_list_t 封装成C++类
 * @version 0.1
 * @date 2024-12-03
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include "track_target.h"

#include "radar_log.h"


#include <vector>
#include <Eigen/Dense>


// C++ 封装类
class TrackedTargets
{
private:
    tracked_targets_list_t *list; ///< 保存一个外部的链表
    bool owner;                   ///< 是否拥有这个链表

public:
    /**
     * @brief  TrackedTargets 的构造函数
     *
     * @details 该构造函数传入一个外部的链表，TrackedTargets
     *          不拥有这个链表的所有权
     *
     * @param init_list 传入的链表
     */
    explicit TrackedTargets(tracked_targets_list_t *init_list)
        : list(init_list)
        , owner(false)
    {
        RADAR_ASSERT_EQ((uintptr_t)list, 0);
    }


    explicit TrackedTargets(tracked_targets_list_t *init_list, bool is_owner)
        : list(init_list)
        , owner(is_owner)
    {
        RADAR_ASSERT_EQ((uintptr_t)list, 0);
    }


    ~TrackedTargets()
    {
        clear();
        if (owner) {
            free(list);
        }
    }

    /**
     * @brief 返回链表大小
     *
     * @return size_t
     */
    size_t size() const
    {
        return list->num;
    }

    // 判断链表是否为空
    bool empty() const
    {
        return list->num == 0;
    }


    /**
     * @brief 清空链表
     *
     */
    void clear()
    {
        tracked_targets_list_clear(list);
    }

    /**
     * @brief 在链表头部构造一个目标
     *
     * @param measurement    测量值
     * @param timestamp_ms   时间戳
     */
    void emplace_front(Eigen::Vector3d measurement, uint32_t timestamp_ms)
    {
        tracked_targets_list_emplace_front(list, 4, measurement.data(), timestamp_ms);
    }

    /**
     * @brief 移除一个目标
     *
     * @param target
     */
    void remove(tracked_target_t *target)
    {
        tracked_targets_list_remove(list, target);
    }


    class Iterator
    {
    private:
        tracked_targets_node_t *current;

    public:
        explicit Iterator(tracked_targets_node_t *node)
            : current(node)
        {
        }

        tracked_target_t &operator*()
        {
            return *(current->data);
        }
        Iterator &operator++()
        {
            current = current->next;
            return *this;
        }
        bool operator!=(const Iterator &other) const
        {
            return current != other.current;
        }
    };

    // 返回起始迭代器
    Iterator begin()
    {
        return Iterator(list->head.next);
    }

    // 返回结束迭代器
    Iterator end()
    {
        return Iterator(nullptr);
    }
};
