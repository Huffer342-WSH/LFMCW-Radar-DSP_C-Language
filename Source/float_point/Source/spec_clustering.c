/**
 * @file spec_clustering.c
 * @author risetochallenge (1308368239@qq.com)
 * @brief 将CFAR出的数据在速度维上聚类，并输出聚类结果
 * @version 0.1
 * @date 2024-10-17
 * 
 * @param cfar2d_result_t  输入cfar检测结果，包含numMax(点结构体的数组长度)、numPoint（点结构体里的点数）、point(点结构体：包含RDM的第1、2维座标、幅值、信噪比)
 * 
 噪声水平
 * 
 * @details 删除相同距离单元中幅度小的点，保留幅度大的点；直接在原数组里操作
 * @copyright Copyright (c) 2024
 * 
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "radar_cfar.h"

void spec_clustering(cfar2d_result_t *cfar2d_result) //数组传进来只有1个元素(传入的是地址)
{
    uint32_t id1_max = cfar2d_result->point[0].idx1;
    uint32_t id0_max = cfar2d_result->point[0].idx0;
    double amp_max = cfar2d_result->point[0].amp;

    //结构体里自带元素数量的信息
    //注意幅度不是有序的，距离单元是有序的

    for (int i = 0; i < cfar2d_result->numPoint - 1; i++) {
        int range_nearby = (cfar2d_result->point[i].idx0 == cfar2d_result->point[i + 1].idx0) ? 1 :
                                                                                                0;
        int last_num = (i == cfar2d_result->numPoint - 2) ? 1 : 0; //判断是否是最后一个点

        switch (range_nearby * 10 + last_num) {
        case 00:
            printf("[idx0,idx1,amp]: [%d,%d,%f]\n", id0_max, id1_max, amp_max);
            id0_max = cfar2d_result->point[i + 1].idx0;
            id1_max = cfar2d_result->point[i + 1].idx1;
            amp_max = cfar2d_result->point[i + 1].amp;
            break;
        case 01:
            printf("[idx0,idx1,amp]: [%d,%d,%f]\n", id0_max, id1_max, amp_max);
            printf("[idx0,idx1,amp]: [%d,%d,%f]\n", cfar2d_result->point[i + 1].idx0,
                   cfar2d_result->point[i + 1].idx1, cfar2d_result->point[i + 1].amp);
            break;
        case 10:
            if (amp_max < cfar2d_result->point[i + 1].amp) {
                //删点
                cfar2d_result->point[i].amp = 0;
                cfar2d_result->point[i].snr = 0;

                //cfar2d_result->numPoint--;
                // cfar2d_result->numMax = cfar2d_result->numMax - sizeof(cfar2d_result->point[i]);
                // ---------一个点占多少
                id0_max = cfar2d_result->point[i + 1].idx0;
                id1_max = cfar2d_result->point[i + 1].idx1;
                amp_max = cfar2d_result->point[i + 1].amp;
            } else {
                cfar2d_result->point[i + 1].amp = 0;
                cfar2d_result->point[i + 1].snr = 0;
            }
            break;
        case 11:
            if (amp_max < cfar2d_result->point[i + 1].amp) {
                //删点
                cfar2d_result->point[i].amp = 0;
                cfar2d_result->point[i].snr = 0;

                //cfar2d_result->numPoint--;
                // cfar2d_result->numMax = cfar2d_result->numMax - sizeof(cfar2d_result->point[i]);
                // ---------一个点占多少
                id0_max = cfar2d_result->point[i + 1].idx0;
                id1_max = cfar2d_result->point[i + 1].idx1;
                amp_max = cfar2d_result->point[i + 1].amp;
                printf("[idx0,idx1,amp]: [%d,%d,%f]\n", id0_max, id1_max, amp_max);
            } else {
                printf("[idx0,idx1,amp]: [%d,%d,%f]\n", id0_max, id1_max, amp_max);

                cfar2d_result->point[i + 1].amp = 0;
                cfar2d_result->point[i + 1].snr = 0;
            }



            break;
        default:
            printf("spec_clustering error!\n");
            break;
        }
    }
    printf("point_clean_check\n");

    for (int i = 0; i < cfar2d_result->numPoint; i++) {
        printf("[idx0,idx1,amp]: [%d,%d,%f]\n", cfar2d_result->point[i].idx0,
               cfar2d_result->point[i].idx1, cfar2d_result->point[i].amp);
    }
}
/*  在目标里从速度维聚类*/


void cfar2d_del_point(cfar2d_result_t *list)
{
    cfar2d_point_t *pLeft, *pRight, *pEnd;
    pLeft = list->point;
    pRight = pLeft + 1;
    pEnd = &list->point[list->numPoint];
    while (pRight < pEnd) {
        // 速度维度相邻
        if (pLeft->idx0 == pRight->idx0 && pLeft->idx1 + 1 == pRight->idx1) {
            if (pLeft->amp < pRight->amp ||
                (pLeft->amp == pRight->amp && pLeft->snr < pRight->snr)) {
                //  左 < 右
                pLeft->amp = 0; //标记为删除掉的点
            } else {
                //  左 > 右
                pRight->amp = 0;
            }
        }
        pRight++;
        pLeft++;
    }
    // 删除不需要的点
    for (pRight = pLeft = list->point; pRight < pEnd; pRight++) {
        if (pRight->amp != 0) {
            if (pLeft != pRight)
                memcpy(pLeft, pRight, sizeof(cfar2d_point_t));
            pLeft++;
        }
    }
    list->numPoint = pLeft - list->point;
}
