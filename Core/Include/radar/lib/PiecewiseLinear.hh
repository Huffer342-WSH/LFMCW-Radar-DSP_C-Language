/**
 * @file PiecewiseLinear.hh
 * @author Huffer342-WSH (718007138@qq.com)
 * @brief 分段线性函数（常数 - 线性 - 常数）
 * @version 0.1
 * @date 2025-08-31
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

/**
 * @brief 分段线性函数
 *
 * @tparam T
 */
template <typename T> class PiecewiseLinear {
public:
    constexpr PiecewiseLinear(T x0, T y0, T x1, T y1)
        : x0(x0)
        , x1(x1)
        , y0(y0)
        , y1(y1)
    {
        k = (y1 - y0) / (x1 - x0);
        b = y0 - k * x0;
    }

    constexpr T operator()(T x) const
    {
        if (x <= x0) {
            return y0;
        } else if (x >= x1) {
            return y1;
        } else {
            return k * x + b;
        }
    }

private:
    T x0, x1; ///< 区间
    T y0, y1; ///< 两端常值
    T k, b;   ///< 线性参数
};
