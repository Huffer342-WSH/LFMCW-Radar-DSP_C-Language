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

uint32_t TrackedTarget::next_uuid = 0;


void TrackedTargets::delete_invalid_targets()
{
    for (TrackedTargets::iterator it = this->begin(); it != this->end();) {
        if ((*it).life_cycle.score <= 0) {
            it = this->erase(it);
        } else {
            it++;
        }
    }
}


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
    return (new TrackedTargets)->cast_to_c();
}


/**
 * @brief 删除链表
 *
 * @param list
 */
void tracked_targets_list_delete(tracked_targets_list_t *list)
{
    delete (TrackedTargets::cast_from_c(list));
    return;
}


/**
 * @brief 获取链表的第一个元素
 *
 * @param list 被跟踪目标链表
 * @return tracked_target_t*
 */
tracked_targets_list_node_t tracked_targets_list_first(const tracked_targets_list_t *list)
{
    TrackedTargets *t = TrackedTargets::cast_from_c(list);
    tracked_targets_list_node_t ret;
    TrackedTargets::iterator it = t->begin();
    memcpy(&ret, &it, sizeof(TrackedTargets::iterator));
    return ret;
}


tracked_targets_list_node_t tracked_targets_list_end(const tracked_targets_list_t *list)
{
    TrackedTargets *t = TrackedTargets::cast_from_c(list);
    tracked_targets_list_node_t ret;
    TrackedTargets::iterator it = t->end();
    memcpy(&ret, &it, sizeof(TrackedTargets::iterator));
    return ret;
}

size_t tracked_targets_list_size(const tracked_targets_list_t *list)
{
    TrackedTargets *t = TrackedTargets::cast_from_c(list);
    return t->size();
}

/**
 * @brief 获取当前元素在链表中的下一个元素
 *
 * @param target 目标
 * @return tracked_target_t*
 */
tracked_targets_list_node_t tracked_targets_list_next(const tracked_targets_list_t *list, tracked_targets_list_node_t node)
{
    tracked_targets_list_node_t ret;
    TrackedTargets &t = *TrackedTargets::cast_from_c(list);
    TrackedTargets::iterator it;
    memcpy(&it, &node, sizeof(TrackedTargets::iterator));
    it++;
    if (it == t.end()) {
        memset(&ret, 0, sizeof(ret));
    } else {
        memcpy(&ret, &it, sizeof(TrackedTargets::iterator));
    }
    return ret;
}


/**
 * @brief 返回一个被跟踪目标的ID
 *
 * @param target 目标
 * @param uuid  ID
 * @return int
 */
int tracked_target_get_uuid(const tracked_targets_list_node_t node, uint32_t *uuid)
{
    TrackedTargets::iterator it;
    memcpy(&it, &node, sizeof(TrackedTargets::iterator));
    *uuid = it->uuid;
    return 0;
}


/**
 * @brief 返回一个被跟踪目标的状态向量
 *
 * @param target    目标
 * @param state_vector 指向状态向量首地址的指针
 * @return int
 *
 * @note 返回的状态向量长度为4，分别为[x,vx,y,vy], 其中x是前后方向，y是左右方向，vx、vy是对应的速度。
 */
int tracked_target_get_state_vector(const tracked_targets_list_node_t node, rd_float_t **state_vector)
{
    TrackedTargets::iterator it;
    memcpy(&it, &node, sizeof(TrackedTargets::iterator));
    *state_vector = it->state.state_vector.data();
    return 0;
}


/**
 * @brief 返回目标的生命周期分数
 *
 * @param node 目标
 * @param score  分数
 * @return int
 *
 * @note  目标的新建与删除通过维护分数来管理，分数为0时目标被删除。观察分数变化来调整跟踪器的参数
 */
int tracked_target_get_score(const tracked_targets_list_node_t node, int32_t *score)
{
    TrackedTargets::iterator it;
    memcpy(&it, &node, sizeof(TrackedTargets::iterator));
    *score = it->life_cycle.score;
    return 0;
}

} // extern "C"
