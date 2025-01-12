/**
 * @file track_target.cc
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-08
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "track_target.hh"


/**
 * @brief C语言接口
 *
 */
extern "C" {


/**
 * @brief 新建链表
 *
 * @return tracked_targets_list_t*
 */
tracked_targets_list_t *tracked_targets_list_new()
{
    tracked_targets_list_t *ret;
    ret = reinterpret_cast<tracked_targets_list_t *>(new TrackedTargets);
    return ret;
}


/**
 * @brief 删除链表
 *
 * @param list
 */
void tracked_targets_list_delete(tracked_targets_list_t *list)
{
    TrackedTargets *t = reinterpret_cast<TrackedTargets *>(list);
    delete t;
    return;
}


/**
 * @brief 获取链表的第一个元素
 *
 * @param list 被跟踪目标链表
 * @return tracked_target_t*
 */
tracked_target_t *tracked_targets_list_first(tracked_targets_list_t *list)
{
    tracked_target_t *ret;
    TrackedTargets *t = reinterpret_cast<TrackedTargets *>(list);
    ret = reinterpret_cast<tracked_target_t *>(t->begin()._M_node);
    return ret;
}


/**
 * @brief 获取当前元素在链表中的下一个元素
 *
 * @param target 目标
 * @return tracked_target_t*
 */
tracked_target_t *tracked_targets_list_next(tracked_targets_list_t *list, tracked_target_t *target)
{
    TrackedTargets *t = cast_from_tracked_targets_list(list);
    tracked_target_t *ret;
    std::list<TrackedTarget>::iterator it = std::list<TrackedTarget>::iterator(reinterpret_cast<std::__detail::_List_node_base *>(target));
    if (++it == t->end())
        return NULL;
    ret = reinterpret_cast<tracked_target_t *>(it._M_node);
    return ret;
}


/**
 * @brief 返回一个被跟踪目标的ID
 *
 * @param target 目标
 * @param uuid  ID
 * @return int
 */
int tracked_target_get_uuid(tracked_target_t *target, uint32_t *uuid)
{
    TrackedTarget *t = cast_from_tracked_target(target);
    *uuid = t->uuid;
    return 0;
}


/**
 * @brief 返回一个被跟踪目标的状态向量
 *
 * @param target    目标
 * @param state_vector 指向状态向量首地址的指针
 * @return int
 */
int tracked_target_get_state_vector(tracked_target_t *target, rd_float_t *state_vector)
{
    TrackedTarget *t = cast_from_tracked_target(target);
    state_vector = t->state.state_vector.data();
    return 0;
}
}
